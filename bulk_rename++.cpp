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
#include <cctype>
#include <queue>

namespace fs = std::filesystem;

std::mutex cout_mutex;
std::mutex dirs_count_mutex;
std::mutex files_count_mutex;

// General purpose stuff

std::string to_camel_case(const std::string& input) {
    std::string result;
    bool capitalizeNext = false;

    for (char c : input) {
        if (std::isalpha(c)) {
            result += capitalizeNext ? std::toupper(c) : std::tolower(c);
            capitalizeNext = false;
        } else if (std::isspace(c)) {
            capitalizeNext = true;
        } else if (c == '.') {
            // If the character is a period, add it to the result
            // as it's part of the file extension
            result += c;
            capitalizeNext = false; // Prevent the next character from being capitalized
        } else {
            // If the character is neither alphabetic nor a space,
            // add it to the result without any modification
            result += c;
            capitalizeNext = false;
        }
    }

    return result;
}


std::string from_camel_case(const std::string& input) {
    std::string result;
    
    for (char c : input) {
        if (std::isupper(c)) {
            result += ' ';
            result += std::tolower(c);
        } else {
            result += c;
        }
    }

    return result;
}



void print_message(const std::string& message) {
        std::lock_guard<std::mutex> lock(cout_mutex);
        std::cout << message << std::endl;
}


void print_error(const std::string& error) {
    std::lock_guard<std::mutex> lock(cout_mutex);
    std::cerr << error << std::endl;
}


void print_verbose_enabled(const std::string& message) {
    std::lock_guard<std::mutex> lock(cout_mutex);
    std::cout << message << std::endl;
}


void print_help() {

std::cout << "Usage: bulk_rename++ [OPTIONS] [PATHS]\n"
          << "Renames all files and folders under the specified path(s).\n"
          << "\n"
          << "Options:\n"
          << "  -h, --help               Print this message and exit\n"
          << "  -d [DEPTH]               Set traverse depth level otherwise maximum depth is used. -cp and -d 0 together rename only parent dir(s).\n"
          << "  -c [MODE]                Set the case conversion mode for file and dir name(s).\n"
          << "  -cp [MODE]               Set the case conversion mode for file and dir name(s), including parent dir(s).\n"
          << "  -ce [MODE]               Set the case conversion mode for file extension(s).\n"
          << "  -v, --verbose_enabled   Enable verbose mode.\n"
          << "\n"
          << "Modes for file and directory names:\n"
          << "  lower      Convert names to lowercase (e.g., Test => test)\n"
          << "  upper      Convert names to UPPERCASE (e.g., Test => TEST)\n"
          << "  title      Convert names to Title Case (e.g., test => Test)\n"
          << "  reverse    Reverse current case in names (e.g., Test => tEST)\n"
          << "  camel      Convert names to camelCase (e.g., Te_st => TeSt)\n"
          << "  rcamel     Reverse camelCase in names (e.g., TeSt => Te_st)\n"
          << "  snake      Convert spaces to underscores in names (e.g., Te st => Te_st)\n"
          << "  rsnake     Convert underscores to spaces in names (e.g., Te_st => Te st)\n"
          << "  kebab      Convert spaces to hyphens in names (e.g., Te st => Te-st)\n"
          << "  rkebab     Convert hyphens to spaces in names (e.g., Te-st => Te st)\n"
          << "  rspecial   Remove special characters from names (e.g., '@T|e/s&t' => Test)\n"
          << "  rnumeric   Remove numeric characters from names (e.g., 1Te0st2 => Test)\n"
          << "  rbra       Remove [ ] { } ( ) from names (e.g., [{Test}] => Test)\n"
          << "  roperand   Remove - + > < = * from names (e.g., =T-e+s<t> => Test)\n"
          << "\n"
          << "Examples:\n"
          << "  bulk_rename++ [path1] [path2]... -c lower\n"
          << "  bulk_rename++ [path1] -cp upper\n"
          << "  bulk_rename++ [path1] -cp upper -d 0\n"
          << "  bulk_rename++ [path1] -ce upper -v\n"
          << "  bulk_rename++ [path1] -v -cp upper\n"
          << "  bulk_rename++ [path1] -c upper -v\n"
          << "  bulk_rename++ [path1] -d 2 -c upper -v\n"
          << "\n";
	  }

// Extension stuff

