#include <iostream>
#include <string>
#include <filesystem>
#include <algorithm>
#include <vector>
#include <thread>
#include <mutex>
#include <unistd.h>
#include <chrono>
#include <cctype>
#include <queue>
#include <unordered_map>

namespace fs = std::filesystem;

std::mutex cout_mutex;
std::mutex dirs_count_mutex;
std::mutex files_count_mutex;

// General purpose stuff

std::string append_numbered_prefix(const fs::path& parent_path, const std::string& new_name) {
    // Retrieve the counter for the current directory from the map
    static std::unordered_map<fs::path, int> counter_map;
    int& counter = counter_map[parent_path];

    // Check if new_name already starts with a number
    if (!new_name.empty() && std::isdigit(new_name[0])) {
        return new_name; // If already numbered, return new_name unchanged
    }

    // Increment the counter for the current directory
    int current_counter = counter++;

    // Format the counter with leading zeros
    std::ostringstream oss;
    oss << std::setw(3) << std::setfill('0') << current_counter + 1;

    // Append the counter and underscore to the new name
    return oss.str() + "_" + new_name;
}


std::string remove_numbered_prefix(const std::string& new_name) {
    // Find the position of the first non-digit character
    size_t pos = new_name.find_first_not_of("0123456789");

    // Check if the filename is already numbered and contains an underscore after numbering
    if (pos != std::string::npos && pos > 0 && new_name[pos] == '_' && new_name[pos - 1] != '_') {
        // Remove the number and the first underscore
        return new_name.substr(pos + 1);
    } else if (pos == 0) {
        // Remove only the number if it's at the beginning
        size_t underscore_pos = new_name.find('_');
        if (underscore_pos != std::string::npos && underscore_pos > 0) {
            return new_name.substr(underscore_pos + 1);
        }
    }
    return new_name; // Return the original name if no number found or if number is not followed by an underscore
}


std::string append_date_seq(const std::string& new_name) {
    // Check if the filename already contains a date seq
    size_t dot_position = new_name.find_last_of('.');
    size_t underscore_position = new_name.find_last_of('_');
    if (dot_position != std::string::npos && underscore_position != std::string::npos && dot_position > underscore_position) {
        std::string date_seq = new_name.substr(underscore_position + 1, dot_position - underscore_position - 1);
        if (date_seq.size() == 8 && std::all_of(date_seq.begin(), date_seq.end(), ::isdigit)) {
            // Filename already contains a valid date seq, no need to append
            return new_name;
        }
    } else if (underscore_position != std::string::npos) {
        std::string date_seq = new_name.substr(underscore_position + 1);
        if (date_seq.size() == 8 && std::all_of(date_seq.begin(), date_seq.end(), ::isdigit)) {
            // Filename already contains a valid date seq, no need to append
            return new_name;
        }
    }
    
    // Get current date and time
    auto now = std::chrono::system_clock::now();
    auto time_t_now = std::chrono::system_clock::to_time_t(now);
    std::tm* local_tm = std::localtime(&time_t_now);

    // Format date as YYYYMMDD
    std::ostringstream oss;
    oss << std::put_time(local_tm, "%Y%m%d");
    std::string date_seq = oss.str();

    // Find the position of the last dot in the filename
    if (dot_position != std::string::npos) {
        // Insert date seq before the last dot
        return new_name.substr(0, dot_position) + "_" + date_seq + new_name.substr(dot_position);
    } else {
        // If no dot found, append date seq at the end
        return new_name + "_" + date_seq;
    }
}


