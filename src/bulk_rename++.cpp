// SPDX-License-Identifier: GNU General Public License v3.0 or later

#include "headers.h"


// General purpose stuff

// Get the number of available processor cores
unsigned int max_threads = (omp_get_num_procs() <= 0) ? 2 : omp_get_num_procs();

// Terminal blocking/unblocking

// Function to flush input buffer
void flushStdin() {
    tcflush(STDIN_FILENO, TCIFLUSH);
}


// Function to disable input during processing
void disableInput() {
    struct termios term;
    tcgetattr(STDIN_FILENO, &term);
    term.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &term);
}


// Function to restore normal input
void restoreInput() {
    struct termios term;
    tcgetattr(STDIN_FILENO, &term);
    term.c_lflag |= ICANON | ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &term);
}


// Global print functions

// Thread-Local Storage for error messages
thread_local std::ostringstream thread_error_stream;

// Print an error message to stderr in a thread safe manner
void print_error(const std::string& error, std::ostream& out) {
    thread_error_stream.str("");  // Clear the buffer
    thread_error_stream << error;
    out << thread_error_stream.str() << std::endl;
}


// Thread-Local Storage for messages
thread_local std::ostringstream thread_stream;

// Print a message to stdout, assuming verbose mode is enabled in a thread safe manner
void print_verbose_enabled(const std::string& message, std::ostream& out) {
    thread_stream.str("");  // Clear the buffer
    thread_stream << message;
    out << thread_stream.str() << std::endl;
}


// Print the version number of the program
void printVersionNumber(const std::string& version) {
    
    std::cout << "\x1B[32mBulk Rename Plus v" << version << "\x1B[0m\n"; // Output the version number in green color
}


// Function to clear scrollbuffer
void clearScrollBuffer() {
    std::cout << "\033[3J";  // Clear the scrollback buffer
    std::cout << "\033[2J";  // Clear the screen
    std::cout << "\033[H";   // Move the cursor to the top-left corner
    std::cout << "\033[0m"; // Restore default shell theme
    std::cout.flush();       // Ensure the output is flushed
}


// Function to print help
void print_help() {

std::cout << "\n\x1B[32mUsage: bulk_rename++ [OPTIONS] [MODE] [PATHS]\n"
          << "Renames all files and folders under the specified path(s).\n"
          << "\n"
          << "Options:\n"
          << "  -h, --help               Print help\n"
          << "  --version                Print version\n"
          << "  -v, --verbose            Activate verbose mode for renamed (optional)\n"
          << "  -vs                      Activate verbose mode for renamed + skipped (optional)\n"
          << "  -vso                     Activate verbose mode for skipped (optional)\n"
          << "  -ni                      Activate headless mode (optional)\n"
          << "  -fi                      Rename files exclusively (optional)\n"
          << "  -fo                      Rename folders exclusively (optional)\n"
          << "  -sym                     Handle symlinks like regular files + folders (optional)\n"
          << "  -d  [DEPTH]              Set recursive depth level (optional)\n"
          << "  -c  [MODE]               Set Case Mode for file + folder - parent names\n"
          << "  -cp [MODE]               Set Case Mode for file + folder + parent names\n"
          << "  -ce [MODE]               Set Case Mode for file extension names\n"
          << "\n"
          << "Available Modes:\n"
          << "Regular CASE Modes:\n"
          << "  title      Convert names to titleCase (e.g., test => Test)\n"
          << "  upper      Convert names to upperCase (e.g., Test => TEST)\n"
          << "  lower      Convert names to lowerCase (e.g., Test => test)\n"
          << "  reverse    Reverse current Case in names (e.g., Test => tEST)\n"
          << "Special CASE Modes:\n"
          << "  snake      Convert names to snakeCase (e.g., Te st => Te_st)\n"
          << "  rsnake     Reverse snakeCase in names (e.g., Te_st => Te st)\n"
          << "  kebab      Convert names to kebabCase (e.g., Te st => Te-st)\n"
          << "  rkebab     Reverse kebabCase in names (e.g., Te-st => Te st)\n"
          << "  camel      Convert names to camelCase (e.g., Te st => teSt)\n"
          << "  rcamel     Reverse camelCase in names (e.g., TeSt => te st)\n"
          << "  pascal     Convert names to pascalCase (e.g., Te st => TeSt)\n"
          << "  rpascal    Reverse pascalCase in names (e.g., TeSt => Te St)\n"
          << "  sentence   Convert names to sentenceCase (e.g., Te st => Te St)\n"
          << "Extension CASE Modes:\n"
          << "  bak        Add .bak on file extension names (e.g., Test.txt => Test.txt.bak)\n"
          << "  rbak       Remove .bak from file extension names (e.g., Test.txt.bak => Test.txt)\n"
          << "  noext      Remove file extensions (e.g., Test.txt => Test)\n"
	  << "Numerical CASE Modes:\n"
	  << "  sequence   Append numeric sequence to names alphabetically (e.g., Test => 001_Test)\n"
          << "  rsequence  Remove numeric sequence from names (e.g., 001_Test => Test)\n"
	  << "  date       Append current date to names (e.g., Test => Test_20240215)\n"
	  << "  rdate      Remove date from names (e.g., Test_20240215 => Test)\n"
	  << "  rnumeric   Remove numeric characters from names (e.g., 1Te0st2 => Test)\n"
          << "Custom CASE Modes:\n"
          << "  rbra       Remove [ ] { } ( ) from names (e.g., [{Test}] => Test)\n"
          << "  roperand   Remove - + > < = * from names (e.g., =T-e+s<t> => Test)\n"
          << "  rspecial   Remove special characters from names (e.g., @T!es#$%^|&~`';?t => Test)\n"
          << "  swap       Swap upper-lower case for names (e.g., Test => TeSt)\n"
	  << "  swapr      Swap lower-upper case for names (e.g., Test => tEsT)\n"
          << "\n"
          << "Examples:\n"
          << "  bulk_rename++ -c lower [path1] [path2]...\n"
          << "  bulk_rename++ -d 0 -cp upper [path1]\n"
          << "  bulk_rename++ -v -cp upper [path1]\n"
          << "  bulk_rename++ -c upper -v [path1]\n"
          << "  bulk_rename++ -d 2 -c upper -v [path1]\n"
          << "  bulk_rename++ -fi -c lower -vs [path1]\n"
          << "  bulk_rename++ -ce noext -v [path1]\n"
          << "  bulk_rename++ -sym -c lower -vso [path1]\n"
          << "  bulk_rename++ -sym -fi -c title -v [path1]\n"
          << "\x1B[0m\n";
}


// Extension stuff

// Global static transformations
static const std::vector<std::string> transformation_commands = {
    "lower",      // Convert to lowercase
    "upper",      // Convert to uppercase
    "reverse",    // Reverse the string
    "title",      // Convert to title case
    "snake",      // Convert to snake_case
    "rsnake",     // Convert to reverse snake_case
    "rspecial",   // Reverse special characters
    "rnumeric",   // Reverse numeric characters
    "rbra",       // Reverse brackets
    "roperand",   // Reverse operands
    "camel",      // Convert to camelCase
    "rcamel",     // Convert to reverse camelCase
    "kebab",      // Convert to kebab-case
    "rkebab",     // Convert to reverse kebab-case
    "sequence",  // Normalize sequence numbers
    "rsequence", // Reverse and normalize sequence numbers
    "date",       // Format date
    "rdate",      // Reverse date format
    "swap",       // Swap case
    "swapr",      // Reverse swap case
    "sentence",   // Convert to sentence case
    "pascal",     // Convert to PascalCase
    "rpascal",    // Convert to reverse PascalCase
    "bak",        // Backup
    "rbak",       // Reverse backup
    "noext"       // Remove extension
};