void rename_extension(const fs::path& item_path, const std::string& case_input, bool verbose_enabled, int& files_count) {
    static const std::regex lower_case("([a-zA-Z]+)");
    static const std::regex upper_case("([a-zA-Z]+)");
    static const std::regex reverse_case("([a-zA-Z])");
    static const std::regex title_case("([a-zA-Z])([a-zA-Z.]*)"); 

    if (!fs::is_regular_file(item_path)) {
        if (verbose_enabled) {
            std::cout << "\033[0m\033[93mSkipped\033[0m " << item_path << " (not a regular file)" << std::endl;
        }
        return;
    }
    
    std::string extension = item_path.extension().string();
    std::string new_extension = extension; 

    if (case_input == "lower") {
        std::transform(new_extension.begin(), new_extension.end(), new_extension.begin(), ::tolower);
    } else if (case_input == "upper") {
        std::transform(new_extension.begin(), new_extension.end(), new_extension.begin(), ::toupper);
    } else if (case_input == "reverse") {
        std::smatch match;
        if (std::regex_search(extension, match, reverse_case)) {
            std::ostringstream oss;
            for (auto& c : match[1].str()) {
                oss << (std::islower(c) ? std::toupper(c) : std::tolower(c));
            }
            new_extension = oss.str();
        }
} else if (case_input == "title") {
    std::smatch match;
    if (std::regex_search(extension, match, title_case)) {
        std::string rest_of_extension = match[2].str();
        std::transform(rest_of_extension.begin(), rest_of_extension.end(), rest_of_extension.begin(), ::tolower);
        char first_letter = std::toupper(match[1].str()[0]);
        new_extension = "." + std::string(1, first_letter) + rest_of_extension;
    }
}

    if (extension != new_extension) {
        fs::path new_path = item_path.parent_path() / (item_path.stem().string() + new_extension);
        try {
            fs::rename(item_path, new_path);
            if (verbose_enabled) {
                std::lock_guard<std::mutex> lock(files_count_mutex);
                ++files_count;
                std::cout << "\033[0m\033[92mRenamed\033[0m file " << item_path.string() << " to " << new_path.string() << std::endl;
            }
        } catch (const fs::filesystem_error& e) {
            std::cerr << "\033[1;91mError\033[0m: " << e.what() << "\n" << std::endl;
        }
    } else {
        if (verbose_enabled) {
            if (extension.empty()) {
                std::cout << "\033[0m\033[93mSkipped\033[0m file " << item_path.string() << " (no extension)" << std::endl;
            } else {
                std::cout << "\033[0m\033[93mSkipped\033[0m file " << item_path.string() << " (extension unchanged)" << std::endl;
            }
        }
    }
}


void rename_extension_path(const std::vector<std::string>& paths, const std::string& case_input, bool verbose_enabled, int depth, int& files_count) {
    //If depth is negative (default value), set it to a very large number to effectively disable the depth limit
    if (depth < 0) {
        depth = std::numeric_limits<int>::max();
    }
    
    bool depth_limit_reached_printed = false; // Flag to track if the depth limit reached message is printed

    auto start_time = std::chrono::steady_clock::now(); // Start time measurement

    // Get the maximum number of threads supported by the system
    unsigned int max_threads = std::thread::hardware_concurrency();
		if (max_threads == 0) {
			max_threads = 1; // If hardware concurrency is not available, default to 1 thread
		}

    std::vector<std::thread> threads; // Vector to store threads

    // Determine the number of threads to create (minimum of max_threads and paths.size())
    unsigned int num_threads = std::min(max_threads, static_cast<unsigned int>(paths.size()));

    for (unsigned int i = 0; i < num_threads; ++i) {
        threads.emplace_back([&paths, i, &case_input, verbose_enabled, depth, &files_count, &depth_limit_reached_printed]() {
            // Each thread handles a subset of paths based on its index i
            // Example: process paths[i], paths[i + num_threads], paths[i + 2*num_threads], ...
            std::queue<std::pair<std::string, int>> directories; // Queue to store directories and their depths
            directories.push({paths[i], 0}); // Push the initial path onto the queue with depth 0

            while (!directories.empty()) {
                auto [current_path, current_depth] = directories.front();
                directories.pop();

                if (current_depth >= depth && !depth_limit_reached_printed) {
                    std::cout << "\n\033[0m\e[1;38;5;214mDepth limit reached at the level of:\033[1;94m " << current_path << "\033[0m" << std::endl;
                    depth_limit_reached_printed = true; // Set the flag to true after printing the message
                    continue; // Skip processing this directory
                }

                fs::path current_fs_path(current_path);

                if (verbose_enabled && !fs::exists(current_fs_path)) {
                    print_error("\033[1;91mError: path does not exist - " + current_path + "\033[0m\n");
                    continue;
                }

                if (fs::is_directory(current_fs_path)) {
                    for (const auto& entry : fs::directory_iterator(current_fs_path)) {
                        if (fs::is_directory(entry)) {
                            directories.push({entry.path().string(), current_depth + 1}); // Push subdirectories onto the queue with incremented depth
                        } else if (fs::is_regular_file(entry)) {
                            rename_extension(entry.path(), case_input, verbose_enabled, files_count);
                        }
                    }
                } else if (fs::is_regular_file(current_fs_path)) {
                    rename_extension(current_fs_path, case_input, verbose_enabled, files_count);
                } else {
                    print_error("\033[1;91mError: specified path is neither a directory nor a regular file\033[0m\n");
                }
            }
        });
    }

    // Join all threads
    for (auto& thread : threads) {
        thread.join();
    }

    auto end_time = std::chrono::steady_clock::now(); // End time measurement

    std::chrono::duration<double> elapsed_seconds = end_time - start_time; // Calculate elapsed time

    std::cout << "\n\033[1mRenamed extensions to " << case_input << "_case: \033[1;92m" << files_count << " file(s) \033[0m\033[1mfrom \033[1;95m" << paths.size()
              << " input path(s) \033[0m\033[1min " << std::setprecision(1)
              << std::fixed << elapsed_seconds.count() << "\033[1m second(s)\n";
}

