#include <iostream>
#include <string>
#include <filesystem>
#include <algorithm>
#include <vector>
#include <thread>
#include <mutex>
#include <unistd.h>
#include <chrono>
#include <regex>

namespace fs = std::filesystem;

bool verbose_enabled = false;
std::mutex cout_mutex;
std::mutex input_mutex;

std::mutex dirs_count_mutex;
std::mutex files_count_mutex;

// General purpose stuff

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


void print_verbose_enabled(const std::string& message, bool verbose_enabled) {
    if (verbose_enabled) {
        std::lock_guard<std::mutex> lock(cout_mutex);
        std::cout << message << std::endl;
    }
}



void print_help() {
    std::cout << "Usage: rename [OPTIONS] [PATHS]\n"
              << "Renames all files and folders under the specified path(s).\n"
              << "\n"
              << "Options:\n"
              << "  -h, --help           Print this message and exit\n"
              << "  -c  [MODE]           Set the case conversion mode (lower/upper/fupper/reverse/rspace/runderscore) w/o parent dir(s)\n"
              << "  -cp [MODE]           Set the case conversion mode (lower/upper/fupper/reverse/rspace/runderscore) w parent dir(s)\n"
              << "  -ce [MODE]           Set the case conversion mode (lower/upper/fupper/reverse) for file extension(s)\n"
              << "  -v, --verbose_enabled        Enable verbose_enabled mode\n"
              << "\n"
              << "Examples:\n"
              << "  bulk_rename++ [path1] [path2]... -c lower\n"
              << "  bulk_rename++ [path1] -cp upper\n"
              << "  bulk_rename++ [path1] -ce upper -v\n"
              << "  bulk_rename++ [path1] -v -cp upper\n"
              << "  bulk_rename++ [path1] -c upper -v\n"
              << "\n";
}

// Extension stuff

void rename_extension(const fs::path& item_path, const std::string& case_input, bool verbose_enabled, int& files_count, int& dirs_count) {
    std::string extension = item_path.extension().string();
    std::string new_extension = extension; // Initialize with original extension

    // Regular expression patterns for transformation
    std::regex lower_case("([a-zA-Z]+)");
    std::regex upper_case("([a-zA-Z]+)");
    std::regex reverse_case("([a-zA-Z])");
    std::regex fupper_case("([a-zA-Z])([a-zA-Z.]*)"); // Capture first letter separately for fupper

    if (case_input == "lower") {
        std::transform(new_extension.begin(), new_extension.end(), new_extension.begin(), ::tolower);
    } else if (case_input == "upper") {
        std::transform(new_extension.begin(), new_extension.end(), new_extension.begin(), ::toupper);
    } else if (case_input == "reverse") {
        std::smatch match;
        if (std::regex_search(extension, match, reverse_case)) {
            for (auto& c : match[1].str()) {
                if (std::islower(c)) {
                    new_extension += std::toupper(c);
                } else {
                    new_extension += std::tolower(c);
                }
            }
        }
    } else if (case_input == "fupper") {
    std::smatch match;
    if (std::regex_search(extension, match, fupper_case)) {
        std::string rest_of_extension = match[2].str();
        std::transform(rest_of_extension.begin(), rest_of_extension.end(), rest_of_extension.begin(), ::tolower);
        new_extension = "." + std::string(1, std::toupper(match[1].str()[0])) + rest_of_extension;
    }
}

    // Skip renaming if the new extension is the same as the old extension
    if (extension != new_extension) {
        fs::path new_path = item_path.parent_path() / (item_path.stem().string() + new_extension);

        try {
            fs::rename(item_path, new_path);
            
            if (verbose_enabled) {
                print_verbose_enabled("\033[0m\033[92mRenamed\033[0m file " + item_path.string() + " to " + new_path.string(), verbose_enabled);
            }
            std::lock_guard<std::mutex> lock(files_count_mutex);
            ++files_count;
        } catch (const fs::filesystem_error& e) {
            std::cerr << "\033[1;91mError\033[0m: " << e.what() << "\n" << std::endl;
        }
    } else {
        if (verbose_enabled) {
            if (extension.empty()) {
                print_verbose_enabled("\033[0m\033[93mSkipped\033[0m file " + item_path.string() + " (no extension)", verbose_enabled);
            } else {
                print_verbose_enabled("\033[0m\033[93mSkipped\033[0m file " + item_path.string() + " (extension unchanged)", verbose_enabled);
            }
        }
    }
}


