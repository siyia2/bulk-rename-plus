#include <iostream>
#include <string>
#include <filesystem>
#include <algorithm>
#include <vector>
#include <thread>
#include <mutex>
#include <unistd.h>
#include <chrono>

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
              << "  -ce [MODE]           Set the case conversion mode (lower/upper/fupper/reverse) w/o file extension(s)\n"
              << "  -v, --verbose        Enable verbose mode\n"
              << "\n"
              << "Examples:\n"
              << "  bulk_rename++ /path/to/folder1 /path/to/folder2 -c lower\n"
              << "  bulk_rename++ /path/to/folder -cp upper\n"
              << "  bulk_rename++ /path/to/folder -ce upper\n"
              << "  bulk_rename++ /path/to/folder -v -cp upper\n"
              << "  bulk_rename++ /path/to/folder -c upper -v\n"
              << "\n";
}


std::string fupper_ext(const std::string& extension) {
    std::string result = extension;
    std::transform(result.begin(), result.end(), result.begin(), ::toupper);
    return result;
}


void rename_file_extension(const fs::path& file_path, const std::string& case_input, bool verbose, int& files_count) {
    std::string extension = file_path.extension().string();
    std::string new_extension = extension; // Initialize with original extension

    if (case_input == "lower") {
        std::transform(new_extension.begin(), new_extension.end(), new_extension.begin(), ::tolower);
    } else if (case_input == "upper") {
        std::transform(new_extension.begin(), new_extension.end(), new_extension.begin(), ::toupper);
    } else if (case_input == "reverse") {
        std::transform(new_extension.begin(), new_extension.end(), new_extension.begin(), [](unsigned char c) {
            return std::islower(c) ? std::toupper(c) : std::tolower(c);
        });
    } else if (case_input == "fupper") {
        new_extension = fupper_ext(extension);
    }

    // Skip renaming if the new extension is the same as the old one
    if (extension != new_extension) {
        fs::path new_path = file_path;
        new_path.replace_extension(new_extension);

        try {
            fs::rename(file_path, new_path);
            if (verbose) {
                std::cout << "\033[0m\033[92mRenamed \033[0mfile " << file_path.string() << " to " << new_path.string() << std::endl;
            }
            ++files_count;
        } catch (const fs::filesystem_error& e) {
            std::cerr << "\033[1;91mError\033[0m: " << e.what() << "\n" << std::endl;
        }
    } else {
        if (verbose) {
            std::cout << "\033[0m\033[93mSkipped file\033[0m " << file_path.string() << " (extension unchanged)" << std::endl;
        }
    }
}


std::string fupper(const std::string& word) {
    std::string result = word;
    if (!result.empty()) {
        result[0] = std::toupper(result[0]);
        std::transform(result.begin() + 1, result.end(), result.begin() + 1, ::tolower);
    }
    return result;
}


void rename_item(const fs::path& item_path, const std::string& case_input, bool is_directory, bool verbose, int& files_count, int& dirs_count, bool rename_only_extension = false) {
    if (rename_only_extension && !is_directory) {
        rename_file_extension(item_path, case_input, verbose, files_count);
        return;
    }

    std::string name = item_path.filename().string();
    std::string new_name = name; // Initialize with original name

    // Apply case transformation only to the filename (not the extension) if needed
    std::string filename = item_path.stem().string();
    std::string extension = item_path.extension().string();

    if (case_input == "lower") {
        std::transform(filename.begin(), filename.end(), filename.begin(), ::tolower);
    } else if (case_input == "upper") {
        std::transform(filename.begin(), filename.end(), filename.begin(), ::toupper);
    } else if (case_input == "reverse") {
        std::transform(filename.begin(), filename.end(), filename.begin(), [](unsigned char c) {
            return std::islower(c) ? std::toupper(c) : std::tolower(c);
        });
    } else if (case_input == "fupper") {
        bool first_letter_encountered = true;
        for (char& c : filename) {
            if (std::isalpha(c)) {
                if (first_letter_encountered) {
                    c = std::toupper(c);
                    first_letter_encountered = false;
                } else {
                    c = std::tolower(c); // Convert subsequent letters to lowercase
                }
            }
        }
    }

    new_name = filename + extension; // Combine the modified filename with the original extension

    // Skip renaming if the new name is the same as the old name
    if (name != new_name) {
        fs::path new_path = item_path.parent_path() / new_name;

        try {
            fs::rename(item_path, new_path);
            if (verbose) {
                std::string item_type = is_directory ? "directory" : "file";
                std::cout << "\033[0m\033[92mRenamed\033[0m " << item_type << "\033[0m " << item_path.string() << " to " << new_path.string() << std::endl;
            }
            if (!is_directory) {
                ++files_count;
            } else {
                ++dirs_count;
            }
        } catch (const fs::filesystem_error& e) {
            std::cerr << "\033[1;91mError\033[0m: " << e.what() << "\n" << std::endl;
        }
    } else {
        if (verbose) {
            std::string item_type = is_directory ? "directory" : "file";
            std::cout << "\033[0m\033[93mSkipped\033[0m " << item_type << "\033[0m " << item_path.string() << " (name unchanged)" << std::endl;
        }
    }
}