// Rename file&directory stuff

void rename_file(const fs::path& item_path, const std::string& case_input, bool is_directory, bool verbose_enabled, int& files_count, int& dirs_count) {
    std::string name = item_path.filename().string();
    std::string new_name = name; // Initialize with original name
    fs::path new_path; // Declare new_path here to make it accessible in both branches

    static const std::regex transformation_pattern("(lower|upper|reverse|title|snake|rsnake|rspecial|rnumeric|rbra|roperand|camel|rcamel|kebab|rkebab)");
    std::smatch match;

    if (fs::is_symlink(item_path)) {
        if (verbose_enabled) {
            print_verbose_enabled("\033[0m\033[93mSkipped\033[0m symlink " + item_path.string() + " (not supported)");
        }
        return;
    }

    // Apply case transformation using regex patterns
    if (std::regex_search(case_input, match, transformation_pattern)) {
        const std::string& transformation = match[1].str();
        if (transformation == "lower") {
            std::transform(new_name.begin(), new_name.end(), new_name.begin(), ::tolower);
        } else if (transformation == "upper") {
            std::transform(new_name.begin(), new_name.end(), new_name.begin(), ::toupper);
        } else if (transformation == "reverse") {
            std::transform(new_name.begin(), new_name.end(), new_name.begin(), [](unsigned char c) {
                return std::islower(c) ? std::toupper(c) : std::tolower(c);
            });
        } else if (transformation == "title") {
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
            
        } else if (transformation == "snake") {
            std::replace(new_name.begin(), new_name.end(), ' ', '_');
        } else if (transformation == "rsnake") {
            std::replace(new_name.begin(), new_name.end(), '_', ' ');
        } else if (transformation == "kebab") {
            std::replace(new_name.begin(), new_name.end(), ' ', '-');
        } else if (transformation == "rkebab") {
            std::replace(new_name.begin(), new_name.end(), ' ', '-');
		
        
        } else if (transformation == "rspecial") {
            // Remove special characters from the name
            new_name.erase(std::remove_if(new_name.begin(), new_name.end(), [](char c) {
                return !std::isalnum(c) && c != '.' && c != '_' && c != '-' && c != '(' && c != ')' && c != '[' && c != ']' && c != '{' && c != '}' && c != '+' && c != '*' && c != '<' && c != '>'; // Retain
            }), new_name.end());
        } else if (transformation == "rnumeric") {
            // Remove numeric characters from the name
            new_name.erase(std::remove_if(new_name.begin(), new_name.end(), [](char c) {
                return std::isdigit(c);
            }), new_name.end());
        } else if (transformation == "rbra") {
            // Remove [ ] { } from the name
            new_name.erase(std::remove_if(new_name.begin(), new_name.end(), [](char c) {
                return c == '[' || c == ']' || c == '{' || c == '}' || c == '(' || c == ')';
            }), new_name.end());
        } else if (transformation == "roperand") {
            // Remove - + > < = * from the name
            new_name.erase(std::remove_if(new_name.begin(), new_name.end(), [](char c) {
                return c == '-' || c == '+' || c == '>' || c == '<' || c == '=' || c == '*';
            }), new_name.end());
        } else if (transformation == "camel") {
            new_name = to_camel_case(new_name);
        
        } else if (transformation == "rcamel") {
            new_name = from_camel_case(new_name);
        }
    }

    // Skip renaming if the new name is the same as the old name
    if (name != new_name) {
        new_path = item_path.parent_path() / new_name; // Assign new_path here

        try {
            fs::rename(item_path, new_path);

            if (verbose_enabled) {
                print_verbose_enabled("\033[0m\033[92mRenamed\033[0m file " + item_path.string() + " to " + new_path.string());
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
            print_verbose_enabled("\033[0m\033[93mSkipped\033[0m file " + item_path.string() + " (name unchanged)");
        }
    }
}


void rename_directory(const fs::path& directory_path, const std::string& case_input, bool rename_immediate_parent, bool verbose_enabled, int& files_count, int& dirs_count, int depth = -1) {
    std::string dirname = directory_path.filename().string();
    std::string new_dirname; // Initialize with original name
    bool renaming_message_printed = false;

    // Static Regular expression patterns for transformations
    static const std::regex transformation_pattern("(lower|upper|reverse|title|snake|rsnake|rspecial|rnumeric|rbra|roperand|camel|rcamel|kebab|rkebab)");

    if (fs::is_symlink(directory_path)) {
        if (verbose_enabled) {
            print_verbose_enabled("\033[0m\033[93mSkipped\033[0m symlink " + directory_path.string() + " (not supported)");
        }
        return;
    }

    // Apply case transformation using regex patterns
    if (std::regex_match(case_input, transformation_pattern)) {
        const std::string& transformation = case_input;
        if (transformation == "lower") {
            new_dirname = dirname;
            std::transform(new_dirname.begin(), new_dirname.end(), new_dirname.begin(), ::tolower);
        } else if (transformation == "upper") {
            new_dirname = dirname;
            std::transform(new_dirname.begin(), new_dirname.end(), new_dirname.begin(), ::toupper);
        } else if (transformation == "reverse") {
            new_dirname = dirname;
            std::transform(new_dirname.begin(), new_dirname.end(), new_dirname.begin(), [](unsigned char c) {
                return std::islower(c) ? std::toupper(c) : std::tolower(c);
            });
        } else if (transformation == "title") {
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
        } else if (transformation == "snake") {
            std::replace(dirname.begin(), dirname.end(), ' ', '_');
            new_dirname = dirname;
        } else if (transformation == "rsnake") {
            std::replace(dirname.begin(), dirname.end(), '_', ' ');
            new_dirname = dirname;
        } else if (transformation == "kebab") {
            std::replace(dirname.begin(), dirname.end(), ' ', '-');
            new_dirname = dirname;
        } else if (transformation == "rkebab") {
            std::replace(dirname.begin(), dirname.end(), '-', ' ');
            new_dirname = dirname;
        } else if (transformation == "rspecial") {
            // Remove special characters from the directory name
            new_dirname = dirname;
            new_dirname.erase(std::remove_if(new_dirname.begin(), new_dirname.end(), [](char c) {
                return !std::isalnum(c) && c != '.' && c != '_' && c != '-' && c != '(' && c != ')' && c != '[' && c != ']' && c != '{' && c != '}' && c != '+' && c != '*' && c != '<' && c != '>'; // Retain
            }), new_dirname.end());
        } else if (transformation == "rnumeric") {
            // Remove numeric characters from the directory name
            new_dirname = dirname;
            new_dirname.erase(std::remove_if(new_dirname.begin(), new_dirname.end(), [](char c) {
                return std::isdigit(c);
            }), new_dirname.end());
        } else if (transformation == "rbra") {
            // Remove [ ] { } from the name
            new_dirname = dirname;
            new_dirname.erase(std::remove_if(new_dirname.begin(), new_dirname.end(), [](char c) {
                return c == '[' || c == ']' || c == '{' || c == '}' || c == '(' || c == ')';
            }), new_dirname.end());
        } else if (transformation == "roperand") {
            // Remove - + > < = * from the name
            new_dirname = dirname;
            new_dirname.erase(std::remove_if(new_dirname.begin(), new_dirname.end(), [](char c) {
                return c == '-' || c == '+' || c == '>' || c == '<' || c == '=' || c == '*';
            }), new_dirname.end());
        } else if (transformation == "camel") {
            new_dirname = dirname;
            new_dirname = to_camel_case(new_dirname);
        } else if (transformation == "rcamel") {
            new_dirname = dirname;
            new_dirname = from_camel_case(new_dirname);
        }
    }

    fs::path new_path = directory_path.parent_path() / std::move(new_dirname); // Move new_dirname instead of copying

    // Check if renaming is necessary
    if (directory_path != new_path) {
        try {
            fs::rename(directory_path, new_path);

            if (verbose_enabled && !renaming_message_printed) {
                print_verbose_enabled("\033[0m\033[94mRenamed\033[0m directory " + directory_path.string() + " to " + new_path.string());
                renaming_message_printed = true; // Set the flag to true after printing the message
            }
            std::lock_guard<std::mutex> lock(dirs_count_mutex);
            ++dirs_count;
        } catch (const fs::filesystem_error& e) {
            std::cerr << "\033[1;91mError\033[0m: " << e.what() << "\n" << std::endl;
            return; // Stop processing if renaming failed
        }

    } else {
        if (verbose_enabled) {
            print_verbose_enabled("\033[0m\033[93mSkipped\033[0m directory " + directory_path.string() + " (name unchanged)");
        }
    }

    // Continue recursion if depth limit not reached
    if (depth != 0) {
        // Decrement depth only if depth limit is positive
        if (depth > 0)
            --depth;

        // Process items within the directory
        unsigned int max_threads = std::thread::hardware_concurrency();
        if (max_threads == 0) {
            max_threads = 1; // If hardware concurrency is not available, default to 1 thread
        }

        std::vector<std::thread> threads;
        for (const auto& entry : fs::directory_iterator(new_path)) {
            if (entry.is_directory()) {
                if (threads.size() < max_threads) {
                    // Start a new thread for each subdirectory
                    threads.emplace_back(rename_directory, entry.path(), case_input, false, verbose_enabled, std::ref(files_count), std::ref(dirs_count), depth);
                } else {
                    // Process directories in the main thread if max_threads is reached
                    rename_directory(entry.path(), case_input, false, verbose_enabled, files_count, dirs_count, depth);
                }
            } else {
                // Process files in the main thread
                rename_file(entry.path(), case_input, false, verbose_enabled, files_count, dirs_count);
            }
        }

        // Join all threads
        for (auto& thread : threads) {
            thread.join();
        }
        
    }

    if (verbose_enabled && depth > 0) {
        print_verbose_enabled("\n\033[0m\e[1;38;5;214mDepth limit reached at the level of:\033[1;94m " + directory_path.string());
    }
}


void rename_path(const std::vector<std::string>& paths, const std::string& case_input, bool rename_immediate_parent, bool verbose_enabled = false, int depth = -1) {
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

    // Calculate num_threads as the minimum of max_threads and the number of paths
    unsigned int num_threads = std::min(max_threads, static_cast<unsigned int>(paths.size()));

    // Process paths in separate threads up to num_threads
    for (unsigned int i = 0; i < num_threads; ++i) {
        fs::path current_path(paths[i]);

        if (fs::exists(current_path)) {
            if (fs::is_directory(current_path)) {
                if (rename_immediate_parent) {
                    // If -p option is used, only rename the immediate parent
                    fs::path immediate_parent_path = current_path.parent_path();
                    rename_directory(immediate_parent_path, case_input, rename_immediate_parent, verbose_enabled, files_count, dirs_count, depth);
                } else {
                    // Otherwise, rename the entire path
                    threads.emplace_back(rename_directory, current_path, case_input, rename_immediate_parent, verbose_enabled, std::ref(files_count), std::ref(dirs_count), std::ref(depth));
                }
            } else if (fs::is_regular_file(current_path)) {
                // For files, directly rename the item without considering the parent directory
                rename_file(current_path, case_input, false, verbose_enabled, files_count, dirs_count);
            } else {
                print_error("\033[1;91mError: specified path is neither a directory nor a regular file\033[0m\n");
            }
        } else {
            print_error("\033[1;91mError: path does not exist - " + paths[i] + "\033[0m\n");
        }
    }

    // Process remaining paths in the main thread
    for (unsigned int i = num_threads; i < paths.size(); ++i) {
        fs::path current_path(paths[i]);

        if (fs::exists(current_path)) {
            if (fs::is_directory(current_path)) {
                if (rename_immediate_parent) {
                    // If -p option is used, only rename the immediate parent
                    fs::path immediate_parent_path = current_path.parent_path();
                    rename_directory(immediate_parent_path, case_input, rename_immediate_parent, verbose_enabled, files_count, dirs_count, depth);
                } else {
                    // Otherwise, rename the entire path in the main thread
                    rename_directory(current_path, case_input, rename_immediate_parent, verbose_enabled, files_count, dirs_count, depth);
                }
            } else if (fs::is_regular_file(current_path)) {
                // For files, directly rename the item without considering the parent directory
                rename_file(current_path, case_input, false, verbose_enabled, files_count, dirs_count);
            } else {
                print_error("\033[1;91mError: specified path is neither a directory nor a regular file\033[0m\n");
            }
        } else {
            print_error("\033[1;91mError: path does not exist - " + paths[i] + "\033[0m\n");
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
    bool verbose_enabled = false;
    int depth = -1;
    bool case_specified = false;

    if (argc == 1) {
        print_help();
        return 0;
    }

    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        if (arg == "-d" && i + 1 < argc) {
            depth = std::atoi(argv[++i]);
        } else if (arg == "-v" || arg == "--verbose_enabled") {
            verbose_enabled = true;
        } else if (arg == "-h" || arg == "--help") {
            print_help();
            return 0;
        } else if (arg == "-cp" || arg == "-c" || arg == "-ce") {
            if (i + 1 < argc) {
                case_input = argv[++i];
                case_specified = true;
                // Check if the case modes are valid
                std::vector<std::string> valid_modes = {"lower", "upper", "reverse", "title", "camel", "rcamel", "kebab", "rkebab", "rsnake", "snake", "rnumeric", "rspecial", "rbra", "roperand"};
                std::vector<std::string> modes;
                std::string mode;
                std::size_t pos = 0;
                while ((pos = case_input.find(';')) != std::string::npos) {
                    mode = case_input.substr(0, pos);
                    if (std::find(valid_modes.begin(), valid_modes.end(), mode) != valid_modes.end()) {
                        modes.push_back(mode);
                    } else {
                        print_error("\033[1;91mError: Unspecified or invalid case mode - " + mode + ". Run 'bulk_rename++ --help'.\n");
                        return 1;
                    }
                    case_input.erase(0, pos + 1);
                }
                if (std::find(valid_modes.begin(), valid_modes.end(), case_input) != valid_modes.end()) {
                    modes.push_back(case_input);
                } else {
                    print_error("\033[1;91mError: Unspecified or invalid case mode - " + case_input + ". Run 'bulk_rename++ --help'.\n");
                    return 1;
                }
                case_input = modes[0];
                for (std::size_t j = 1; j < modes.size(); ++j) {
                    case_input += ";" + modes[j];
                }
            } else {
                print_error("\033[1;91mError: Missing argument for option " + arg + "\n");
                return 1;
            }
            if (arg == "-cp") {
                rename_parents = true;
            } else if (arg == "-ce") {
                rename_extensions = true;
            }
        } else {
            // Check for duplicate paths
            if (std::find(paths.begin(), paths.end(), arg) != paths.end()) {
                print_error("\033[1;91mError: Duplicate path detected - " + arg + "\033[0m\n");
                return 1;
            }
            paths.emplace_back(arg);
        }
    }

    if (!case_specified) {
        print_error("\033[1;91mError: Case conversion mode not specified (-c, -cp, or -ce option is required)\033[0m\n");
        return 1;
    }

    // Check if paths exist
    for (const auto& path : paths) {
        if (!fs::exists(path)) {
            print_error("\033[1;91mError: Path does not exist - " + path + "\033[0m\n");
            return 1;
        }
    }

    std::system("clear");
    if (case_input != "snake" && case_input != "rsnake") {
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
        rename_path(paths, case_input, true, verbose_enabled, depth); // Pass true for rename_immediate_parent
    } else if (rename_extensions) {
        int files_count = 0; // Declare files_count here
        rename_extension_path(paths, case_input, verbose_enabled, depth, files_count);
    } else {
        rename_path(paths, case_input, false, verbose_enabled, depth); // Pass false for rename_immediate_parent
    }

    std::cout << "\n\033[1mPress enter to exit...\033[0m";
    std::cin.get();
    std::system("clear");
    return 0;
}