void rename_extension_path(const std::vector<std::string>& paths, const std::string& case_input, bool verbose_enabled) {
    // Check if case_input is empty
    if (case_input.empty()) {
        print_error("\033[1;91mError: Case conversion mode not specified (-ce option is required)\n\033[0m");
        return;
    }

    auto start_time = std::chrono::steady_clock::now(); // Start time measurement

    int files_count = 0; // Initialize files count

    for (const auto& path : paths) {
        fs::path current_path(path);

        if (fs::exists(current_path)) {
            if (fs::is_directory(current_path)) {
                // For directories, recursively rename all files within the directory
                for (const auto& entry : fs::recursive_directory_iterator(current_path)) {
                    if (fs::is_regular_file(entry.path())) {
                        rename_extension(entry.path(), case_input, verbose_enabled, files_count, files_count);
                    }
                }
            } else if (fs::is_regular_file(current_path)) {
                // For individual files, directly rename the file
                rename_extension(current_path, case_input, verbose_enabled, files_count, files_count);
            } else {
                print_error("\033[1;91mError: specified path is neither a directory nor a regular file\033[0m\n");
            }
        } else {
            print_error("\033[1;91mError: path does not exist - " + path + "\033[0m\n");
        }
    }

    auto end_time = std::chrono::steady_clock::now(); // End time measurement

    std::chrono::duration<double> elapsed_seconds = end_time - start_time; // Calculate elapsed time

    std::cout << "\n\033[1mRenamed extensions to " << case_input << "_case: \033[1;92m" << files_count << " file(s) \033[0m\033[1mfrom \033[1;95m" << paths.size()
              << " input path(s) \033[0m\033[1min " << std::setprecision(1)
              << std::fixed << elapsed_seconds.count() << "\033[1m second(s)\n";
}

// Rename file&directory stuff

void rename_item(const fs::path& item_path, const std::string& case_input, bool is_directory, bool verbose_enabled, int& files_count, int& dirs_count) {
    std::string name = item_path.filename().string();
    std::string new_name = name; // Initialize with original name
    fs::path new_path; // Declare new_path here to make it accessible in both branches
    
    static std::regex lower_pattern("lower");
    static std::regex upper_pattern("upper");
    static std::regex reverse_pattern("reverse");
    static std::regex fupper_pattern("fupper");
    static std::regex rspace_pattern("rspace");
    static std::regex runderscore_pattern("runderscore");

    if (fs::is_symlink(item_path)) {
        if (verbose_enabled) {
            print_verbose_enabled("\033[0m\033[93mSkipped\033[0m symlink " + item_path.string() + " (not supported)", verbose_enabled);
        }
        return;
    }

    if (std::regex_match(case_input, lower_pattern)) {
        std::transform(new_name.begin(), new_name.end(), new_name.begin(), ::tolower);
    } else if (std::regex_match(case_input, upper_pattern)) {
        std::transform(new_name.begin(), new_name.end(), new_name.begin(), ::toupper);
    } else if (std::regex_match(case_input, reverse_pattern)) {
        std::transform(new_name.begin(), new_name.end(), new_name.begin(), [](unsigned char c) {
            return std::islower(c) ? std::toupper(c) : std::tolower(c);
        });
    } else if (std::regex_match(case_input, fupper_pattern)) {
        bool first_letter_encountered = true;
        for (char& c : new_name) {
            if (std::isalpha(c)) {
                if (first_letter_encountered) {
                    c = std::toupper(c);
                    first_letter_encountered = false;
                } else {
                    c = std::tolower(c); // Convert subsequent letters to lowercase
                }
            }
        }
    } else if (std::regex_match(case_input, rspace_pattern)) {
        std::regex_replace(std::back_inserter(new_name), new_name.begin(), new_name.end(), std::regex(" "), "_");
    } else if (std::regex_match(case_input, runderscore_pattern)) {
        std::regex_replace(std::back_inserter(new_name), new_name.begin(), new_name.end(), std::regex("_"), " ");
    }

    if (name != new_name) {
        new_path = item_path.parent_path() / new_name; // Move assignment to avoid copy
        
        try {
            fs::rename(item_path, new_path);
            
            if (verbose_enabled) {
                print_verbose_enabled("\033[0m\033[92mRenamed\033[0m file " + item_path.string() + " to " + new_path.string(), verbose_enabled);
            }
            if (!is_directory) {
			std::lock_guard<std::mutex> lock(files_count_mutex);
                ++files_count;
            } else {
				std::lock_guard<std::mutex> lock(dirs_count_mutex);
                ++dirs_count;
            }
        } catch (const fs::filesystem_error& e) {
            std::cerr << "\033[1;91mError\033[0m: " << e.what() << "\n" << std::endl;
        }
    } else {
        if (verbose_enabled) {
            print_verbose_enabled("\033[0m\033[93mSkipped\033[0m file " + item_path.string() + " (name unchanged)", verbose_enabled);
        }
    }
}



