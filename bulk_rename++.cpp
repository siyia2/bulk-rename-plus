#include <iostream>
#include <string>
#include <filesystem>
#include <algorithm>
#include <vector>
#include <thread>
#include <mutex>
#include <unistd.h>
#include <chrono>
#include <iomanip>

namespace fs = std::filesystem;

bool verbose_enabled = false;
std::mutex cout_mutex;
std::mutex input_mutex;

void print_message(const std::string& message) {
    if (verbose_enabled) {
        std::lock_guard<std::mutex> lock(cout_mutex);
        std::cout << message << std::endl;
    }
}

void print_error(const std::string& error) {
    std::lock_guard<std::mutex> lock(cout_mutex);
    std::cerr << error << std::endl;
}

void print_help() {
    std::cout << "Usage: rename [OPTIONS] [PATHS]\n"
              << "Renames all files and folders under the specified path(s).\n"
              << "\n"
              << "Options:\n"
              << "  -h, --help           Print this message and exit\n"
              << "  -c  [MODE]           Set the case conversion mode (lower/upper/fupper/reverse) w/o parent dir(s)\n"
              << "  -cp [MODE]           Set the case conversion mode (lower/upper/fupper/reverse) w parent dir(s)\n"
              << "  -v, --verbose        Enable verbose mode\n"
              << "\n"
              << "Examples:\n"
              << "  bulk_rename++ /path/to/folder1 /path/to/folder2 -c lower\n"
              << "  bulk_rename++ /path/to/folder -cp upper\n"
              << "  bulk_rename++ /path/to/folder -v -cp upper\n"
              << "  bulk_rename++ /path/to/folder -c upper -v\n"
              << "\n";
}

std::string fupper(const std::string& word) {
    std::string result = word;
    if (!result.empty()) {
        result[0] = std::toupper(result[0]);
        std::transform(result.begin() + 1, result.end(), result.begin() + 1, ::tolower);
    }
    return result;
}

void rename_item(const fs::path& item_path, const std::string& case_input, bool is_directory, bool verbose, int& files_count, int& dirs_count) {
    std::string name = item_path.filename().string();
    std::string new_name;
    new_name.resize(name.size()); // Resize new_name

    std::transform(name.begin(), name.end(), new_name.begin(), [case_input](unsigned char c) -> unsigned char {
        if (case_input == "lower") {
            return std::tolower(c);
        } else if (case_input == "upper") {
            return std::toupper(c);
        } else if (case_input == "reverse") {
            return std::islower(c) ? std::toupper(c) : std::tolower(c);
        } else {
            return c;
        }
    });

if (case_input == "fupper") {
    // Special handling for filenames like "1_file" or "1-file"
    bool found_non_letter = false; // Flag to track if a non-letter character is found
    for (size_t i = 0; i < new_name.size(); ++i) {
        // Check if the current character is not a letter
        if (!isalpha(new_name[i])) {
            found_non_letter = true; // Set the flag to true when a non-letter character is found
        }
        // Check if the current character is a letter and follows a non-letter character
        else if (found_non_letter) {
            // Capitalize the letter character
            new_name[i] = std::toupper(new_name[i]);
            found_non_letter = false; // Reset the flag since we found the first letter after a non-letter character
        }
        else {
            // Lowercase the character if it's not already lowercase
            if (std::isupper(new_name[i])) {
                new_name[i] = std::tolower(new_name[i]);
            }
        }
    }
}
    fs::path new_path = item_path.parent_path() / new_name;

    try {
        fs::rename(item_path, new_path);
        if (verbose) {
            std::string item_type = is_directory ? "directory" : "file";
            print_message("\033[92mRenamed\033[0m " + item_type + " " + item_path.string() + " to " + new_path.string());
        }
        if (!is_directory) {
            ++files_count; // Increment files count
        } else {
            ++dirs_count; // Increment directories count
        }
    } catch (const std::filesystem::filesystem_error& e) {
        print_error("\033[91mError\033[0m: " + std::string(e.what()));
    }
}