// Function to rename file extensions
void rename_extension(const std::vector<fs::path>& item_paths, const std::string& case_input, bool verbose_enabled, std::atomic<int>& files_count, size_t batch_size, bool symlinks, std::atomic<int>& skipped_file_count, bool skipped, bool skipped_only) {
    // Local vector to store pairs of old and new paths for renaming (per thread)
    std::vector<std::pair<fs::path, fs::path>> rename_batch;
    rename_batch.reserve(item_paths.size()); // Reserve space for efficiency

    // Iterate through each item path
    for (const auto& item_path : item_paths) {
        // Check if the item is a directory or a symlink
        if (fs::is_symlink(item_path) && !symlinks) {
            ++skipped_file_count;
            // Skip if it's a symlink, print a message if verbose mode enabled
            if (verbose_enabled && skipped) {
                print_verbose_enabled("\033[0m\033[93mSkipped\033[0m \033[95msymlink_file\033[0m " + item_path.string() + " (excluded)", std::cout);
            }
            continue;
        }

        // Get the current extension of the file
        std::string extension = item_path.extension().string();
        std::string new_extension = extension;

        // Check if the requested case transformation is valid and apply it
        if (std::find(transformation_commands.begin(), transformation_commands.end(), case_input) != transformation_commands.end()) {
            // Case input is valid, perform appropriate transformation
            if (case_input == "lower") {
                std::transform(extension.begin(), extension.end(), new_extension.begin(), ::tolower);
            } else if (case_input == "upper") {
                std::transform(extension.begin(), extension.end(), new_extension.begin(), ::toupper);
            } else if (case_input == "reverse") {
                std::transform(extension.begin(), extension.end(), new_extension.begin(), [](char c) {
                    return std::islower(c) ? std::toupper(c) : std::tolower(c);
                });
            } else if (case_input == "title") {
                new_extension = capitalizeFirstLetter(new_extension);
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
                new_extension = swapr_transform(extension);
            } else if (case_input == "swapr") {
                new_extension = swap_transform(extension);
            }

            if (extension == new_extension) {
                ++skipped_file_count;
            }

            // If extension changed, create new path and add to rename batch
            if (extension != new_extension) {
                fs::path new_path = item_path.parent_path() / (item_path.stem().string() + new_extension);
                rename_batch.emplace_back(item_path, new_path); // Add to the batch
            } else {
                if (skipped && verbose_enabled && ((!fs::is_regular_file(item_path) && !fs::is_symlink(item_path)) || (fs::is_symlink(item_path) && symlinks))) {
                    print_verbose_enabled("\033[0m\033[93mSkipped\033[0m \033[95msymlink_file\033[0m " + item_path.string() + (extension.empty() ? " (no name change)" : " (name unchanged)"), std::cout);
                }
                // Print a message for skipped file if extension remains unchanged and parent directory is not a symlink
                if (verbose_enabled && !fs::is_symlink(item_path.parent_path()) && !symlinks && skipped) {
                    print_verbose_enabled("\033[0m\033[93mSkipped\033[0m file " + item_path.string() + (extension.empty() ? " (no extension)" : " (extension unchanged)"), std::cout);
                } else if (verbose_enabled && skipped) {
                    print_verbose_enabled("\033[0m\033[93mSkipped\033[0m file " + item_path.string() + (extension.empty() ? " (no extension)" : " (extension unchanged)"), std::cout);
                }
            }
        }

        // Batch processing: if batch size reached, rename batch and clear
        if (rename_batch.size() >= batch_size) {
            batch_rename_extension(rename_batch, verbose_enabled, files_count, skipped_only);
            rename_batch.clear(); // Clear the batch after processing
        }
    }

    // Process remaining items in the batch if any
    if (!rename_batch.empty()) {
        batch_rename_extension(rename_batch, verbose_enabled, files_count, skipped_only);
    }
}



// Function to rename a batch of files using multiple threads for parallel execution
void batch_rename_extension(const std::vector<std::pair<fs::path, fs::path>>& data, bool verbose_enabled, std::atomic<int>& files_count, bool skipped_only) {
    int num_threads = std::min(static_cast<int>(data.size()), static_cast<int>(max_threads));
    #pragma omp parallel for schedule(dynamic) num_threads(num_threads)
    for (std::size_t i = 0; i < data.size(); ++i) {
        const auto& [old_path, new_path] = data[i];

        try {
            // Attempt to rename the file
            fs::rename(old_path, new_path);

            // Safely increment files_count when a file is successfully renamed
            files_count.fetch_add(1, std::memory_order_relaxed);

            // Print a success message if verbose mode enabled
            if (verbose_enabled && !skipped_only) {
                if (fs::is_symlink(old_path) || fs::is_symlink(new_path)) {
                    print_verbose_enabled("\033[0m\033[92mRenamed\033[0m \033[95msymlink_file\033[0m " + old_path.string() + "\e[1;38;5;214m -> \033[0m" + new_path.string(), std::cout);
                } else {
                    // Use fs::status to check the file status after renaming
                    fs::file_status status = fs::status(new_path);

                    if (fs::is_regular_file(status)) {
                        print_verbose_enabled("\033[0m\033[92mRenamed\033[0m file " + old_path.string() + "\e[1;38;5;214m -> \033[0m" + new_path.string(), std::cout);
                    }
                }
            }
        } catch (const fs::filesystem_error& e) {
            // Print an error message if renaming fails
            if (e.code() == std::errc::permission_denied) {
                if (verbose_enabled) {
                    print_error("\033[1;91mError\033[0m: " + std::string(e.what()) + "\n", std::cerr);
                }
            }
        }
    }
}