void rename_directory(const fs::path& directory_path, const std::string& case_input, bool rename_immediate_parent, bool verbose_enabled, int& files_count, int& dirs_count) {
    std::string dirname = directory_path.filename().string();
    std::string new_dirname; // Initialize with original name

    // Regular expression patterns for transformations
    std::regex lower_pattern("lower");
    std::regex upper_pattern("upper");
    std::regex reverse_pattern("reverse");
    std::regex fupper_pattern("fupper");
    std::regex rspace_pattern("rspace");
    std::regex runderscore_pattern("runderscore");

    if (fs::is_symlink(directory_path)) {
        if (verbose_enabled) {
            print_verbose_enabled("\033[0m\033[93mSkipped\033[0m symlink " + directory_path.string() + " (not supported)", verbose_enabled);
        }
        return;
    }

    // Apply case transformation using regex patterns
    if (std::regex_match(case_input, lower_pattern)) {
        new_dirname = dirname;
        std::transform(new_dirname.begin(), new_dirname.end(), new_dirname.begin(), ::tolower);
    } else if (std::regex_match(case_input, upper_pattern)) {
        new_dirname = dirname;
        std::transform(new_dirname.begin(), new_dirname.end(), new_dirname.begin(), ::toupper);
    } else if (std::regex_match(case_input, reverse_pattern)) {
        new_dirname = dirname;
        std::transform(new_dirname.begin(), new_dirname.end(), new_dirname.begin(), [](unsigned char c) {
            return std::islower(c) ? std::toupper(c) : std::tolower(c);
        });
    } else if (std::regex_match(case_input, fupper_pattern)) {
        bool first_letter = true;
        new_dirname.reserve(dirname.size()); // Reserve space for efficiency
        for (char c : dirname) {
            if (std::isalpha(c)) {
                if (first_letter) {
                    new_dirname.push_back(std::toupper(c));
                    first_letter = false;
                } else {
                    new_dirname.push_back(std::tolower(c));
                }
            } else {
                new_dirname.push_back(c);
            }
        }
    } else if (std::regex_match(case_input, rspace_pattern)) {
        std::replace(dirname.begin(), dirname.end(), ' ', '_');
        new_dirname = dirname;
    } else if (std::regex_match(case_input, runderscore_pattern)) {
        std::replace(dirname.begin(), dirname.end(), '_', ' ');
        new_dirname = dirname;
    }

    fs::path new_path = directory_path.parent_path() / std::move(new_dirname); // Move new_dirname instead of copying

    // Check if renaming is necessary
    if (directory_path != new_path) {
        try {
            fs::rename(directory_path, new_path);

            if (verbose_enabled) {
                print_verbose_enabled("\033[0m\033[94mRenamed\033[0m directory " + directory_path.string() + " to " + new_path.string(), verbose_enabled);
            }
            std::lock_guard<std::mutex> lock(dirs_count_mutex);
            ++dirs_count;
        } catch (const fs::filesystem_error& e) {
            std::cerr << "\033[1;91mError\033[0m: " << e.what() << "\n" << std::endl;
            return; // Stop processing if renaming failed
        }
    } else {
        if (verbose_enabled) {
            print_verbose_enabled("\033[0m\033[93mSkipped\033[0m directory " + directory_path.string() + " (name unchanged)", verbose_enabled);
        }
    }

    unsigned int max_threads = std::thread::hardware_concurrency();
    if (max_threads == 0) {
        max_threads = 1; // If hardware concurrency is not available, default to 1 thread
    }

    // Process items within the directory
    std::vector<std::thread> threads;
    for (const auto& entry : fs::directory_iterator(new_path)) {
        if (entry.is_directory()) {
            if (threads.size() < max_threads) {
                // Start a new thread for each subdirectory
                threads.emplace_back(rename_directory, entry.path(), case_input, false, verbose_enabled, std::ref(files_count), std::ref(dirs_count));
            } else {
                // Process directories in the main thread if max_threads is reached
                rename_directory(entry.path(), case_input, false, verbose_enabled, files_count, dirs_count);
            }
        } else {
            // Process files in the main thread
            rename_item(entry.path(), case_input, false, verbose_enabled, files_count, dirs_count);
        }
    }

    // Join all threads
    for (auto& thread : threads) {
        thread.join();
    }
}


