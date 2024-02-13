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

std::cout << "\x1B[32mUsage: bulk_rename++ [OPTIONS] [PATHS]\n"
          << "Renames all files and folders under the specified path(s).\n"
          << "\n"
          << "Options:\n"
          << "  -h, --help               Print this message and exit\n"
          << "  -v, --verbose            Enable verbose mode.\n"
          << "  -fi                      Only files will be renamed, by default everything is renamed.\n"
          << "  -fo                      Only folders will be renamed, by default everything is renamed.\n"
          << "  -d  [DEPTH]              Set traverse depth level otherwise maximum (-1) is used. -cp and -d 0 together rename only parent dir(s).\n"
          << "  -c  [MODE]               Set the case conversion mode for file and dir name(s).\n"
          << "  -cp [MODE]               Set the case conversion mode for file and dir name(s), including parent dir(s).\n"
          << "  -ce [MODE]               Set the case conversion mode for file extension(s).\n"
          << "\n"
          << "Modes for file, directory and extension renaming:\n"
          << "Common Regular:\n"
          << "  title      Convert names to Title Case (e.g., test => Test)\n"
          << "  upper      Convert names to UPPERCASE (e.g., Test => TEST)\n"
          << "  lower      Convert names to lowercase (e.g., Test => test)\n"
          << "  reverse    Reverse current case in names (e.g., Test => tEST)\n"
          << "Common Special:\n"
          << "  snake      Convert spaces to underscores in names (e.g., Te st => Te_st)\n"
          << "  rsnake     Convert underscores to spaces in names (e.g., Te_st => Te st)\n"
          << "  kebab      Convert spaces to hyphens in names (e.g., Te st => Te-st)\n"
          << "  rkebab     Convert hyphens to spaces in names (e.g., Te-st => Te st)\n"
          << "  camel      Convert names to camelCase (e.g., Te_st => TeSt)\n"
          << "  rcamel     Reverse camelCase in names (e.g., TeSt => Te_st)\n"
          << "Extension exclusive:\n"
          << "  bak        Add .bak at file extension names (e.g., Test.txt => Test.txt.bak)\n"
          << "  rbak       Remove .bak from file extension names (e.g., Test.txt.bak => Test.txt)\n"
          << "  noext      Remove extensions (e.g., Test.txt => Test)\n"
		  << "Numbering:\n"
		  << "  sequence   Apply sequential numbering to files only (e.g. Test => 001_Test)\n"
          << "  rsequence  Remove sequential numbering of files (e.g. 001_Test => Test)\n"
		  << "  rnumeric   Remove numeric characters from names (e.g., 1Te0st2 => Test)\n"
          << "Custom:\n"
          << "  rbra       Remove [ ] { } ( ) from names (e.g., [{Test}] => Test)\n"
          << "  roperand   Remove - + > < = * from names (e.g., =T-e+s<t> => Test)\n"
          << "  rspecial   Remove special characters from names (e.g., Tes\t!@#$%^|&~`'\"\";? => Test)\n"
          << "\n"
          << "Examples:\n"
          << "  bulk_rename++ [path1] [path2]... -c lower\n"
          << "  bulk_rename++ [path1] -cp upper\n"
          << "  bulk_rename++ [path1] -cp upper -d 0\n"
          << "  bulk_rename++ [path1] -v -cp upper\n"
          << "  bulk_rename++ [path1] -c upper -v\n"
          << "  bulk_rename++ [path1] -d 2 -c upper -v\n"
          << "  bulk_rename++ [path1] -fi -c lower -v\n"
          << "  bulk_rename++ [path1] [path2] -fo upper\n"
          << "  bulk_rename++ [path1] -ce noext -v\n"
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
    std::string new_extension; 

    if (case_input == "lower") {
        std::transform(extension.begin(), extension.end(), std::back_inserter(new_extension), ::tolower);
    } else if (case_input == "upper") {
        std::transform(extension.begin(), extension.end(), std::back_inserter(new_extension), ::toupper);
    } else if (case_input == "reverse") {
        std::transform(extension.begin(), extension.end(), std::back_inserter(new_extension), [](char c) {
            return std::islower(c) ? std::toupper(c) : std::tolower(c);
        });
    } else if (case_input == "title") {
        std::smatch match;
        if (std::regex_search(extension, match, title_case)) {
            std::string rest_of_extension = match[2].str();
            std::transform(rest_of_extension.begin(), rest_of_extension.end(), rest_of_extension.begin(), ::tolower);
            char first_letter = std::toupper(match[1].str()[0]);
            new_extension = "." + std::string(1, first_letter) + rest_of_extension;
        }
    } else if (case_input == "bak") {
        new_extension = ".bak";
    } else if (case_input == "rbak") {
        if (extension.length() >= 4 && extension.substr(extension.length() - 4) == ".bak") {
            new_extension = extension.substr(0, extension.length() - 4);
        }
    } else if (case_input == "noext") {
        new_extension = ""; // Setting new_extension to empty string effectively removes the extension
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

void rename_file(const fs::path& item_path, const std::string& case_input, bool is_directory, bool verbose_enabled, bool transform_dirs, bool transform_files, int& files_count, int& dirs_count) {
    std::string name = item_path.filename().string();
    std::string new_name = name; // Initialize with original name
    fs::path new_path; // Declare new_path here to make it accessible in both branches

    static const std::regex transformation_pattern("(lower|upper|reverse|title|snake|rsnake|rspecial|rnumeric|rbra|roperand|camel|rcamel|kebab|rkebab|sequence|rsequence)");
    std::smatch match;

    if (fs::is_symlink(item_path)) {
        if (verbose_enabled) {
            print_verbose_enabled("\033[0m\033[93mSkipped\033[0m symlink " + item_path.string() + " (not supported)");
        }
        return;
    }

    std::vector<std::pair<std::string, size_t>> transformations;
    std::sregex_iterator iter(case_input.begin(), case_input.end(), transformation_pattern);
    std::sregex_iterator end;
    for (; iter != end; ++iter) {
        transformations.emplace_back((*iter)[1].str(), (*iter).position());
    }


    if (transform_files) {
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
                std::replace(new_name.begin(), new_name.end(), '-', ' ');
            } else if (transformation == "rspecial") {
                // Remove special characters from the name
                new_name.erase(std::remove_if(new_name.begin(), new_name.end(), [](char c) {
                    return !std::isalnum(c) && c != '.' && c != '_' && c != '-' && c != '(' && c != ')' && c != '[' && c != ']' && c != '{' && c != '}' && c != '+' && c != '*' && c != '<' && c != '>' && c != ' '; // Retain
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
            } else if (transformation == "sequence") {
                // Check if the filename is already numbered
                if (!std::isdigit(new_name.front())) {
                    static int counter = 1;
                    std::ostringstream oss;
                    oss << std::setw(3) << std::setfill('0') << counter++;
                    new_name = oss.str() + "_" + new_name;
                }
            } else if (transformation == "rsequence") {
                // Find the position of the first non-digit character
                size_t pos = new_name.find_first_not_of("0123456789");
                
                // Check if the filename is already numbered and contains an underscore after numbering
                if (pos != std::string::npos && pos > 0 && new_name[pos] == '_') {
                    // Remove the number and the first underscore
                    new_name.erase(0, pos + 1);
                }
                else if (pos != std::string::npos && pos > 0) {
                    // Remove only the number
                    new_name.erase(0, pos);
                }
            }
        }
    }

    // Skip renaming if the new name is the same as the old name
    if (name != new_name) {
        fs::path new_path = item_path.parent_path() / std::move(new_name);

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
        if (verbose_enabled && transform_dirs && !transform_files) {
            print_verbose_enabled("\033[0m\033[93mSkipped\033[0m file " + item_path.string() + " (skipped by -fo)");
        } else { 
			print_verbose_enabled("\033[0m\033[93mSkipped\033[0m file " + item_path.string() + " (name unchanged)");
		}
    }
}

void rename_directory(const fs::path& directory_path, const std::string& case_input, bool rename_immediate_parent, bool verbose_enabled, bool transform_dirs, bool transform_files, int& files_count, int& dirs_count, int depth) {
    std::string dirname = directory_path.filename().string();
    std::string new_dirname; // Initialize with original name
    bool renaming_message_printed = false;

    // Static Regular expression patterns for transformations
    static const std::regex transformation_pattern("(lower|upper|reverse|title|snake|rsnake|rspecial|rnumeric|rbra|roperand|camel|rcamel|kebab|rkebab|sequence|rsequence)");

    if (fs::is_symlink(directory_path)) {
        if (verbose_enabled) {
            print_verbose_enabled("\033[0m\033[93mSkipped\033[0m symlink " + directory_path.string() + " (not supported)");
        }
        return;
    }
	
    if (transform_dirs) {
        // Apply case transformation using regex patterns
            std::smatch match;
        if (std::regex_match(case_input, match, transformation_pattern)) {
        const std::string& transformation = match[1].str();
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
                    return !std::isalnum(c) && c != '.' && c != '_' && c != '-' && c != '(' && c != ')' && c != '[' && c != ']' && c != '{' && c != '}' && c != '+' && c != '*' && c != '<' && c != '>' && c != ' '; // Retain
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
            } else if (transformation == "sequence") {
                // Do nothing for directories
                new_dirname = dirname;
            } else if (transformation == "rsequence") {
                // Do nothing for directories
                new_dirname = dirname;
            }
        }
    } else {
        // If transform_dirs is false, keep the original directory name
        new_dirname = dirname;
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
        if (verbose_enabled && !transform_dirs && transform_files) {
            print_verbose_enabled("\033[0m\033[93mSkipped\033[0m directory " + directory_path.string() + " (skipped by -fi)");
            }
         else {
			print_verbose_enabled("\033[0m\033[93mSkipped\033[0m directory " + directory_path.string() + " (name unchanged)");
		}
			
    }

    // Continue recursion if depth limit not reached
    if (depth != 0) {
        // Decrement depth only if depth limit is positive
        if (depth > 0)
            --depth;
        
        unsigned int max_threads = std::thread::hardware_concurrency();
        if (max_threads == 0) {
            max_threads = 1; // If hardware concurrency is not available, default to 1 thread
        }

        if (rename_immediate_parent) {
            // Process subdirectories without spawning threads
            for (const auto& entry : fs::directory_iterator(new_path)) {
                if (entry.is_directory()) {
                    rename_directory(entry.path(), case_input, false, verbose_enabled, transform_dirs, transform_files, files_count, dirs_count, depth);
                } else {
                    rename_file(entry.path(), case_input, false, verbose_enabled, transform_dirs, transform_files, files_count, dirs_count);
                }
            }
        } else {
            std::vector<std::thread> threads;
            for (const auto& entry : fs::directory_iterator(new_path)) {
                if (entry.is_directory()) {
                    if (threads.size() < max_threads) {
                        // Start a new thread for each subdirectory
                        threads.emplace_back(rename_directory, entry.path(), case_input, false, verbose_enabled, transform_dirs, transform_files, std::ref(files_count), std::ref(dirs_count), depth);
                    } else {
                        // Process directories in the main thread if max_threads is reached
                        rename_directory(entry.path(), case_input, false, verbose_enabled, transform_dirs, transform_files, files_count, dirs_count, depth);
                    }
                } else {
                    // Process files in the main thread
                    rename_file(entry.path(), case_input, false, verbose_enabled, transform_dirs, transform_files, files_count, dirs_count);
                }
            }

            // Join all threads
            for (auto& thread : threads) {
                thread.join();
            }
        }

        static bool depth_limit_reached_printed = false; // Declare a static boolean flag

        if (verbose_enabled && depth == 0 && !depth_limit_reached_printed) {
            depth_limit_reached_printed = true;
            usleep(500000);
            print_verbose_enabled("\n\033[0m\e[1;38;5;214mDepth limit reached at the level of:\033[1;94m " + directory_path.string());
        }
    }
}


void rename_path(const std::vector<std::string>& paths, const std::string& case_input, bool rename_immediate_parent, bool verbose_enabled = false,bool transform_dirs = true, bool transform_files = true, int depth = -1)  {
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
                    rename_directory(immediate_parent_path, case_input, rename_immediate_parent, verbose_enabled,transform_dirs,transform_files, files_count, dirs_count, depth);
                } else {
                    // Otherwise, rename the entire path
                    threads.emplace_back(rename_directory, current_path, case_input, rename_immediate_parent, verbose_enabled, transform_dirs, transform_files, std::ref(files_count), std::ref(dirs_count), depth);
                }
            } else if (fs::is_regular_file(current_path)) {
                // For files, directly rename the item without considering the parent directory
                rename_file(current_path, case_input, false, verbose_enabled,transform_dirs,transform_files, files_count, dirs_count);
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
                    rename_directory(immediate_parent_path, case_input, rename_immediate_parent, verbose_enabled,transform_dirs,transform_files, files_count, dirs_count, depth);
                } else {
                    // Otherwise, rename the entire path in the main thread
                    rename_directory(current_path, case_input, rename_immediate_parent, verbose_enabled,transform_dirs,transform_files, files_count, dirs_count, depth);
                }
            } else if (fs::is_regular_file(current_path)) {
                // For files, directly rename the item without considering the parent directory
                rename_file(current_path, case_input, false, verbose_enabled, true, true, files_count, dirs_count);
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
    bool transform_dirs = true;
    bool transform_files = true;

    if (argc == 1) {
        print_help();
        return 0;
    }

    bool fi_flag = false;
    bool fo_flag = false;
    bool c_flag = false;
    bool cp_flag = false;
    bool ce_flag = false;

    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        if (arg == "-fi") {
            transform_dirs = false;
            fi_flag = true;
        } else if (arg == "-fo") {
            transform_files = false;
            fo_flag = true;
        } else if (arg == "-d" && i + 1 < argc) {
            // Check if the depth value is empty or not a number
            if (argv[i + 1] == nullptr || std::string(argv[i + 1]).empty() || !isdigit(argv[i + 1][0])) {
                print_error("\033[1;91mError: Depth value if set must be a non-negative integer.\033[0m\n");
                return 1;
            }
            depth = std::atoi(argv[++i]);
            if (depth < -1) {
                print_error("\033[1;91mError: Depth value if set must be -1 or greater.\033[0m\n");
                return 1;
            }
        } else if (arg == "-v" || arg == "--verbose") {
            verbose_enabled = true;
        } else if (arg == "-h" || arg == "--help") {
            std::system("clear");
            print_help();
            return 0;
        } else if (arg == "-c") {
            // Check if -c, -cp, -ce options are mixed
            if (c_flag || cp_flag || ce_flag) {
                print_error("\033[1;91mError: Cannot mix -c, -cp, and -ce options.\033[0m\n");
                return 1;
            }
            c_flag = true;
            if (i + 1 < argc) {
                case_input = argv[++i];
                case_specified = true;
            } else {
                print_error("\033[1;91mError: Missing argument for option " + arg + "\033[0m\n");
                return 1;
            }
        } else if (arg == "-cp") {
            // Check if -c, -cp, -ce options are mixed
            if (c_flag || cp_flag || ce_flag) {
                print_error("\033[1;91mError: Cannot mix -c, -cp, and -ce options.\033[0m\n");
                return 1;
            }
            cp_flag = true;
            rename_parents = true;
            if (i + 1 < argc) {
                case_input = argv[++i];
                case_specified = true;
            } else {
                print_error("\033[1;91mError: Missing argument for option " + arg + "\033[0m\n");
                return 1;
            }
        } else if (arg == "-ce") {
            // Check if -c, -cp, -ce options are mixed
            if (c_flag || cp_flag || ce_flag) {
                print_error("\033[1;91mError: Cannot mix -c, -cp, and -ce options.\033[0m\n");
                return 1;
            }
            
            ce_flag = true;
            rename_extensions = true;
            if (i + 1 < argc) {
                case_input = argv[++i];
                case_specified = true;
            } else {
                print_error("\033[1;91mError: Missing argument for option " + arg + "\033[0m\n");
                return 1;
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

    // Perform renaming based on flags and options
    if (fi_flag && fo_flag) {
        print_error("\033[1;91mError: Cannot mix -fi and -fo options.\033[0m\n");
        return 1;
    }

    if (!case_specified) {
        print_error("\033[1;91mError: Case conversion mode not specified (-c, -cp, or -ce option is required)\033[0m\n");
        return 1;
    }

    // Check for valid case modes
    std::vector<std::string> valid_modes;
    if (cp_flag || c_flag) { // Valid modes for -cp and -ce
        valid_modes = {"lower", "upper", "reverse", "title", "camel", "rcamel", "kebab", "rkebab", "rsnake", "snake", "rnumeric", "rspecial", "rbra", "roperand", "sequence", "rsequence"};
    } else { // Valid modes for -c
        valid_modes = {"lower", "upper", "reverse", "title", "rbak", "bak", "noext"};
    }

    if (std::find(valid_modes.begin(), valid_modes.end(), case_input) == valid_modes.end()) {
        print_error("\033[1;91mError: Unspecified or invalid case mode - " + case_input + ". Run 'bulk_rename++ --help'.\033[0m\n");
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
    if (case_input == "rnumeric" || case_input == "rspecial" || case_input == "rbra" || case_input == "roperand" || case_input == "bak" || case_input == "bak") {
        std::cout << "\033[1;93m!!! WARNING SELECTED OPERATION IRREVERSIBLE !!!\033[0m\n\n";
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
    if (verbose_enabled){
    std::cout << " " << std::endl;
	}
    if (confirmation != "y") {
        std::cout << "\n\033[1;91mOperation aborted by user.\033[0m";
        std::cout << "\n" << std::endl;
        std::cout << "\033[1mPress enter to exit...";
        std::cin.get();
        std::system("clear");
        return 0;
    }

    if (rename_parents) {
        rename_path(paths, case_input, true, verbose_enabled, transform_dirs, transform_files, depth); // Pass true for rename_immediate_parent
    } else if (rename_extensions) {
        int files_count = 0; // Declare files_count here
        rename_extension_path(paths, case_input, verbose_enabled, depth, files_count);
    } else {
        rename_path(paths, case_input, false, verbose_enabled, transform_dirs, transform_files, depth); // Pass false for rename_immediate_parent
    }

    std::cout << "\n\033[1mPress enter to exit...\033[0m";
    std::cin.get();
    std::system("clear");
    return 0;
}