void rename_directory(const fs::path& directory_path, const std::string& case_input, bool rename_immediate_parent, bool verbose, int& files_count, int& dirs_count) {
    std::string dirname = directory_path.filename().string();
    std::string new_dirname; // Initialize with original name

    // Apply case transformation if needed
    if (case_input == "lower") {
        new_dirname = dirname;
        std::transform(new_dirname.begin(), new_dirname.end(), new_dirname.begin(), ::tolower);
    } else if (case_input == "upper") {
        new_dirname = dirname;
        std::transform(new_dirname.begin(), new_dirname.end(), new_dirname.begin(), ::toupper);
    } else if (case_input == "reverse") {
        new_dirname = dirname;
        std::transform(new_dirname.begin(), new_dirname.end(), new_dirname.begin(), [](unsigned char c) {
            return std::islower(c) ? std::toupper(c) : std::tolower(c);
        });
    } else if (case_input == "fupper") {
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
    }

    fs::path new_path = directory_path.parent_path() / new_dirname;

    // Check if renaming is necessary
    if (directory_path != new_path) {
        try {
            fs::rename(directory_path, new_path);
            if (verbose) {
                std::cout << "\033[0m\033[94mRenamed \033[0mdirectory " << directory_path.string() << " to " << new_path.string() << std::endl;
            }
            ++dirs_count;
        } catch (const fs::filesystem_error& e) {
            std::cerr << "\033[1;91mError\033[0m: " << e.what() << "\n" << std::endl;
            return; // Stop processing if renaming failed
        }
    } else {
        if (verbose && !rename_immediate_parent) { // Only print skipped message if not renaming the immediate parent
            std::cout << "\033[0m\033[93mSkipped \033[0mdirectory " << directory_path.string() << " (name unchanged)" << std::endl;
        }
    }

    // If rename_immediate_parent is true, rename the immediate parent directory
    if (rename_immediate_parent) {
        rename_directory(new_path, case_input, false, verbose, files_count, dirs_count);
    } else {
        // Otherwise, recursively rename all contents within the directory
        for (const auto& entry : fs::directory_iterator(new_path)) {
            if (entry.is_directory()) {
                rename_directory(entry.path(), case_input, false, verbose, files_count, dirs_count);
            } else {
                rename_item(entry.path(), case_input, false, verbose, files_count, dirs_count);
            }
        }
    }
}


void rename_path(const std::vector<std::string>& paths, const std::string& case_input, bool rename_immediate_parent, bool verbose = true, bool rename_only_extension = false) {
    // Check if case_input is empty
    if (case_input.empty()) {
        print_error("\033[1;91mError: Case conversion mode not specified (-c option is required)\n\033[0m");
        return;
    }

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
                    rename_directory(current_path, case_input, rename_immediate_parent, verbose, files_count, dirs_count);
                }
            } else if (fs::is_regular_file(current_path)) {
                // For files, directly rename the item without considering the parent directory
                if (rename_only_extension) {
                    rename_file_extension(current_path, case_input, verbose, files_count);
                } else {
                    rename_item(current_path, case_input, false, verbose, files_count, dirs_count);
                }
            } else {
                print_error("\033[1;91mError: specified path is neither a directory nor a regular file\033[0m\n");
            }
        } else {
            print_error("\033[1;91mError: path does not exist - " + path + "\033[0m\n");
        }
    }

    std::cout << "\n\033[1mRenamed to " << case_input << "_case: \033[1;92m" << files_count << " file(s) \033[0m\033[1mand \033[1;94m"
              << dirs_count << " dir(s) \033[0m\033[1mfrom \033[1;95m" << paths.size()
              << " input path(s) \033[0m\n";
}


int main(int argc, char *argv[]) {
    std::vector<std::string> paths;
    std::string case_input;
    bool rename_parents = false;
    bool rename_only_extension = false; // Flag for -ce option

    bool case_specified = false;

    bool c_option_provided = false;
    bool cp_option_provided = false;
    bool ce_option_provided = false;

    // Parse command line arguments
    if (argc >= 2) {
        for (int i = 1; i < argc; ++i) {
            std::string arg(argv[i]);
            if (arg == "-v" || arg == "--verbose") {
                verbose_enabled = true;
            } else if (arg == "-h" || arg == "--help") {
                print_help();
                return 0;
            } else if (arg == "-c") {
                if (c_option_provided || cp_option_provided || ce_option_provided) {
                    print_error("\033[1;91mError: Multiple case conversion options specified. Please provide only one of -c, -cp, or -ce.\n");
                    return 1;
                }
                c_option_provided = true;
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
            } else if (arg == "-cp") {
                if (c_option_provided || cp_option_provided || ce_option_provided) {
                    print_error("\033[1;91mError: Multiple case conversion options specified. Please provide only one of -c, -cp, or -ce.\n");
                    return 1;
                }
                cp_option_provided = true;
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
            } else if (arg == "-ce") {
                if (c_option_provided || cp_option_provided || ce_option_provided) {
                    print_error("\033[1;91mError: Multiple case conversion options specified. Please provide only one of -c, -cp, or -ce.\n");
                    return 1;
                }
                ce_option_provided = true;
                rename_only_extension = true;
                if (i + 1 < argc) {
                    case_input = argv[++i];
                    case_specified = true;
                    // Check if the case mode is valid
                    if (case_input != "lower" && case_input != "upper" && case_input != "reverse" && case_input != "fupper") {
                        print_error("\033[1;91mError: Unspecified case mode. Please specify 'lower', 'upper', 'reverse', or 'fupper'.\n");
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

    // Check if exactly one case conversion option is provided
    if (!c_option_provided && !cp_option_provided && !ce_option_provided) {
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

    // Determine whether to call rename_path or rename_only_extension based on the option specified
    if (rename_only_extension) {
        rename_path(paths, case_input, false, verbose_enabled, true);
    } else {
        rename_path(paths, case_input, rename_parents, verbose_enabled);
    }

    std::cout << "\n";

    std::cout << "\033[1mPress enter to exit...";
    std::cin.get();
    std::system("clear");

    return 0;
}