void rename_path(const std::vector<std::string>& paths, const std::string& case_input, bool rename_immediate_parent, bool verbose_enabled = false) {
    // Check if case_input is empty
    if (case_input.empty()) {
        print_error("\033[1;91mError: Case conversion mode not specified (-c option is required)\n\033[0m");
        return;
    }

    std::vector<std::thread> threads;

    unsigned int max_threads = std::thread::hardware_concurrency();
    if (max_threads == 0) {
        max_threads = 1; // If hardware concurrency is not available, default to 1 thread
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
                    rename_directory(immediate_parent_path, case_input, rename_immediate_parent, verbose_enabled, files_count, dirs_count);
                } else {
                    // Otherwise, rename the entire path
                    if (threads.size() < max_threads) {
                        threads.emplace_back(rename_directory, current_path, case_input, rename_immediate_parent, verbose_enabled, std::ref(files_count), std::ref(dirs_count));
                    } else {
                        rename_directory(current_path, case_input, rename_immediate_parent, verbose_enabled, files_count, dirs_count);
                    }
                }
            } else if (fs::is_regular_file(current_path)) {
                // For files, directly rename the item without considering the parent directory
                rename_item(current_path, case_input, false, verbose_enabled, files_count, dirs_count);
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

    std::cout << "\n\033[1mRenamed to " << case_input << "_case: \033[1;92m" << files_count << " file(s) \033[0m\033[1mand \033[1;94m"
              << dirs_count << " dir(s) \033[0m\033[1mfrom \033[1;95m" << paths.size()
              << " input path(s) \033[0m\033[1min " << std::setprecision(1)
              << std::fixed << elapsed_seconds.count() << "\033[1m second(s)\n";
}