// Function to search subdirs for file extensions recursively for multiple paths in parallel
void rename_extension_path(const std::vector<std::string>& paths, const std::string& case_input, bool verbose_enabled, std::atomic<int>& depth, std::atomic<int>& files_count, size_t batch_size_files, bool symlinks, std::atomic<int>& skipped_file_count, bool skipped, bool skipped_only, bool non_interactive) {
    // If depth is negative, set it to a very large number to effectively disable the depth limit
    if (depth < 0) {
        depth = std::numeric_limits<int>::max();
    }

    auto start_time = std::chrono::steady_clock::now(); // Start time measurement

    // Get the number of available threads
    int num_threads = max_threads;

    // Define the function to process each path
    auto process_paths = [&](int start_index, int end_index) {
        for (int i = start_index; i < end_index; ++i) {
            const auto& current_fs_path = paths[i];
            std::queue<std::pair<fs::path, int>> directories; // Queue to store directories and their depths
            directories.push({fs::path(current_fs_path), 0}); // Push the initial path onto the queue with depth 0

            while (!directories.empty()) {
                auto [current_path, current_depth] = directories.front();
                directories.pop();

                // Check if depth limit is reached
                if (current_depth >= depth) {
                    continue;
                }

                try {
                    if (fs::is_directory(current_path)) {
                        for (const auto& entry : fs::directory_iterator(current_path)) {
                            if (fs::is_symlink(entry) && !symlinks) {
                                if (verbose_enabled && skipped) {
                                    if (fs::is_directory(entry)) {
                                        std::cout << "\n\033[0m\033[93mSkipped\033[0m processing \033[95msymlink_folder\033[0m " << entry.path().string() << " (excluded)";
                                    } else {
                                        std::cout << "\n\033[0m\033[93mSkipped\033[0m \033[95msymlink_file\033[0m " << entry.path().string() << " (excluded)";
                                    }
                                }
                                continue;
                            }
                            
                            if (fs::is_directory(entry)) {
                                directories.push({entry.path(), current_depth + 1});
                            } else if (fs::is_regular_file(entry)) {
                                rename_extension({entry.path()}, case_input, verbose_enabled, files_count, batch_size_files, symlinks, skipped_file_count, skipped, skipped_only);
                            }
                        }
                    } else if (fs::is_regular_file(current_path)) {
                        rename_extension({current_path}, case_input, verbose_enabled, files_count, batch_size_files, symlinks, skipped_file_count, skipped, skipped_only);
                    }
                } catch (const std::exception& ex) {
                    if (verbose_enabled) {
                        std::cerr << "\n\033[1;91mError processing path\033[0m: " << current_path.string() << " - " << ex.what();
                    }
                }
            }
        }
    };

    // Divide the paths into chunks based on the number of available threads
    int chunk_size = paths.size() / num_threads;

    // Process paths in parallel using OpenMP
    #pragma omp parallel
    {
        int thread_id = omp_get_thread_num();
        int start_index = thread_id * chunk_size;
        int end_index = (thread_id == num_threads - 1) ? paths.size() : start_index + chunk_size;

        process_paths(start_index, end_index);
    }

    auto end_time = std::chrono::steady_clock::now(); // End time measurement

    // Calculate elapsed time
    std::chrono::duration<double> elapsed_seconds = end_time - start_time;
    if (verbose_enabled) {
        std::cout << "\n";
    }
    if (!non_interactive || verbose_enabled) {
        // Print summary
        std::cout << "\n\033[1mRenamed: \033[1;92m" << files_count << " file(s) \033[0;1m | Skipped: \033[1;93m" << skipped_file_count << " file(s)\033[0;1m | \033[1mFrom: \033[1;95m" << paths.size()
                  << " input path(s) \033[0;1m" << "\n\n\033[0;1mTime Elapsed: " << std::setprecision(1)
                  << std::fixed << elapsed_seconds.count() << "\033[1m second(s)\n\n";
    }
}


// Rename file&directory stuff
 
// Function to rename files
void rename_file(const fs::path& item_path, const std::string& case_input, bool is_directory, bool verbose_enabled, bool transform_dirs, bool transform_files, std::atomic<int>& files_count, std::atomic<int>& dirs_count, size_t batch_size_files, bool symlinks, std::atomic<int>& skipped_file_count, std::atomic<int>& skipped_folder_count, bool skipped, bool skipped_only) {
    // Check if the item is a symbolic link
    if ((fs::is_symlink(item_path) && !symlinks) || !fs::is_regular_file(item_path)) {
        ++skipped_file_count;
        if (verbose_enabled && transform_files && !symlinks && skipped) {
            print_verbose_enabled("\033[0m\033[93mSkipped\033[0m \033[95msymlink_file\033[0m " + item_path.string() + " (excluded)", std::cout);
        }
        return; // Skip processing symbolic links
    }

    // Thread-local vector for renaming data
    std::vector<std::pair<fs::path, std::string>> rename_data;
    rename_data.reserve(batch_size_files);  // Reserve space for batch size

    // Check if the item is a directory
    if (is_directory) {
        for (const auto& entry : fs::directory_iterator(item_path)) {
            rename_file(entry.path(), case_input, entry.is_directory(), verbose_enabled, transform_dirs, transform_files, files_count, dirs_count, batch_size_files, symlinks, skipped_file_count, skipped_folder_count, skipped, skipped_only);
        }
        dirs_count.fetch_add(1, std::memory_order_relaxed);
        return;
    }

    // Extract the relative path of the item from its parent directory
    fs::path relative_path = item_path.filename();
    std::string name = relative_path.string();
    std::string new_name = name;

    // Perform transformations on file names if requested
    if (transform_files) {
        static const std::unordered_set<std::string> transformations = {
            "lower", "upper", "reverse", "title", "snake", "rsnake",
            "kebab", "rkebab", "rspecial", "rnumeric", "rbra", "roperand",
            "camel", "rcamel", "sequence", "rsequence", "date", "rdate",
            "sentence", "swap", "swapr", "pascal", "rpascal"
        };

        if (transformations.count(case_input)) {
            if (case_input == "lower") {
                std::transform(new_name.begin(), new_name.end(), new_name.begin(), ::tolower);
            } else if (case_input == "upper") {
                std::transform(new_name.begin(), new_name.end(), new_name.begin(), ::toupper);
            } else if (case_input == "reverse") {
                std::transform(new_name.begin(), new_name.end(), new_name.begin(), [](unsigned char c) {
                    return std::islower(c) ? std::toupper(c) : std::tolower(c);
                });
            } else if (case_input == "title") {
                new_name = capitalizeFirstLetter(new_name);
            } else if (case_input == "snake") {
                std::replace(new_name.begin(), new_name.end(), ' ', '_');
            } else if (case_input == "rsnake") {
                std::replace(new_name.begin(), new_name.end(), '_', ' ');
            } else if (case_input == "kebab") {
                std::replace(new_name.begin(), new_name.end(), ' ', '-');
            } else if (case_input == "rkebab") {
                std::replace(new_name.begin(), new_name.end(), '-', ' ');
            } else if (case_input == "rspecial") {
                new_name.erase(std::remove_if(new_name.begin(), new_name.end(), [](char c) {
                    return !std::isalnum(c) && c != '.' && c != '_' && c != '-' && c != '(' && c != ')' && c != '[' && c != ']' && c != '{' && c != '}' && c != '+' && c != '*' && c != '<' && c != '>' && c != ' ';
                }), new_name.end());
            } else if (case_input == "rnumeric") {
                new_name.erase(std::remove_if(new_name.begin(), new_name.end(), [](char c) {
                    return std::isdigit(c);
                }), new_name.end());
            } else if (case_input == "rbra") {
                new_name.erase(std::remove_if(new_name.begin(), new_name.end(), [](char c) {
                    return c == '[' || c == ']' || c == '{' || c == '}' || c == '(' || c == ')';
                }), new_name.end());
            } else if (case_input == "roperand") {
                new_name.erase(std::remove_if(new_name.begin(), new_name.end(), [](char c) {
                    return c == '-' || c == '+' || c == '>' || c == '<' || c == '=' || c == '*';
                }), new_name.end());
            } else if (case_input == "camel") {
				bool isFile = true;
                new_name = to_camel_case(new_name, isFile);
            } else if (case_input == "rcamel") {
                new_name = from_camel_case(new_name);
            } else if (case_input == "sequence") {
                new_name = append_numbered_prefix(item_path.parent_path(), new_name);
            } else if (case_input == "rsequence") {
                new_name = remove_numbered_prefix(new_name);
            } else if (case_input == "date") {
                new_name = append_date_seq(new_name);
            } else if (case_input == "rdate") {
                new_name = remove_date_seq(new_name);
            } else if (case_input == "sentence") {
                new_name = sentenceCase(new_name);
            } else if (case_input == "swap") {
                new_name = swap_transform(new_name);
            } else if (case_input == "swapr") {
                new_name = swapr_transform(new_name);
            } else if (case_input == "pascal") {
				bool isFile = true;
                new_name = to_pascal(new_name, isFile);
            } else if (case_input == "rpascal") {
                new_name = from_pascal_case(new_name);
            }
        }
    }

    // Check if the name is unchanged, and skip if necessary
    if (name == new_name && transform_files) {
        ++skipped_file_count;
    }

    // Add data to the list if new name differs
    if (name != new_name) {
        rename_data.emplace_back(item_path, new_name);
    }

    // Check if batch size is reached and perform renaming
    if (rename_data.size() >= batch_size_files) {
        rename_batch(rename_data, verbose_enabled, files_count, dirs_count, skipped_only);
        rename_data.clear();
    } else {
        // Rename any remaining data after processing the loop
        if (!rename_data.empty()) {
            rename_batch(rename_data, verbose_enabled, files_count, dirs_count, skipped_only);
        }
        if (name == new_name && verbose_enabled && skipped && transform_files) {
            print_verbose_enabled("\033[0m\033[93mSkipped\033[0m file " + item_path.string() + " (name unchanged)", std::cout);
        }
    }
}