std::string remove_date_seq(const std::string& filename) {
    size_t dot_position = filename.find_last_of('.');
    size_t underscore_position = filename.find_last_of('_');
    
    if (underscore_position != std::string::npos) {
        // Check if there's a dot after the underscore
        if (dot_position != std::string::npos && dot_position > underscore_position) {
            std::string date_seq = filename.substr(underscore_position + 1, dot_position - underscore_position - 1);
            // Check if the substring between underscore and dot is a valid date seq
            if (date_seq.size() == 8 && std::all_of(date_seq.begin(), date_seq.end(), ::isdigit)) {
                // Valid date seq found, remove it
                return filename.substr(0, underscore_position) + filename.substr(dot_position);
            }
        } else {
            // No dot found after underscore, consider the substring from underscore to the end as potential date seq
            std::string date_seq = filename.substr(underscore_position + 1);
            // Check if the substring after underscore is a valid date seq
            if (date_seq.size() == 8 && std::all_of(date_seq.begin(), date_seq.end(), ::isdigit)) {
                // Valid date seq found, remove it
                return filename.substr(0, underscore_position);
            }
        }
    }
    
    // No valid date seq found, return original filename
    return filename;
}


std::string swap_transform(const std::string& input) {
    std::string transformed;
    bool capitalize = false; // Start by capitalizing
    bool inFolderName = true; // Start within folder name
    size_t folderDelimiter = input.find_last_of("/\\"); // Find the last folder delimiter

for (size_t i = 0; i < input.length(); ++i) {
    char c = input[i];
    if (i < folderDelimiter || folderDelimiter == std::string::npos) { // Ignore transformation after folder delimiter
        if (inFolderName) {
            transformed += std::toupper(c); // Capitalize first character in folder name
            inFolderName = false; // Exit folder name after first character
        } else {
            if (std::isalpha(c)) {
                if (capitalize) {
                    transformed += std::toupper(c);
                } else {
                    transformed += std::tolower(c);
                }
                capitalize = !capitalize; // Toggle between upper and lower case
            } else {
                transformed += c; // Keep non-alphabetic characters unchanged
            }
        }
    } else {
        transformed += c; // Keep characters after the folder delimiter unchanged
    }
}

    
    return transformed;
}