void rename_directory(const fs::path& directory_path, const std::string& case_input, bool rename_immediate_parent, bool verbose, int& files_count, int& dirs_count) {
    fs::path new_path = directory_path;
    std::string dirname = directory_path.filename().string();
    std::string new_dirname;
    new_dirname.resize(dirname.size());

    std::transform(dirname.begin(), dirname.end(), new_dirname.begin(), [case_input, first_letter = true](unsigned char c) mutable -> unsigned char {
        if (case_input == "lower") {
            return std::tolower(c);
        } else if (case_input == "upper") {
            return std::toupper(c);
        } else if (case_input == "reverse") {
            return std::islower(c) ? std::toupper(c) : std::tolower(c);
        } else if (case_input == "fupper") {
            if (!std::isalpha(c)) {
                first_letter = true; // Reset the flag when encountering non-letter character
                return c;
            }
            if (first_letter) {
                c = std::toupper(c);
                first_letter = false; // Set the flag to false after capitalizing the first letter
            } else {
                c = std::tolower(c);
            }
            return c;
        } else {
            return c;
        }
    });

    new_path.remove_filename(); // Remove the last component (file name) from the path
    new_path /= new_dirname; // Append the new directory name

    try {
        fs::rename(directory_path, new_path);
        if (verbose) {
            print_message("\033[94mRenamed\033[0m directory " + directory_path.string() + " to " + new_path.string());
        }
        ++dirs_count; // Increment directories count
    } catch (const std::filesystem::filesystem_error& e) {
        print_error("\033[1;91mError\033[0m: " + std::string(e.what()));
    }

    // Recursively rename all contents within the directory
    for (const auto& entry : fs::directory_iterator(new_path)) {
        if (entry.is_directory()) {
            rename_directory(entry.path(), case_input, rename_immediate_parent, verbose, files_count, dirs_count);
        } else {
            rename_item(entry.path(), case_input, false, verbose, files_count, dirs_count);
        }
    }

    if (rename_immediate_parent) {
        // If rename_immediate_parent is true, do not recursively rename
        return;
    }
}


void rename_path(const std::vector<std::string>& paths, const std::string& case_input, bool rename_immediate_parent, bool verbose = true) {
    // Check if case_input is empty
    if (case_input.empty()) {
        print_error("\033[1;91mError: Case conversion mode not specified (-c option is required)\033[0m");
        return;
    }

    std::vector<std::thread> threads;

    unsigned int max_threads = std::thread::hardware_concurrency();
    if (max_threads == 0) {
        max_threads = 2; // If hardware concurrency is not available, default to 1 thread
    }

    auto start_time = std::chrono::steady_clock::now(); // Start time measurement

    int files_count = 0; // Initialize files count
    int dirs_count = 0;  // Initialize directories count

    for (const auto& path : paths) {
        fs::path current_path(path);

        if (fs::exists(current_path)) {
            if (fs::is_directory(current_path)) {
                if (rename_immediate_parent) {
                    // If -p option is used, only rename the immediate parent
                    fs::path immediate_parent_path = current_path.parent_path();
                    rename_directory(immediate_parent_path, case_input, rename_immediate_parent, verbose, files_count, dirs_count);
                } else {
                    // Otherwise, rename the entire path
                    if (threads.size() < max_threads) {
                        threads.emplace_back(rename_directory, current_path, case_input, rename_immediate_parent, verbose, std::ref(files_count), std::ref(dirs_count));
                    } else {
                        rename_directory(current_path, case_input, rename_immediate_parent, verbose, files_count, dirs_count);
                    }
                }
            } else if (fs::is_regular_file(current_path)) {
                // For files, directly rename the item without considering the parent directory
                rename_item(current_path, case_input, false, verbose, files_count, dirs_count);
            } else {
                print_error("\033[1;91mError: specified path is neither a directory nor a regular file\033[0m\n");
            }
        } else {
            print_error("\033[1;91mError: path does not exist - " + path + "\033[0m\n");
        }
    }

    // Wait for all threads to finish
    for (auto& thread : threads) {
        thread.join();
    }

    auto end_time = std::chrono::steady_clock::now(); // End time measurement

    std::chrono::duration<double> elapsed_seconds = end_time - start_time; // Calculate elapsed time

    std::cout << "\n\033[1mRenamed to "<< case_input <<"_case: \033[1;92m" << files_count << " file(s) \033[0m\033[1mand \033[1;94m" 
              << dirs_count << " dir(s) \033[0m\033[1mfrom \033[1;95m" << paths.size() 
              << " input path(s) \033[0m\033[1min " << std::setprecision(1) 
              << std::fixed << elapsed_seconds.count() << "\033[1m second(s)\n";
}