// Function to rename a batch of files/directories using multiple threads for parallel execution
void rename_batch(const std::vector<std::pair<fs::path, std::string>>& data, bool verbose_enabled, std::atomic<int>& files_count, std::atomic<int>& dirs_count, bool skipped_only) {
    int num_threads = std::min(static_cast<int>(data.size()), static_cast<int>(max_threads));
    #pragma omp parallel for schedule(dynamic) num_threads(num_threads)
    for (std::size_t i = 0; i < data.size(); ++i) {
        const auto& [item_path, new_name] = data[i];
        fs::path new_path = item_path.parent_path() / new_name;

        try {
            // Attempt to rename the file/directory
            fs::rename(item_path, new_path);

            if (verbose_enabled && !skipped_only) {
                // Print a success message if verbose mode enabled
                if (fs::is_symlink(item_path) || fs::is_symlink(new_path)) {
                    print_verbose_enabled("\033[0m\033[92mRenamed\033[0m \033[95msymlink_file\033[0m " + item_path.string() + "\e[1;38;5;214m -> \033[0m" + new_path.string(), std::cout);
                } else {
                    print_verbose_enabled("\033[0m\033[92mRenamed\033[0m file " + item_path.string() + "\e[1;38;5;214m -> \033[0m" + new_path.string(), std::cout);
                }
            }

            // Use fs::status to check the file status after renaming
            fs::file_status status = fs::status(new_path);

            // Update files_count or dirs_count based on the type of the renamed item
            if (fs::is_regular_file(status)) {
                // Update files_count when a file is successfully renamed
                files_count.fetch_add(1, std::memory_order_relaxed);
            } else if (fs::is_directory(status)) {
                // Update dirs_count when a directory is successfully renamed
                dirs_count.fetch_add(1, std::memory_order_relaxed);
            }
        } catch (const fs::filesystem_error& e) {
            // Print an error message if renaming fails
            if (e.code() == std::errc::permission_denied) {
                if (verbose_enabled) {
                    print_error("\033[1;91mError\033[0m: " + std::string(e.what()) + "\n", std::cerr);
                }
            }
        }
    }
}


// Helper template for batch parallel processing
template<typename Func>
void process_in_batches(const std::vector<fs::path>& items, 
                       size_t batch_size, 
                       unsigned int num_threads, 
                       Func func) {
    const size_t total_items = items.size();
    for (size_t start = 0; start < total_items; start += batch_size) {
        const size_t end = std::min(start + batch_size, total_items);
        const std::vector<fs::path> batch(items.begin() + start, items.begin() + end);
        
        #pragma omp parallel for num_threads(num_threads) schedule(static)
        for (size_t i = 0; i < batch.size(); ++i) {
            func(batch[i]);
        }
    }
}