std::string to_camel_case(const std::string& input) {
    bool hasUpperCase = false;
    bool hasSpace = false;

    // Check if there are any spaces and at least one uppercase letter
    for (char c : input) {
        if (std::isupper(c)) {
            hasUpperCase = true;
        } else if (c == ' ') {
            hasSpace = true;
        }
        if (c == '.') {
            // Stop processing after encountering a period
            break;
        }
    }

    // If there are no spaces and at least one uppercase letter, return input string as it is
    if (!hasSpace && hasUpperCase) {
        return input;
    }

    std::string result;
    result.reserve(input.size()); // Reserve memory for the result string

    bool capitalizeNext = false;
    bool afterDot = false; // Track if we have encountered a period ('.')
    for (char c : input) {
        if (c == '.') {
            afterDot = true;
        }
        if (afterDot) {
            result += c; // Preserve characters after the first period
        } else {
            if (std::isalpha(c)) {
                result += capitalizeNext ? std::toupper(c) : std::tolower(c);
                capitalizeNext = false;
            } else if (c == ' ') {
                capitalizeNext = true;
            } else {
                result += c;
            }
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


void print_error(const std::string& error) {
    std::lock_guard<std::mutex> lock(cout_mutex);
    std::cerr << error << std::endl;
}


void print_verbose_enabled(const std::string& message) {
    std::lock_guard<std::mutex> lock(cout_mutex);
    std::cout << message << std::endl;
}


void print_help() {

std::cout << "\x1B[32mUsage: bulk_rename++ [OPTIONS] [MODE] [PATHS]\n"
          << "Renames all files and folders under the specified path(s).\n"
          << "\n"
          << "Options:\n"
          << "  -h, --help               Print this message and exit\n"
          << "  -v, --verbose            Enable verbose mode\n"
          << "  -fi                      Only files will be renamed, by default everything is renamed\n"
          << "  -fo                      Only folders will be renamed, by default everything is renamed\n"
          << "  -d  [DEPTH]              Set traverse depth level otherwise maximum (-1) is used. -cp and -d 0 together rename only parent dir(s)\n"
          << "  -c  [MODE]               Set the case conversion mode for file and dir name(s)\n"
          << "  -cp [MODE]               Set the case conversion mode for file and dir name(s), including parent dir(s)\n"
          << "  -ce [MODE]               Set the case conversion mode for file extension(s)\n"
          << "\n"
          << "Modes for file, directory and extension renaming:\n"
          << "Regular CASE Modes:\n"
          << "  title      Convert names to Title Case (e.g., test => Test)\n"
          << "  upper      Convert names to UPPERCASE (e.g., Test => TEST)\n"
          << "  lower      Convert names to lowercase (e.g., Test => test)\n"
          << "  reverse    Reverse current case in names (e.g., Test => tEST)\n"
          << "Special CASE Modes:\n"
          << "  snake      Convert spaces to underscores in names (e.g., Te st => Te_st)\n"
          << "  rsnake     Convert underscores to spaces in names (e.g., Te_st => Te st)\n"
          << "  kebab      Convert spaces to hyphens in names (e.g., Te st => Te-st)\n"
          << "  rkebab     Convert hyphens to spaces in names (e.g., Te-st => Te st)\n"
          << "  camel      Convert names to camelCase (e.g., Te st => TeSt)\n"
          << "  rcamel     Reverse camelCase in names (e.g., TeSt => Te st)\n"
          << "Extension CASE Modes:\n"
          << "  bak        Add .bak on file extension names (e.g., Test.txt => Test.txt.bak)\n"
          << "  rbak       Remove .bak from file extension names (e.g., Test.txt.bak => Test.txt)\n"
          << "  noext      Remove extensions (e.g., Test.txt => Test)\n"
		  << "Numerical CASE Modes:\n"
		  << "  nsequence  Apply sequential numbering (e.g., Test => 001_Test)\n"
          << "  rnsequence Remove sequential numbering (e.g., 001_Test => Test)\n"
		  << "  date       Apply current date to files only (e.g., Test => Test_20240215)\n"
		  << "  rdate      Remove date from files (e.g., Test_20240215 => Test)\n"
		  << "  rnumeric   Remove numeric characters from names (e.g., 1Te0st2 => Test)\n"
          << "Custom CASE Modes:\n"
          << "  rbra       Remove [ ] { } ( ) from names (e.g., [{Test}] => Test)\n"
          << "  roperand   Remove - + > < = * from names (e.g., =T-e+s<t> => Test)\n"
          << "  rspecial   Remove special characters from names (e.g., @T!es#$%^|&~`';?t => Test)\n"
          << "  swap       Swap upper-lower case for names including extensions (e.g., Test.txt => TeSt.TxT)\n"
          << "\n"
          << "Examples:\n"
          << "  bulk_rename++ -c lower [path1] [path2]...\n"
          << "  bulk_rename++ -cp upper -d 0 [path1]\n"
          << "  bulk_rename++ -v -cp upper [path1]\n"
          << "  bulk_rename++ -c upper -v [path1]\n"
          << "  bulk_rename++ -d 2 -c upper -v [path1]\n"
          << "  bulk_rename++ -fi -c lower -v [path1]\n"
          << "  bulk_rename++ -ce noext -v [path1]\n"
          << "\n";
}

// Extension stuff

void rename_extension(const fs::path& item_path, const std::string& case_input, bool verbose_enabled, int& files_count) {
    if (!fs::is_regular_file(item_path)) {
        if (verbose_enabled) {
            std::cout << "\033[0m\033[93mSkipped\033[0m " << item_path << " (not a regular file)" << std::endl;
        }
        return;
    }

    std::string extension = item_path.extension().string();
    std::string new_extension = extension;

    if (case_input == "lower") {
        std::transform(extension.begin(), extension.end(), new_extension.begin(), ::tolower);
    } else if (case_input == "upper") {
        std::transform(extension.begin(), extension.end(), new_extension.begin(), ::toupper);
    } else if (case_input == "reverse") {
        std::transform(extension.begin(), extension.end(), new_extension.begin(), [](char c) {
            return std::islower(c) ? std::toupper(c) : std::tolower(c);
        });
    } else if (case_input == "title") {
        std::string temp_extension = extension;
        if (!temp_extension.empty()) {
            temp_extension[0] = std::toupper(temp_extension[0]);
            std::transform(temp_extension.begin() + 1, temp_extension.end(), temp_extension.begin() + 1, ::tolower);
            new_extension = temp_extension;
        }
    } else if (case_input == "bak") {
        if (extension.length() < 4 || extension.substr(extension.length() - 4) != ".bak") {
            new_extension = extension + ".bak";
        } else {
            new_extension = extension; // Keep the extension unchanged
        }
    } else if (case_input == "rbak") {
        if (extension.length() >= 4 && extension.substr(extension.length() - 4) == ".bak") {
            new_extension = extension.substr(0, extension.length() - 4);
        }
    } else if (case_input == "noext") {
        new_extension.clear(); // Clearing extension removes it
    } else if (case_input == "swap") {
        new_extension = swap_transform(extension);
    }

    if (extension != new_extension) {
        fs::path new_path = item_path.parent_path() / (item_path.stem().string() + new_extension);
        try {
            fs::rename(item_path, new_path);
            ++files_count;
            if (verbose_enabled) {
                std::lock_guard<std::mutex> lock(files_count_mutex);
                std::cout << "\033[0m\033[92mRenamed\033[0m file " << item_path.string() << " to " << new_path.string() << std::endl;
            }
        } catch (const fs::filesystem_error& e) {
            std::cerr << "\033[1;91mError\033[0m: " << e.what() << "\n" << std::endl;
        }
    } else {
        if (verbose_enabled) {
            std::cout << "\033[0m\033[93mSkipped\033[0m file " << item_path.string();
            if (extension.empty()) {
                std::cout << " (no extension)";
            } else {
                std::cout << " (extension unchanged)";
            }
            std::cout << std::endl;
        }
    }
}


void rename_extension_path(const std::vector<std::string>& paths, const std::string& case_input, bool verbose_enabled, int depth, int& files_count) {
    //If depth is negative (default value), set it to a very large number to effectively disable the depth limit
    if (depth < 0) {
        depth = std::numeric_limits<int>::max();
    }

    auto start_time = std::chrono::steady_clock::now(); // Start time measurement

    // Get the maximum number of threads supported by the system
    unsigned int max_threads = std::thread::hardware_concurrency();
    if (max_threads == 0) {
        max_threads = 1; // If hardware concurrency is not available, default to 1 thread
    }

    std::vector<std::thread> threads; // Vector to store threads

    // Determine the number of threads to create (minimum of max_threads and paths.size())
    unsigned int num_threads = std::min(max_threads, static_cast<unsigned int>(paths.size()));

    std::string depth_limit_reached_path; // Store the path where depth limit is reached

    for (unsigned int i = 0; i < num_threads; ++i) {
        threads.emplace_back([&paths, i, &case_input, verbose_enabled, depth, &files_count, &depth_limit_reached_path]() {
            // Each thread handles a subset of paths based on its index i
            // Example: process paths[i], paths[i + num_threads], paths[i + 2*num_threads], ...
            std::queue<std::pair<std::string, int>> directories; // Queue to store directories and their depths
            directories.push({paths[i], 0}); // Push the initial path onto the queue with depth 0

            while (!directories.empty()) {
                auto [current_path, current_depth] = directories.front();
                directories.pop();

                if (current_depth >= depth && depth_limit_reached_path.empty()) {
                    depth_limit_reached_path = current_path; // Store the path where depth limit is reached
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

    std::cout << "\n\033[1mRenamed \033[4mfile extensions\033[0m\033[1m to \033[1;38;5;214m" << case_input << "_case\033[0m\033[1m: for \033[1;92m" << files_count << " file(s) \033[0m\033[1mfrom \033[1;95m" << paths.size()
              << " input path(s) \033[0m\033[1min " << std::setprecision(1)
              << std::fixed << elapsed_seconds.count() << "\033[1m second(s)\n";
}


// Rename file&directory stuff

void rename_file(const fs::path& item_path, const std::string& case_input, bool is_directory, bool verbose_enabled, bool transform_dirs, bool transform_files, int& files_count, int& dirs_count) {
    // Check if the item is a directory
    if (is_directory) {
        // If it is a directory, recursively process its contents
        for (const auto& entry : fs::directory_iterator(item_path)) {
            if (entry.is_directory()) {
                // Recursively call rename_file for subdirectories
                rename_file(entry.path(), case_input, true, verbose_enabled, transform_dirs, transform_files, files_count, dirs_count);
            } else {
                // Rename files within the subdirectory
                rename_file(entry.path(), case_input, false, verbose_enabled, transform_dirs, transform_files, files_count, dirs_count);
            }
        }
        // Increment the directory count
        std::lock_guard<std::mutex> lock(dirs_count_mutex);
        ++dirs_count;
        return;
    }

    // Extract the relative path of the item from its parent directory
    fs::path relative_path = item_path.filename();
    fs::path parent_path = item_path.parent_path();

    // Initialize variables for new name and path
    std::string name = relative_path.string();
    std::string new_name = name;
    fs::path new_path;

    // Static list of transformation commands
    static const std::vector<std::string> transformation_commands = {
        "lower", "upper", "reverse", "title", "snake", "rsnake", "rspecial", 
        "rnumeric", "rbra", "roperand", "camel", "rcamel", "kebab", "rkebab", 
        "nsequence", "rnsequence", "date", "rdate", "swap"
    };

    for (const auto& transformation : transformation_commands) {
        if (case_input.find(transformation) != std::string::npos) {
            // Apply the corresponding transformation
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
            } else if (transformation == "nsequence") {
                // Check if the filename is already numbered
                new_name = append_numbered_prefix(parent_path, new_name);
            } else if (transformation == "rnsequence") {
                new_name = remove_numbered_prefix(new_name);
            } else if (transformation == "date") {
				new_name = append_date_seq(new_name);
			} 
			else if (transformation == "swap") {
				new_name = swap_transform(new_name);	
			} 
			else if (transformation == "rdate") {
				new_name = remove_date_seq(new_name);	
			}
		}

	}
    // Skip renaming if the new name is the same as the old name
    if (name != new_name) {
        // Construct the new path within the same parent directory
        new_path = parent_path / std::move(new_name);

        try {
            // Rename the file
            fs::rename(item_path, new_path);

            if (verbose_enabled) {
                print_verbose_enabled("\033[0m\033[92mRenamed\033[0m file " + item_path.string() + " to " + new_path.string());
            }
            // Increment the file count
            std::lock_guard<std::mutex> lock(files_count_mutex);
            ++files_count;
        } catch (const fs::filesystem_error& e) {
            std::cerr << "\033[1;91mError\033[0m: " << e.what() << "\n" << std::endl;
        }
    } else {
        // Print a message if the name remains unchanged
        if (verbose_enabled && (!transform_dirs || (transform_dirs && transform_files))) {
            print_verbose_enabled("\033[0m\033[93mSkipped\033[0m file " + item_path.string() + " (name unchanged)");
        }
    }
}


void remove_sequential_numbering(const fs::path& base_directory, int& dirs_count) {
    for (const auto& folder : fs::directory_iterator(base_directory)) {
        if (folder.is_directory()) {
            std::string folder_name = folder.path().filename().string();

            // Check if the folder is numbered
            size_t underscore_pos = folder_name.find('_');
            if (underscore_pos != std::string::npos && std::isdigit(folder_name[0])) {
                // Extract the original name without the numbering
                std::string original_name = folder_name.substr(underscore_pos + 1);

                // Construct the new name without numbering
                fs::path new_name = base_directory / original_name;

                // Check if the folder is already renamed to the new name
                if (folder.path() != new_name) {
                    // Move the contents of the source directory to the destination directory
                    try {
                        fs::rename(folder.path(), new_name);
                    } catch (const fs::filesystem_error& e) {
                        if (e.code() == std::errc::permission_denied) {
                            std::cerr << "Error renaming folder due to permission denied: " << e.what() << std::endl;
                        }
                        continue; // Skip renaming if moving fails
                    }

                    std::cout << "Removed numbering from folder: " << folder.path() << " to " << new_name << std::endl;
                    ++dirs_count; // Increment dirs_count after each successful rename
                }

                // Recursively process subdirectories
                remove_sequential_numbering(new_name, dirs_count);
            }
        }
    }
}


void rename_folders_with_sequential_numbering(const fs::path& base_directory, std::string prefix, int& dirs_count) {
    int counter = 1; // Counter for immediate subdirectories
    for (const auto& folder : fs::directory_iterator(base_directory)) {
        if (folder.is_directory()) {
            std::string folder_name = folder.path().filename().string();

            // Check if the folder is already numbered
            if (folder_name.find('_') != std::string::npos && std::isdigit(folder_name[0])) {
                // Skip renaming if already numbered
                continue;
            }

            // Construct the new name with sequential numbering and original name
            std::stringstream ss;
            ss << std::setw(3) << std::setfill('0') << counter << "_" << folder_name; // Append original name to the numbering
            fs::path new_name = base_directory / (prefix.empty() ? "" : (prefix + "_")) / ss.str(); // Corrected the concatenation

            // Check if the folder is already renamed to the new name
            if (folder.path() != new_name) {
                // Move the contents of the source directory to the destination directory
                try {
                    fs::rename(folder.path(), new_name);
                } catch (const fs::filesystem_error& e) {
                    if (e.code() == std::errc::permission_denied) {
                        std::cerr << "Error renaming folder due to permission denied: " << e.what() << std::endl;
                    }
                    continue; // Skip renaming if moving fails
                }

                std::cout << "Renamed folder: " << folder.path() << " to " << new_name << std::endl;
                ++dirs_count; // Increment dirs_count after each successful rename
            }

            // Recursively process subdirectories with updated prefix
            rename_folders_with_sequential_numbering(new_name, prefix + ss.str(), dirs_count);
            counter++; // Increment counter after each directory is processed
        }
    }
}

// Call this function to start the renaming process
void rename_folders_with_sequential_numbering(const fs::path& base_directory, int& dirs_count) {
    rename_folders_with_sequential_numbering(base_directory, "", dirs_count);
}


void rename_directory(const fs::path& directory_path, const std::string& case_input, bool rename_parents, bool verbose_enabled, bool transform_dirs, bool transform_files, int& files_count, int& dirs_count, int depth) {
    std::string dirname = directory_path.filename().string();
    std::string new_dirname = dirname; // Initialize with original name
    bool renaming_message_printed = false;

    // Static list of transformation commands
    static const std::vector<std::string> transformation_commands = {
        "lower", "upper", "reverse", "title", "snake", "rsnake", "rspecial",
        "rnumeric", "rbra", "roperand", "camel", "rcamel", "kebab", "rkebab", "swap,"
        "nsequence", "rnsequence"
    };

    // Early exit if directory is a symlink
    if (fs::is_symlink(directory_path)) {
        if (verbose_enabled) {
            print_verbose_enabled("\033[0m\033[93mSkipped\033[0m symlink " + directory_path.string() + " (not supported)");
        }
        return;
    }

    if (transform_dirs) {
        for (const auto& transformation : transformation_commands) {
            if (case_input == transformation) {
                if (transformation == "lower") {
                    std::transform(new_dirname.begin(), new_dirname.end(), new_dirname.begin(), ::tolower);
                } else if (transformation == "upper") {
                    std::transform(new_dirname.begin(), new_dirname.end(), new_dirname.begin(), ::toupper);
                } else if (transformation == "reverse") {
                    std::transform(new_dirname.begin(), new_dirname.end(), new_dirname.begin(), [](unsigned char c) {
                        return std::islower(c) ? std::toupper(c) : std::tolower(c);
                    });
                } else if (transformation == "title") {
                    bool first_letter = true;
                    new_dirname.reserve(dirname.size());
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
                    std::replace(new_dirname.begin(), new_dirname.end(), ' ', '_');
                } else if (transformation == "rsnake") {
                    std::replace(new_dirname.begin(), new_dirname.end(), '_', ' ');
                } else if (transformation == "kebab") {
                    std::replace(new_dirname.begin(), new_dirname.end(), ' ', '-');
                } else if (transformation == "rkebab") {
                    std::replace(new_dirname.begin(), new_dirname.end(), '-', ' ');
                } else if (transformation == "rspecial") {
                    new_dirname.erase(std::remove_if(new_dirname.begin(), new_dirname.end(), [](char c) {
                        return !std::isalnum(c) && c != '.' && c != '_' && c != '-' && c != '(' && c != ')' && c != '[' && c != ']' && c != '{' && c != '}' && c != '+' && c != '*' && c != '<' && c != '>' && c != ' ';
                    }), new_dirname.end());
                } else if (transformation == "rnumeric") {
                    new_dirname.erase(std::remove_if(new_dirname.begin(), new_dirname.end(), [](char c) {
                        return std::isdigit(c);
                    }), new_dirname.end());
                } else if (transformation == "rbra") {
                    new_dirname.erase(std::remove_if(new_dirname.begin(), new_dirname.end(), [](char c) {
                        return c == '[' || c == ']' || c == '{' || c == '}' || c == '(' || c == ')';
                    }), new_dirname.end());
                } else if (transformation == "roperand") {
                    new_dirname.erase(std::remove_if(new_dirname.begin(), new_dirname.end(), [](char c) {
                        return c == '-' || c == '+' || c == '>' || c == '<' || c == '=' || c == '*';
                    }), new_dirname.end());
                } else if (transformation == "camel") {
                    new_dirname = to_camel_case(new_dirname);
                } else if (transformation == "rcamel") {
                    new_dirname = from_camel_case(new_dirname);
                } else if (transformation == "swap") {
                    rename_folders_with_sequential_numbering(directory_path, dirs_count);
                }
                else if (transformation == "nsequence") {
                    rename_folders_with_sequential_numbering(directory_path, dirs_count);
                }
                else if (transformation == "rnsequence") {
                    remove_sequential_numbering(directory_path, dirs_count);
                }
            }
        }
    }

    fs::path new_path = directory_path.parent_path() / std::move(new_dirname); // Move new_dirname instead of copying

    // Check if renaming is necessary
    if (directory_path != new_path) {
        try {
            fs::rename(directory_path, new_path);

            if (verbose_enabled && !renaming_message_printed) {
                print_verbose_enabled("\033[0m\033[92mRenamed\033[0m\033[94m directory\033[0m " + directory_path.string() + " to " + new_path.string());
                renaming_message_printed = true; // Set the flag to true after printing the message
            }
            std::lock_guard<std::mutex> lock(dirs_count_mutex);
            ++dirs_count;
        } catch (const fs::filesystem_error& e) {
            std::cerr << "\033[1;91mError\033[0m: " << e.what() << "\n" << std::endl;
            return; // Stop processing if renaming failed
        }

    } else {
        if (verbose_enabled && !transform_files) {
            print_verbose_enabled("\033[0m\033[93mSkipped\033[0m\033[94m directory\033[0m " + directory_path.string() + " (name unchanged)");
        } else if (verbose_enabled && transform_dirs && transform_files) {
            print_verbose_enabled("\033[0m\033[93mSkipped\033[0m\033[94m directory\033[0m " + directory_path.string() + " (name unchanged)");
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

        if (rename_parents) {
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
    }
}



void rename_path(const std::vector<std::string>& paths, const std::string& case_input, bool rename_parents, bool verbose_enabled,bool transform_dirs, bool transform_files, int depth)  {
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
                if (rename_parents) {
                    // If -p option is used, only rename the immediate parent
                    fs::path immediate_parent_path = current_path.parent_path();
                    rename_directory(immediate_parent_path, case_input, rename_parents, verbose_enabled,transform_dirs,transform_files, files_count, dirs_count, depth);
                } else {
                    // Otherwise, rename the entire path
                    threads.emplace_back(rename_directory, current_path, case_input, rename_parents, verbose_enabled, transform_dirs, transform_files, std::ref(files_count), std::ref(dirs_count), depth);
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
                if (rename_parents) {
                    // If -p option is used, only rename the immediate parent
                    fs::path immediate_parent_path = current_path.parent_path();
                    rename_directory(immediate_parent_path, case_input, rename_parents, verbose_enabled,transform_dirs,transform_files, files_count, dirs_count, depth);
                } else {
                    // Otherwise, rename the entire path in the main thread
                    rename_directory(current_path, case_input, rename_parents, verbose_enabled,transform_dirs,transform_files, files_count, dirs_count, depth);
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

    std::cout << "\n\033[0m\033[1mRenamed to \033[1;38;5;214m" << case_input << "_case\033[0m\033[1m: \033[1;92m" << files_count << " file(s) \033[0m\033[1mand \033[1;94m"
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
    int files_count = 0;
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
        valid_modes = {"lower", "upper", "reverse", "title", "date", "swap","rdate", "camel", "rcamel", "kebab", "rkebab", "rsnake", "snake", "rnumeric", "rspecial", "rbra", "roperand", "nsequence", "rnsequence"};
    } else { // Valid modes for -c
        valid_modes = {"lower", "upper", "reverse", "title", "swap", "rbak", "bak", "noext"};
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
    if (case_input == "rnumeric" || case_input == "rspecial" || case_input == "rbra" || case_input == "roperand" || case_input == "noext") {
        std::cout << "\033[1;93m!!! WARNING SELECTED OPERATION IRREVERSIBLE !!!\033[0m\n\n";
    }

    std::string confirmation;
if (rename_parents) {
    std::cout << "\033[0m\033[1mThe following path(s), along with their \033[4mlowest Parent\033[0m\033[1m dir(s), will be recursively renamed to \033[0m\e[1;38;5;214m" << case_input << "_case\033[0m";
    if (depth != -1) {
        std::cout << "\033[0m\033[1m (up to depth " << depth << ")";
    }
    if (!transform_dirs) {
        std::cout << "\033[0m\033[1m (excluding directories)";
    }
    if (!transform_files) {
        std::cout << "\033[0m\033[1m (excluding files)";
    }
    std::cout << ":\033[1m\n\n";
    for (const auto& path : paths) {
        std::cout << "\033[1;94m" << path << "\033[0m" << std::endl;
    }
} else if (rename_extensions) {
    std::cout << "\033[0m\033[1mThe file \033[4mextensions\033[0m\033[1m under the following path(s) \033[1mwill be recursively renamed to \033[0m\e[1;38;5;214m" << case_input << "_case\033[0m";
    if (depth != -1) {
        std::cout << "\033[0m\033[1m (up to depth " << depth << ")";
    }
    std::cout << ":\033[1m\n\n";
    for (const auto& path : paths) {
        std::cout << "\033[1;94m" << path << "\033[0m" << std::endl;
    }
} else {
    std::cout << "\033[0m\033[1mThe following path(s) will be recursively renamed to \033[0m\e[1;38;5;214m" << case_input << "_case\033[0m";
    if (depth != -1) {
        std::cout << "\033[0m\033[1m (up to depth " << depth << ")";
    }
    if (!transform_dirs && rename_parents) {
        std::cout << "\033[0m\033[1m (excluding both files and directories)";
    } else if (!transform_dirs) {
        std::cout << "\033[0m\033[1m (excluding directories)";
    } else if (!transform_files) {
        std::cout << "\033[0m\033[1m (excluding files)";
    }
    std::cout << ":\033[1m\n\n";
    for (const auto& path : paths) {
        std::cout << "\033[1;94m" << path << "\033[0m" << std::endl;
    }
}
    std::cout << "\n\033[1mDo you want to proceed? (y/n): ";
    std::getline(std::cin, confirmation);
    if (verbose_enabled && confirmation == "y"){
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
        rename_path(paths, case_input, true, verbose_enabled, transform_dirs, transform_files, depth); // Pass true for rename_parents
    } else if (rename_extensions) {
        rename_extension_path(paths, case_input, verbose_enabled, depth, files_count);
    }
		else {
			rename_path(paths, case_input, rename_parents, verbose_enabled, transform_dirs, transform_files, depth);
		}
			

    std::cout << "\n\033[1mPress enter to exit...\033[0m";
    std::cin.get();
    std::system("clear");
    return 0;
}