int main(int argc, char *argv[]) {
    std::vector<std::string> paths;
    std::string case_input;
    bool rename_parents = false;
    bool rename_extensions = false;

    bool case_specified = false;

    if (argc >= 2) {
        for (int i = 1; i < argc; ++i) {
            std::string arg(argv[i]);
            if (arg == "-v" || arg == "--verbose_enabled") {
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
                    if (case_input != "lower" && case_input != "upper" && case_input != "reverse" && case_input != "fupper" && case_input != "rspace" && case_input != "runderscore") {
                        print_error("\033[1;91mError: Unspecified case mode. Please specify 'lower', 'upper', 'reverse', 'fupper', 'rspace', or 'runderscore'.\n");
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
                    if (case_input != "lower" && case_input != "upper" && case_input != "reverse" && case_input != "fupper" && case_input != "rspace" && case_input != "runderscore") {
                        print_error("\033[1;91mError: Unspecified case mode. Please specify 'lower', 'upper', 'reverse', 'fupper', 'rspace', or 'runderscore'.\n");
                        return 1;
                    }
                } else {
                    print_error("\033[1;91mError: Missing argument for option -c\n");
                    return 1;
                }
            } else if (arg == "-ce") {
                rename_extensions = true;
                if (i + 1 < argc) {
                    case_input = argv[++i];
                    case_specified = true;
                    // Check if the case mode is valid
                    if (case_input != "lower" && case_input != "upper" && case_input != "reverse" && case_input != "fupper") {
                        print_error("\033[1;91mError: Unspecified case mode. Please specify 'lower', 'upper', 'reverse', 'fupper'.\n");
                        return 1;
                    }
                } else {
                    print_error("\033[1;91mError: Missing argument for option -ce\n");
                    return 1;
                }
            } else {
                paths.emplace_back(arg);
            }
        }
    }

    if (!case_specified) {
        print_error("\033[1;91mError: Case conversion mode not specified (-c, -cp, or -ce option is required)\033[0m\n");
        return 1;
    }

    for (const auto& path : paths) {
        if (!fs::exists(path)) {
            print_error("\033[1;91mError: Path does not exist - " + path + "\033[0m\n");
            return 1;
        }
    }

    std::system("clear");
    if (case_input != "rspace" && case_input != "runderscore"){
    std::cout << "\033[1;93m!!! WARNING OPERATION IRREVERSIBLE !!!\033[0m\n\n";
	}

    std::string confirmation;
    if (rename_parents) {
        std::cout << "\033[1mThe following path(s), along with their parent dir(s), will be recursively renamed to \033[1;92m" << case_input << "_case\033[0m:\033[1m\n\n";
        for (const auto& path : paths) {
            std::cout << "\033[1;94m" << path << "\033[0m" << std::endl;
        }
    } else if (rename_extensions) {
        std::cout << "\033[1mThe following path(s) file \033[1;95mextensions\033[0m \033[1mwill be recursively renamed to \033[1;92m" << case_input << "_case\033[0m:\033[1m\n\n";
        for (const auto& path : paths) {
            std::cout << "\033[1;94m" << path << "\033[0m" << std::endl;
        }
    } else {
        std::cout << "\033[1mThe following path(s) will be recursively renamed to \033[1;92m" << case_input << "_case\033[0m:\033[1m\n\n";
        for (const auto& path : paths) {
            std::cout << "\033[1;94m" << path << "\033[0m" << std::endl;
        }
    }
    std::cout << "\n\033[1mDo you want to proceed? (y/n): ";
    std::getline(std::cin, confirmation);
    std::cout << " " << std::endl;

    if (confirmation != "y") {
        std::cout << "\n\033[1;91mOperation aborted by user.\n\033[0m\n";
        std::cout << "\033[1mPress enter to exit...";
        std::cin.get();
        std::system("clear");
        return 0;
    }

    if (rename_parents) {
        rename_path(paths, case_input, true);
    } else if (rename_extensions) {
        rename_extension_path(paths, case_input, false);
    } else {
        rename_path(paths, case_input, false);
    }

    std::cout << "\n\033[1mPress enter to exit...\033[0m";
    std::cin.get();
    std::system("clear");
    return 0;
}