// Function to rename a directory based on specified transformations
void rename_directory(const fs::path& directory_path, const std::string& case_input, bool rename_parents, bool verbose_enabled, bool transform_dirs, bool transform_files, std::atomic<int>& files_count, std::atomic<int>& dirs_count, std::atomic<int>& depth, size_t batch_size_files, size_t batch_size_folders, bool symlinks, std::atomic<int>& skipped_file_count, std::atomic<int>& skipped_folder_count, std::atomic<int>& skipped_folder_special_count, bool skipped, bool skipped_only, std::atomic<bool>& isFirstRun, std::atomic<bool>& special, int num_paths) {
    std::string dirname = directory_path.filename().string();
    std::string new_dirname = dirname; // Initialize with the original name
    
    // Use maximum number of available cores for batch size division
    unsigned int num_threads = max_threads;
    
    if (num_paths > 1) {
        // Calculate the maximum number of threads per path
        unsigned int max_threads_per_path = max_threads / num_paths;
    
        // Ensure at least one thread per path
        num_threads = std::max(max_threads_per_path, 1u);
    }
    
    // Early exit if the directory is a symlink and should not be transformed
    if (fs::is_symlink(directory_path) && !symlinks) {
		if (transform_dirs) {
		        skipped_folder_count.fetch_add(1, std::memory_order_relaxed);
		}
        if (verbose_enabled && skipped) {
            // Print a message if verbose mode enabled
            print_verbose_enabled("\033[0m\033[93mSkipped\033[0m processing \033[95msymlink_folder\033[0m " + directory_path.string() + " (excluded)");
        }
        return;
    }

    // Apply transformations to the directory name if required
    if (transform_dirs) {
        static const std::unordered_set<std::string> transformations = {
            "lower", "upper", "reverse", "title", "snake", "rsnake",
            "kebab", "rkebab", "rspecial", "rnumeric", "rbra", "roperand",
            "camel", "rcamel", "sequence", "rsequence", "date", "rdate",
            "sentence", "swap", "swapr", "pascal", "rpascal"
        };

        if (transformations.count(case_input)) {
            if (case_input == "lower") {
                std::transform(new_dirname.begin(), new_dirname.end(), new_dirname.begin(), ::tolower);
            } else if (case_input == "upper") {
                std::transform(new_dirname.begin(), new_dirname.end(), new_dirname.begin(), ::toupper);
            } else if (case_input == "reverse") {
                std::transform(new_dirname.begin(), new_dirname.end(), new_dirname.begin(), [](unsigned char c) {
                    return std::islower(c) ? std::toupper(c) : std::tolower(c);
                });
            } else if (case_input == "title") {
                new_dirname = capitalizeFirstLetter(new_dirname);
            } else if (case_input == "snake") {
                std::replace(new_dirname.begin(), new_dirname.end(), ' ', '_');
            } else if (case_input == "rsnake") {
                std::replace(new_dirname.begin(), new_dirname.end(), '_', ' ');
            } else if (case_input == "kebab") {
                std::replace(new_dirname.begin(), new_dirname.end(), ' ', '-');
            } else if (case_input == "rkebab") {
                std::replace(new_dirname.begin(), new_dirname.end(), '-', ' ');
            } else if (case_input == "rspecial") {
                new_dirname.erase(std::remove_if(new_dirname.begin(), new_dirname.end(), [](char c) {
                    return !std::isalnum(c) && c != '.' && c != '_' && c != '-' && c != '(' && c != ')' && c != '[' && c != ']' && c != '{' && c != '}' && c != '+' && c != '*' && c != '<' && c != '>' && c != ' ';
                }), new_dirname.end());
            } else if (case_input == "rnumeric") {
                new_dirname.erase(std::remove_if(new_dirname.begin(), new_dirname.end(), [](char c) {
                    return std::isdigit(c);
                }), new_dirname.end());
            } else if (case_input == "rbra") {
                new_dirname.erase(std::remove_if(new_dirname.begin(), new_dirname.end(), [](char c) {
                    return c == '[' || c == ']' || c == '{' || c == '}' || c == '(' || c == ')';
                }), new_dirname.end());
            } else if (case_input == "roperand") {
                new_dirname.erase(std::remove_if(new_dirname.begin(), new_dirname.end(), [](char c) {
                    return c == '-' || c == '+' || c == '>' || c == '<' || c == '=' || c == '*';
                }), new_dirname.end());
            } else if (case_input == "camel") {
				bool isFile = false;
                new_dirname = to_camel_case(new_dirname, isFile);
            } else if (case_input == "rcamel") {
                new_dirname = from_camel_case(new_dirname);
            } else if (case_input == "sequence") {
                special = true;
                rename_folders_with_sequential_numbering(directory_path, "", dirs_count, skipped_folder_special_count, depth, verbose_enabled, skipped, skipped_only, symlinks, batch_size_folders, num_paths);
            } else if (case_input == "rsequence") {
                new_dirname = get_renamed_folder_name_without_numbering(new_dirname);
            } else if (case_input == "date") {
                new_dirname = append_date_suffix_to_folder_name(new_dirname);
            } else if (case_input == "rdate") {
                new_dirname = get_renamed_folder_name_without_date(new_dirname);
            } else if (case_input == "sentence") {
                new_dirname = sentenceCase(new_dirname);
            }  else if (case_input == "swap") {
                new_dirname = swap_transform(new_dirname);
            } else if (case_input == "swapr") {
                new_dirname = swapr_transform(new_dirname);
            } else if (case_input == "pascal") {
				bool isFile = false;
                new_dirname = to_pascal(new_dirname, isFile);
            } else if (case_input == "rpascal") {
                new_dirname = from_pascal_case(new_dirname);
            }
        }
    }

    fs::path new_path = directory_path.parent_path() / new_dirname;

    if (directory_path == new_path && transform_dirs && !special) {
            skipped_folder_count.fetch_add(1, std::memory_order_relaxed);
    }

    // Check if renaming is necessary
    if (directory_path != new_path) {
        try {
            fs::rename(directory_path, new_path);
            if (verbose_enabled && !skipped_only) {
                if ((std::filesystem::is_symlink(directory_path) || (std::filesystem::is_symlink(new_path) && symlinks)) && !special) {
                    print_verbose_enabled("\033[0m\033[92mRenamed \033[95msymlink_folder\033[0m " + directory_path.string() + "\e[1;38;5;214m -> \033[0m" + new_path.string(), std::cout);
                } else {
                    print_verbose_enabled("\033[0m\033[92mRenamed \033[94mfolder\033[0m " + directory_path.string() + "\e[1;38;5;214m -> \033[0m" + new_path.string(), std::cout);
                }
            }
            dirs_count.fetch_add(1, std::memory_order_relaxed);
        } catch (const fs::filesystem_error& e) {
            if (e.code() == std::errc::permission_denied) {
                if (verbose_enabled) {
                    print_error("\033[1;91mError\033[0m: Permission denied: " + directory_path.string(), std::cout);
                }
            }
            return;
        }
    } else {
        if (verbose_enabled && (std::filesystem::is_symlink(directory_path) || std::filesystem::is_symlink(new_path)) && !transform_files && skipped && !special) {
            print_verbose_enabled("\033[0m\033[93mSkipped\033[0m\033[95m symlink_folder\033[0m " + directory_path.string() + " (name unchanged)", std::cout);
        } else if (verbose_enabled && (std::filesystem::is_symlink(directory_path) || std::filesystem::is_symlink(new_path)) && transform_dirs && transform_files && !special && skipped) {
            print_verbose_enabled("\033[0m\033[93mSkipped\033[0m\033[95m symlink_folder\033[0m " + directory_path.string() + " (name unchanged)", std::cout);
        }
        if (!rename_parents && isFirstRun) {
        } else {
            if (verbose_enabled && !transform_files && skipped && !special) {
                print_verbose_enabled("\033[0m\033[93mSkipped\033[0m\033[94m folder\033[0m " + directory_path.string() + " (name unchanged)", std::cout);
            } else if (verbose_enabled && transform_dirs && transform_files && !special && skipped) {
                print_verbose_enabled("\033[0m\033[93mSkipped\033[0m\033[94m folder\033[0m " + directory_path.string() + " (name unchanged)", std::cout);
            }
        }
    }

    if (isFirstRun) {
        isFirstRun = false;
    }
    
    
    if (num_paths > 1) {
        num_threads = std::max(omp_get_max_threads() / num_paths, 1);
    }

    // Process directory contents
    if (depth != 0) {
    if (depth > 0) --depth;

    std::vector<fs::path> dir_batch;
    std::vector<fs::path> file_batch;
    dir_batch.reserve(batch_size_folders);
    file_batch.reserve(batch_size_files);

    // Separate directory and file processing
    for (const auto& entry : fs::directory_iterator(new_path)) {
        if (entry.is_directory()) {
            if (rename_parents) {
                rename_directory(entry.path(), case_input, false, verbose_enabled,
                               transform_dirs, transform_files, files_count, dirs_count,
                               depth, batch_size_files, batch_size_folders, symlinks,
                               skipped_file_count, skipped_folder_count,
                               skipped_folder_special_count, skipped, skipped_only,
                               isFirstRun, special, num_paths);
            } else {
                dir_batch.emplace_back(entry.path());
            }
        } else {
            file_batch.emplace_back(entry.path());
        }
    }

	// Parallel directory processing
        process_in_batches(dir_batch, batch_size_folders, num_threads,
            [&](const fs::path& dir) {
                rename_directory(dir, case_input, false, verbose_enabled,
                               transform_dirs, transform_files, files_count, dirs_count,
                               depth, batch_size_files, batch_size_folders, symlinks,
                               skipped_file_count, skipped_folder_count,
                               skipped_folder_special_count, skipped, skipped_only,
                               isFirstRun, special, num_paths);
            });
    

    // Parallel file processing
    process_in_batches(file_batch, batch_size_files, num_threads,
        [&](const fs::path& file) {
            rename_file(file, case_input, false, verbose_enabled,
                      transform_dirs, transform_files, files_count, dirs_count,
                      batch_size_files, symlinks, skipped_file_count,
                      skipped_folder_count, skipped, skipped_only);
        });
	}
}
 