int main(int argc, char *argv[]) {
    std::vector<std::string> paths;
    std::string case_input;
    bool rename_parents = false;

    bool case_specified = false;

    if (argc >= 2) {
        for (int i = 1; i < argc; ++i) {
            std::string arg(argv[i]);
            if (arg == "-v" || arg == "--verbose") {
                verbose_enabled = true;
            } else if (arg == "-h" || arg == "--help") {
                print_help();
                return 0;
            } else if (arg == "-cp") {
                rename_parents = true;
                if (i + 1 < argc) {
                    case_input = argv[++i];
                    case_specified = true;
                    // Check if the case mode is valid
                    if (case_input != "lower" && case_input != "upper" && case_input != "reverse" && case_input != "fupper") {
                        print_error("\033[1;91mError: Unspecified case mode. Please specify 'lower', 'upper', 'reverse', or 'fupper'.\n");
                        return 1;
                    }
                } else {
                    print_error("\033[1;91mError: Missing argument for option -cp\n");
                    return 1;
                }
            } else if (arg == "-c") {
                if (i + 1 < argc) {
                    case_input = argv[++i];
                    case_specified = true;
                    // Check if the case mode is valid
                    if (case_input != "lower" && case_input != "upper" && case_input != "reverse" && case_input != "fupper") {
                        print_error("\033[1;91mError: Unspecified case mode. Please specify 'lower', 'upper', 'reverse', or 'fupper'.\n");
                        return 1;
                    }
                } else {
                    print_error("\033[1;91mError: Missing argument for option -c\n");
                    return 1;
                }
            } else {
                paths.emplace_back(arg);
            }
        }
    }

    if (!case_specified) {
        print_error("\033[1;91mError: Case conversion mode not specified (-c or -cp option is required)\033[0m\n");
        return 1;
    }

    for (const auto& path : paths) {
        if (!fs::exists(path)) {
            print_error("\033[1;91mError: Path does not exist - " + path + "\033[0m\n");
            return 1;
        }
    }

    std::system("clear");
    std::cout << "\033[1;93m!!! WARNING OPERATION IRREVERSIBLE !!!\033[0m\n\n";

    std::string confirmation;
    if (rename_parents) {
        std::cout << "\033[1mThe following path(s), along with their parent dir(s), will be recursively renamed to \033[1;92m"<< case_input <<"_case\033[0m:\033[1m\n\n";
        for (const auto& path : paths) {
            std::cout << "\033[1;95m" << path << "\033[0m" << std::endl;
        }
    } else {
        std::cout << "\033[1mThe following path(s) will be recursively renamed to \033[1;92m"<< case_input <<"_case\033[0m:\033[1m\n\n";
        for (const auto& path : paths) {
            std::cout << "\033[1;94m" << path << "\033[0m" << std::endl;
        }
    }
    std::cout << "\n\033[1mDo you want to proceed? (y/n): ";
    std::getline(std::cin, confirmation);

    if (confirmation != "y") {
        std::cout << "\n\033[1;91mOperation aborted by user.\n\033[0m\n";
        std::cout << "\033[1mPress enter to exit...";
        std::cin.get();
        std::system("clear");
        return 0;
    }
    if (verbose_enabled) {
        std::cout << "\n";
    }
    
    rename_path(paths, case_input, rename_parents, verbose_enabled);
    
    std::cout << "\n";
    
    std::cout << "\033[1mPress enter to exit...";
    std::cin.get();
    std::system("clear");

    return 0;
}