// Function to rename paths (directories and files) based on specified transformations asynchronously
void rename_path(const std::vector<std::string>& paths, const std::string& case_input, bool rename_parents, bool verbose_enabled, bool transform_dirs, bool transform_files, std::atomic<int>& depth, std::atomic<int>& files_count, std::atomic<int>& dirs_count, size_t batch_size_files, size_t batch_size_folders, bool symlinks, std::atomic<int>& skipped_file_count, std::atomic<int>& skipped_folder_count, std::atomic<int>& skipped_folder_special_count, bool skipped, bool skipped_only, std::atomic<bool>& isFirstRun, bool non_interactive, std::atomic<bool>& special) {
    auto start_time = std::chrono::steady_clock::now(); // Start time measurement
    // Number of paths to be processed based on std::vector<std::string> paths
    int num_paths = paths.size();
    omp_set_num_threads(num_paths);
    #pragma omp parallel for default(none) shared(paths, case_input, rename_parents, verbose_enabled, transform_dirs, transform_files, depth, files_count, dirs_count, batch_size_files, batch_size_folders, symlinks, skipped_file_count, skipped_folder_count, skipped_folder_special_count, skipped, skipped_only, isFirstRun, special, num_paths)
    for (int i = 0; i < num_paths; ++i) {
		isFirstRun = true;
        
        // Obtain the current path
        fs::path current_path(paths[i]);
        if (fs::exists(current_path)) {
            if (fs::is_directory(current_path)) {
                if (rename_parents) {
                    // If -p option is used, only rename the immediate parent
                    fs::path immediate_parent_path = current_path.parent_path();
                    rename_directory(immediate_parent_path, case_input, rename_parents, verbose_enabled, transform_dirs, transform_files, files_count, dirs_count, depth, batch_size_files, batch_size_folders, symlinks, skipped_file_count, skipped_folder_count, skipped_folder_special_count, skipped, skipped_only, isFirstRun, special, num_paths);
                } else {
                    // Otherwise, rename the entire path
                    rename_directory(current_path, case_input, rename_parents, verbose_enabled, transform_dirs, transform_files, files_count, dirs_count, depth, batch_size_files, batch_size_folders, symlinks, skipped_file_count, skipped_folder_count, skipped_folder_special_count, skipped, skipped_only, isFirstRun, special, num_paths);
                }
            } else if (fs::is_regular_file(current_path)) {
                // For files, directly rename the item without considering the parent directory
                rename_file(current_path, case_input, false, verbose_enabled, transform_dirs, transform_files, files_count, dirs_count, batch_size_files, symlinks, skipped_file_count, skipped_folder_count, skipped, skipped_only);
            }
        }
    }

    auto end_time = std::chrono::steady_clock::now(); // End time measurement

    std::chrono::duration<double> elapsed_seconds = end_time - start_time; // Calculate elapsed time
    if (verbose_enabled) {
        std::cout << "\n";
    }
    if (!non_interactive || verbose_enabled) {
        // Output summary of the renaming process
        std::cout << "\n\033[0;1mRenamed: \033[1;92m" << files_count << " file(s) \033[0;1m&& \033[1;94m"
                  << dirs_count << " folder(s) \033[1m\033[0;1m| Skipped: \033[1;93m" << skipped_file_count << " file(s) \033[0;1m&& \033[1;93m";

        if (special) {
            std::cout << skipped_folder_special_count << " folder(s) ";
        } else {
            std::cout << skipped_folder_count << " folder(s) ";
        }

        std::cout << "\033[0m\033[0;1m| From: \033[1;95m" << paths.size() << " input path(s)"
                  << "\n\n\033[0;1mTime Elapsed: " << std::setprecision(1)
                  << std::fixed << elapsed_seconds.count() << "\033[1m second(s)\n\n";
    }
}


// Function to provide examples on user query
std::string example_transform(const std::string& mode, std::string word, bool ce_flag) {
    std::string transformed_word;  // To hold the transformed word

     // Decide the initial word based on the ce_flag and modes
    if (ce_flag && !(mode == "rbak")) {
        word = "Test.txt";
    } else if (ce_flag && mode == "rbak") {
        word = "Test.txt.bak";
    } 
    else if (!ce_flag && mode == "title") {
        word = "test.txt";
    } 
    else if (!ce_flag && mode == "rsequence") {
        word = "001_Test";
    } else if (!ce_flag && mode == "rkebab") {
        word = "Te-st";
    } else if (!ce_flag && (mode == "kebab" || mode == "snake")) {
        word = "Te st";
    } else if (!ce_flag && (mode == "pascal" || mode == "camel")) {
        word = "Te st";
    } else if (!ce_flag && mode == "rsnake") {
        word = "Te_st";
    } else if (!ce_flag && (mode == "rcamel" || mode == "rpascal")) {
        word = "TeSt";
    } else if (!ce_flag && mode == "sentence") {
        word = "Te st";
    }
    else if (!ce_flag && mode == "rdate") {
        word = "Test_20240215";
    }
    else if (!ce_flag && mode == "rspecial") {
        word = "T'es!@#$%^|&~t;";
    }
    else if (!ce_flag && mode == "roperand") {
        word = "=T-e+s<t>";
    }
    else if (!ce_flag && mode == "rnumeric") {
        word = "1Te0st2";
    }
    else if (!ce_flag && mode == "rbra") {
        word = "[{Test}]";
    }
    else {
        word = "Test";
    }
    

    // Display transformation based on the mode
    if (mode == "lower") {
        transformed_word = ce_flag ? "test.txt" : "test";  // Adjust for ce_flag
    } else if (mode == "upper") {
        transformed_word = ce_flag ? "TEST.TXT" : "TEST";  // Uppercase transformation
    } else if (mode == "reverse") {
        transformed_word = ce_flag ? "tSET.TXT" : "tSET";  // Reverse case
    } else if (mode == "title") {
        transformed_word = ce_flag ? "Test.Txt" : "Test";  // Title Case
    } else if (mode == "swap") {
        transformed_word = ce_flag ? "tEST.TXT" : "tEST";  // Swap case
    } else if (mode == "swapr") {
        transformed_word = ce_flag ? "TesT.Txt" : "tEsT";  // Swap reverse case
    } else if (mode == "pascal") {
        transformed_word = ce_flag ? "TeSt.Txt" : "TeSt";  // Pascal case
    } else if (mode == "camel") {
        transformed_word = ce_flag ? "teSt.txt" : "teSt";  // Camel case
    } else if (mode == "kebab") {
        transformed_word = ce_flag ? "Te-st.txt" : "Te-st";  // Kebab case
    } else if (mode == "snake") {
        transformed_word = ce_flag ? "Te_st.txt" : "Te_st";  // Snake case
    } else if (mode == "rpascal") {
        transformed_word = ce_flag ? "Te St.txt" : "Te St";  // Reverse Pascal case
    } else if (mode == "rcamel") {
        transformed_word = ce_flag ? "te st.txt" : "te st";  // Reverse Camel case
    } else if (mode == "rsnake") {
        transformed_word = ce_flag ? "Te st.txt" : "Te st";  // Reverse Snake case
    } else if (mode == "rkebab") {
        transformed_word = ce_flag ? "Te st.txt" : "Te st";  // Reverse Kebab case
    } else if (mode == "date") {
        transformed_word = ce_flag ? "Test_20240215.txt" : "Test_20240215";  // Append date
    } else if (mode == "sequence") {
        transformed_word = ce_flag ? "001_Test.txt" : "001_Test";  // Append numeric sequence
    } else if (mode == "rsequence") {
        transformed_word = ce_flag ? "Test.txt" : "Test";  // Remove numeric sequence
    } else if (mode == "rdate") {
        transformed_word = ce_flag ? "Test.txt" : "Test";  // Remove date
    } else if (mode == "rbra") {
        transformed_word = ce_flag ? "Test.txt" : "Test"; // Remove brackets around names
    } else if (mode == "rspecial") {
        transformed_word = ce_flag ? "Test.txt" : "Test"; // Remove special characters from names
    } else if (mode == "roperand") {
        transformed_word = ce_flag ? "Test.txt" : "Test"; // Remove arithmetic operands from names
    } else if (mode == "rnumeric") {
        transformed_word = ce_flag ? "Test.txt" : "Test"; // Remove numeric characters from names
    }
	  else if (mode == "bak") {
        transformed_word = ce_flag ? "Test.txt.bak" : "Test.bak";  // Add .bak to file extension
    } else if (mode == "rbak") {
        transformed_word = ce_flag ? "Test.txt" : "Test";  // Remove .bak from file extension
    } else if (mode == "noext") {
        transformed_word = "Test";  // Remove file extension, no need to check ce_flag
    } else {
        transformed_word = word; // Default to original word if no mode matches
    }

    // Return formatted string
    return std::string(mode + "_Case \033[0;1m(e.g., \033[38;5;130m" + word + " => " + transformed_word + "\033[0;1m)\033[0m");
}


// Main function
int main(int argc, char *argv[]) {
    // Initialize variables and flags
    std::vector<std::string> paths;
    std::string case_input;
    bool rename_parents = false;
    bool rename_extensions = false;
    bool verbose_enabled = false;
    std::atomic<int> skipped_folder_special_count{0};
    std::atomic<int> skipped_folder_count{0};
    std::atomic<int> skipped_file_count{0};
    std::atomic<int> files_count {0};
    std::atomic<int> dirs_count{0};
    std::atomic<int> depth{-1};
    bool case_specified = false;
    bool transform_dirs = true;
    bool transform_files = true;
    bool skipped = false;
    bool skipped_only = false;
    bool symlinks = false;
    std::atomic<bool> isFirstRun{true};
    std::atomic<bool> special{false};
    bool non_interactive = false;
    constexpr int batch_size_files = 1000;
    constexpr int batch_size_folders = 100;

    // Define constants for flag strings
    const std::unordered_set<std::string> valid_flags = {
        "-fi", "-sym", "-fo", "-d", "-v", "--verbose", "-vs", "-vso", "-ni", "-h", "--help", "-c", "-cp", "-ce"
    };

    // Handle command-line arguments
    // Display help message if no arguments provided
    if (argc == 1) {
        print_help();
        return 0;
    }

    // Check if --version flag is present
    if (argc > 1 && std::string(argv[1]) == "--version") {
        // Print version number and exit
        printVersionNumber("2.0.8");
        return 0;
    }

    // Flags to handle command-line options
    bool fi_flag = false;
    bool fo_flag = false;
    bool c_flag = false;
    bool cp_flag = false;
    bool ce_flag = false;
    bool ni_flag = false;
    bool v_flag = false;
    bool vs_flag = false;
    bool vso_flag = false;

    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        if (valid_flags.count(arg)) {
            if (arg == "-fi") {
                transform_dirs = false;
                fi_flag = true;
            } else if (arg == "-sym") {
                symlinks = true;
            } else if (arg == "-fo") {
                transform_files = false;
                fo_flag = true;
            } else if (arg == "-d" && i + 1 < argc) {
                // Check if the depth value is empty or not a number
                if (argv[i + 1] == nullptr || std::string(argv[i + 1]).empty() || !isdigit(argv[i + 1][0])) {
                    print_error("\033[1;91mError: Depth value if set must be a non-negative integer.\033[0m\n\n");
                    return 1;
                }
                depth = std::atoi(argv[++i]);
                if (depth < -1) {
                    print_error("\033[1;91mError: Depth value if set must be -1 or greater.\033[0m\n\n");
                    return 1;
                }
            } else if (arg == "-v" || arg == "--verbose") {
                v_flag = true;
                verbose_enabled = true;
            } else if (arg == "-vs") {
                vs_flag = true;
                verbose_enabled = true;
                skipped = true;
            } else if (arg == "-vso") {
                vso_flag = true;
                verbose_enabled = true;
                skipped = true;
                skipped_only = true;
            } else if (arg == "-ni") {
                non_interactive = true;
                ni_flag = true;
            } else if (arg == "-h" || arg == "--help") {
                clearScrollBuffer();
                print_help();
                return 0;
            } else if (arg == "-c") {
                // Check if -c, -cp, -ce options are mixed
                if (c_flag || cp_flag || ce_flag) {
                    print_error("\033[1;91mError: Cannot mix -c, -cp, and -ce options.\033[0m\n\n");
                    return 1;
                }
                c_flag = true;
                if (i + 1 < argc) {
                    case_input = argv[++i];
                    case_specified = true;
                } else {
                    print_error("\033[1;91mError: Missing argument for option " + arg + "\033[0m\n\n");
                    return 1;
                }
            } else if (arg == "-cp") {
                // Check if -c, -cp, -ce options are mixed
                if (c_flag || cp_flag || ce_flag) {
                    print_error("\033[1;91mError: Cannot mix -c, -cp, and -ce options.\033[0m\n\n");
                    return 1;
                }
                cp_flag = true;
                rename_parents = true;
                if (i + 1 < argc) {
                    case_input = argv[++i];
                    case_specified = true;
                } else {
                    print_error("\033[1;91mError: Missing argument for option " + arg + "\033[0m\n\n");
                    return 1;
                }
            } else if (arg == "-ce") {
                // Check if -c, -cp, -ce options are mixed
                if (c_flag || cp_flag || ce_flag) {
                    print_error("\033[1;91mError: Cannot mix -c, -cp, and -ce options.\033[0m\n\n");
                    return 1;
                } else if (arg == "-ce") {
                    // Check if -c, -cp, -ce options are mixed
                    if (fi_flag || fo_flag || ce_flag) {
                        print_error("\033[1;91mError: Cannot mix -fi or -fo with -ce option.\033[0m\n\n");
                        return 1;
                    }
                }

                ce_flag = true;
                rename_extensions = true;
                if (i + 1 < argc) {
                    case_input = argv[++i];
                    case_specified = true;
                } else {
                    print_error("\033[1;91mError: Missing argument for option " + arg + "\033[0m\n\n");
                    return 1;
                }
            }
        } else {
            // Check for duplicate paths
            if (std::find(paths.begin(), paths.end(), arg) != paths.end()) {
                print_error("\033[1;91mError: Duplicate path detected - " + arg + "\033[0m\n\n");
                return 1;
            }
            paths.emplace_back(arg);
        }
    }
    std::string word;
    std::string result = example_transform(case_input, word, ce_flag);

    // Perform renaming based on flags and options
    if (fi_flag && fo_flag) {
        print_error("\033[1;91mError: Cannot mix -fi and -fo options.\033[0m\n\n");
        return 1;
    }

    if ((v_flag && (vs_flag || vso_flag)) || (vs_flag && (v_flag || vso_flag)) || (vso_flag && (v_flag || vs_flag))) {
        print_error("\033[1;91mError: Cannot mix -v, -vs, and -vso options.\033[0m\n\n");
        return 1;
    }

    if (!case_specified) {
        print_error("\033[1;91mError: Case conversion mode not specified (-c, -cp, or -ce option is required)\033[0m\n\n");
        return 1;
    }

    // Check for valid case modes
    std::vector<std::string> valid_modes;
    if (cp_flag || c_flag) { // Valid modes for -cp and -ce
        valid_modes = {"lower", "upper", "reverse", "title", "date", "swap", "swapr", "rdate", "pascal", "rpascal", "camel", "sentence", "rcamel", "kebab", "rkebab", "rsnake", "snake", "rnumeric", "rspecial", "rbra", "roperand", "sequence", "rsequence"};
    } else { // Valid modes for -c
        valid_modes = {"lower", "upper", "reverse", "title", "swap", "swapr", "rbak", "bak", "noext"};
    }

    if (std::find(valid_modes.begin(), valid_modes.end(), case_input) == valid_modes.end()) {
        print_error("\033[1;91mError: Unspecified or invalid case mode - " + case_input + ". Run 'bulk_rename++ --help'.\033[0m\n\n");
        return 1;
    }

    if (cp_flag && (std::find(valid_modes.begin(), valid_modes.end(), case_input) != valid_modes.end())) {
        if (case_input == "sequence") {
            print_error("\033[1;91mError: sequence mode is only available with -c option.\033[0m\n\n");
            return 1;
        }
    }

    // Check if paths exist
    for (const auto& path : paths) {
        if (!fs::exists(path)) {
            print_error("\033[1;91mError: Path does not exist or not a directory - " + path + "\033[0m\n\n");
            return 1;
        }
    }

    // Check if paths end with '/'
    for (const std::string& path : paths) {
        if (path.back() != '/') {
            print_error("\033[1;91mError: Path(s) must end with '/' - \033[0;1me.g. \033[1;91m" + path + " \033[0;1m-> \033[1;94m" + path +"/\033[0m" "\n\033[0m\n");
            return 0;
        }
    }
    if (!ni_flag || verbose_enabled) {
        clearScrollBuffer();
    }

    // Prompt the user for confirmation before proceeding
    std::string confirmation;
    if (rename_parents && !ni_flag) {
        // Display the paths and their lowest parent directories that will be renamed
		std::cout << "\033[0;1mThe following path(s) and their \033[4mlowest Parent\033[0;1m dir(s), will be recursively renamed to \033[0m\e[1;38;5;214m"
         << result;
        if (depth != -1) {
            std::cout << "\033[0;1m (up to depth " << depth << ")";
        }
        if (!transform_dirs) {
            std::cout << "\033[0;1m (excluding directories)";
        }
        if (!transform_files) {
            std::cout << "\033[0;1m (excluding files)";
        }
        std::cout << ":\033[1m\n";
        for (const auto& path : paths) {
            std::cout << "\n" << "\033[1;94m" << path << "\033[0m";
        }
    } else if (rename_extensions && !ni_flag) {
        // Display the paths where file extensions will be recursively renamed
        std::cout << "\033[0;1mThe file \033[4mextensions\033[0;1m under the following path(s) \033[1mwill be recursively renamed to \033[0m\e[1;38;5;214m"
         << result;
        if (depth != -1) {
            std::cout << "\033[0;1m (up to depth " << depth << ")";
        }
        std::cout << ":\033[1m\n";
        for (const auto& path : paths) {
            std::cout << "\n" << "\033[1;94m" << path << "\033[0m";
        }
    } else if (!ni_flag) {
        // Display the paths that will be recursively renamed
        std::cout << "\033[0;1mThe following path(s) will be recursively renamed to \033[0m\e[1;38;5;214m"
         << result;
        if (depth != -1) {
            std::cout << "\033[0;1m (up to depth " << depth << ")";
        }
        if (!transform_dirs && rename_parents) {
            std::cout << "\033[0;1m (excluding both files and directories)";
        } else if (!transform_dirs) {
            std::cout << "\033[0;1m (excluding directories)";
        } else if (!transform_files) {
            std::cout << "\033[0;1m (excluding files)";
        }
        std::cout << ":\033[1m\n";
        for (const auto& path : paths) {
            std::cout << "\n" << "\033[1;94m" << path << "\033[0m";
        }
    }

    // Prompt the user for confirmation
    if (!ni_flag) {
        std::cout << "\n\n\033[1mDo you want to proceed? (y/n): ";
        std::getline(std::cin, confirmation);

    }
    if (!ni_flag) {
        // If the user does not confirm, abort the operation
        if (confirmation != "y") {
            std::cout << "\n\033[1;93mRename operation aborted by user.\033[0m";
            std::cout << "\n";
            std::cout << "\n\033[1mPress enter to exit...\033[0m";
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            clearScrollBuffer();
            return 0;
        }
    }
    
	if (!ni_flag) disableInput();
	
    // Perform the renaming operation based on the selected mode
    if (rename_parents) {
        rename_path(paths, case_input, true, verbose_enabled, transform_dirs, transform_files, depth, files_count, dirs_count, batch_size_files, batch_size_folders, symlinks, skipped_file_count, skipped_folder_count, skipped_folder_special_count, skipped, skipped_only, isFirstRun, non_interactive, special); // Pass true for rename_parents
    } else if (rename_extensions) {
        rename_extension_path(paths, case_input, verbose_enabled, depth, files_count, batch_size_files, symlinks, skipped_file_count, skipped, skipped_only, non_interactive);
    } else if (!transform_dirs) {
        rename_path(paths, case_input, rename_parents, verbose_enabled, transform_dirs, transform_files, depth, files_count, dirs_count, batch_size_files, batch_size_folders, symlinks, skipped_file_count, skipped_folder_count, skipped_folder_special_count, skipped, skipped_only, isFirstRun, non_interactive, special);
    } else {
		std::atomic<int> temp_skipped_folder_count = skipped_folder_count - paths.size();
        rename_path(paths, case_input, rename_parents, verbose_enabled, transform_dirs, transform_files, depth, files_count, dirs_count, batch_size_files, batch_size_folders, symlinks, skipped_file_count, temp_skipped_folder_count, skipped_folder_special_count, skipped, skipped_only, isFirstRun, non_interactive, special);
    }
    
	if (!ni_flag) {
        flushStdin();
		restoreInput();
        // Prompt the user to press enter to exit
        std::cout << "\033[1mPress enter to exit...\033[0m";
        
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        
        
        clearScrollBuffer();
        
    }
    
    return 0;
}
