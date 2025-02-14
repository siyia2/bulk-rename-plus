// SPDX-License-Identifier: GNU General Public License v3.0 or later

#include "headers.h"


// Separate string operations

// for Files&Dirs

// Function to reaname to sentenceCase
std::string sentenceCase(const std::string& string) {
    if (string.empty()) return string; // Handling empty string case
    
    std::stringstream result;
    bool newWord = true;
    
    for (char c : string) {
        if (newWord && std::isalpha(c)) {
            result << static_cast<char>(std::toupper(c));
            newWord = false;
        } else {
            result << static_cast<char>(std::tolower(c));
        }
        if (std::isspace(c) || c == '.') { // Consider a new word after a space or period
            newWord = true;
        }
    }
    
    return result.str();
}


// Function to reaname to titlerCase
std::string capitalizeFirstLetter(const std::string& input) {
    if (input.empty()) return input; // Handling empty string case
    
    std::stringstream result;
    bool first = true;
    
    for (char c : input) {
        if (first && std::isalpha(c)) {
            result << static_cast<char>(std::toupper(c));
            first = false;
        } else {
            result << static_cast<char>(std::tolower(c)); // Convert to lowercase
        }
    }
    
    return result.str();
}


// Function to reaname to swapCase
std::string swap_transform(const std::string& string) {
    std::stringstream transformed;
    bool capitalize = false; // Toggle state for characters after the first in folder names
    bool inFolderName = true; // Track if we're at the start of a folder component
    const size_t folderDelimiter = string.find_last_of("/\\");
    const size_t length = string.length();

    for (size_t i = 0; i < length; ++i) {
        const char c = string[i];
        
        // Apply transformation only to the path part before the last folder delimiter
        if (i < folderDelimiter || folderDelimiter == std::string::npos) {
            if (inFolderName) {
                // Capitalize first character of folder component
                transformed << static_cast<char>(std::toupper(c));
                inFolderName = false;
            } else {
                if (std::isalpha(c)) {
                    // Alternate case for subsequent characters
                    if (capitalize) {
                        transformed << static_cast<char>(std::toupper(c));
                    } else {
                        transformed << static_cast<char>(std::tolower(c));
                    }
                    capitalize = !capitalize;
                } else {
                    transformed << c;
                    // Reset state for new folder component
                    if (c == '/' || c == '\\') {
                        inFolderName = true;
                        capitalize = false; // Reset toggle for next folder
                    }
                }
            }
        } else {
            // Leave filename part unchanged
            transformed << c;
        }
    }

    return transformed.str();
}


// Function to reaname to swaprCase
std::string swapr_transform(const std::string& string) {
    std::stringstream transformed;
    bool capitalize = false; // Toggle state for characters after the first in folder names
    bool inFolderName = true; // Track if we're at the start of a folder component
    const size_t folderDelimiter = string.find_last_of("/\\");
    const size_t length = string.length();

    for (size_t i = 0; i < length; ++i) {
        const char c = string[i];
        
        // Apply transformation only to the path part before the last folder delimiter
        if (i < folderDelimiter || folderDelimiter == std::string::npos) {
            if (inFolderName) {
                // Lowercase first character of folder component
                transformed << static_cast<char>(std::tolower(c));
                inFolderName = false;
            } else {
                if (std::isalpha(c)) {
                    // Alternate case for subsequent characters
                    if (capitalize) {
                        transformed << static_cast<char>(std::tolower(c));
                    } else {
                        transformed << static_cast<char>(std::toupper(c));
                    }
                    capitalize = !capitalize;
                } else {
                    transformed << c;
                    // Reset state for new folder component
                    if (c == '/' || c == '\\') {
                        inFolderName = true;
                        capitalize = false; // Reset toggle for next folder
                    }
                }
            }
        } else {
            // Leave filename part unchanged
            transformed << c;
        }
    }

    return transformed.str();
}


// Converts to camelCase (first letter lowercase, rest words capitalized)
std::string to_camel_case(const std::string& string, bool isFile) {
    std::string base = string;
    std::string extension;
    
    // Split filename into base and extension at last dot
    if (isFile) {
        size_t lastDot = string.find_last_of('.');
        if (lastDot != std::string::npos) {
            base = string.substr(0, lastDot);
            extension = string.substr(lastDot);
        }
    }

    std::string result;
    bool capitalizeNext = false;
    bool firstChar = true;

    for (char c : base) {
        if (std::isalpha(c)) {
            if (firstChar) {
                result += tolower(c); // Force first character lowercase
                firstChar = false;
            } else {
                result += capitalizeNext ? toupper(c) : tolower(c);
            }
            capitalizeNext = false;
        } else if (c == ' ') {
            capitalizeNext = true;
        } else {
            result += c;
            capitalizeNext = !std::isalnum(c); // Reset on non-alphanumeric
        }
    }

    return result + extension;
}

// Converts to PascalCase (all words capitalized)
std::string to_pascal(const std::string& string, bool isFile) {
    std::string base = string;
    std::string extension;
    
    // Split filename into base and extension at last dot
    if (isFile) {
        size_t lastDot = string.find_last_of('.');
        if (lastDot != std::string::npos) {
            base = string.substr(0, lastDot);
            extension = string.substr(lastDot);
        }
    }

    std::string result;
    bool capitalizeNext = true;

    for (char c : base) {
        if (std::isalpha(c)) {
            result += capitalizeNext ? toupper(c) : tolower(c);
            capitalizeNext = false;
        } else if (c == ' ') {
            capitalizeNext = true;
        } else {
            result += c;
            capitalizeNext = !std::isalnum(c); // Reset on non-alphanumeric
        }
    }

    return result + extension;
}

// Reverses camelCase or PascalCase to space-separated
std::string from_camel_case(const std::string& string) {
    std::string result;
    for (size_t i = 0; i < string.size(); ++i) {
        char c = string[i];
        if (isupper(c) && i != 0) {
            result += ' ';
        }
        result += tolower(c);
    }
    return result;
}

// Reverses PascalCase to space-separated (special handling for initial caps)
std::string from_pascal_case(const std::string& string) {
    std::string result;
    for (size_t i = 0; i < string.size(); ++i) {
        char c = string[i];
        if (isupper(c) && i != 0) {
            result += ' ';
        }
        result += tolower(c);
    }
    return result;
}


// Function to remove sequential numbering from folder name
std::string get_renamed_folder_name_without_numbering(const fs::path& folder_path) {
    std::string folder_name = folder_path.filename().string();

    // Find the position of the first non-zero digit
    size_t first_non_zero = folder_name.find_first_not_of('0');

    // Check if the folder name starts with a sequence of digits followed by an underscore
    size_t underscore_pos = folder_name.find('_', first_non_zero);
    if (underscore_pos != std::string::npos && folder_name.find_first_not_of("0123456789", first_non_zero) == underscore_pos) {
        // Extract the original name without the numbering
        std::string original_name = folder_name.substr(underscore_pos + 1);
        return original_name;
    }

    // No sequential numbering found, return original name
    return folder_name;
}


// Function to remove date suffix from folder name
std::string get_renamed_folder_name_without_date(const fs::path& folder_path) {
    std::string folder_name = folder_path.filename().string();

    // Check if the folder name ends with the date suffix format "_YYYYMMDD"
    if (folder_name.size() < 9 || folder_name.substr(folder_name.size() - 9, 1) != "_")
        return folder_name; // No date suffix found, return original name

    bool is_date_suffix = true;
    for (size_t i = folder_name.size() - 8; i < folder_name.size(); ++i) {
        if (!std::isdigit(folder_name[i])) {
            is_date_suffix = false;
            break;
        }
    }

    if (!is_date_suffix)
        return folder_name; // Not a valid date suffix, return original name

    // Remove the date suffix from the folder name
    std::string new_folder_name = folder_name.substr(0, folder_name.size() - 9);

    return new_folder_name;
}

// Function to append date suffix to folder name
std::string append_date_suffix_to_folder_name(const fs::path& folder_path) {
    std::string folder_name = folder_path.filename().string();

    // If the folder name is empty, return an empty string
    if (folder_name.empty())
        return "";

    // Check if the folder name already ends with a date suffix
    if (folder_name.size() >= 9 && folder_name.substr(folder_name.size() - 9, 1) == "_" &&
        folder_name.substr(folder_name.size() - 8, 8).find_first_not_of("0123456789") == std::string::npos)
        return folder_name; // If it does, do not rename

    // Find the last underscore character in the folder name, excluding numeric prefix
    size_t last_underscore_pos = folder_name.find_last_of('_', folder_name.find_first_not_of("0123456789"));

    // Check if there is an underscore and if the substring after it matches the date format
    if (last_underscore_pos != std::string::npos &&
        folder_name.size() - last_underscore_pos == 9 &&
        folder_name.substr(last_underscore_pos + 1, 8).find_first_not_of("0123456789") == std::string::npos)
        return folder_name; // If it does, do not rename

    // Get the current date in YYYYMMDD format
    auto now = std::chrono::system_clock::now();
    auto now_time_t = std::chrono::system_clock::to_time_t(now);
    std::tm local_tm = *std::localtime(&now_time_t);
    std::ostringstream oss;
    oss << std::put_time(&local_tm, "%Y%m%d");
    std::string date_suffix = "_" + oss.str();

    // Append the date suffix to the folder name
    std::string new_folder_name = folder_name + date_suffix;

    return new_folder_name;
}


// For Files

// Function to add sequencial numbering to files
std::string append_numbered_prefix(const std::filesystem::path& parent_path, const std::string& file_string) {
    static std::unordered_map<std::filesystem::path, std::vector<std::string>> file_map;
    
    // Clear the file map for this parent path
    file_map[parent_path].clear();
    
    // Function to remove existing numeric prefix
    auto remove_prefix = [](const std::string& filename) {
        size_t pos = filename.find('_');
        if (pos != std::string::npos && pos > 0 && std::all_of(filename.begin(), filename.begin() + pos, ::isdigit)) {
            return filename.substr(pos + 1);
        }
        return filename;
    };

    // Collect all files in the directory
    for (const auto& entry : std::filesystem::directory_iterator(parent_path)) {
        if (entry.is_regular_file()) {
            file_map[parent_path].push_back(remove_prefix(entry.path().filename().string()));
        }
    }
    
    auto& files = file_map[parent_path];
    
    // Add the new file to the list if it doesn't exist
    std::string file_without_prefix = remove_prefix(file_string);
    if (std::find(files.begin(), files.end(), file_without_prefix) == files.end()) {
        files.push_back(file_without_prefix);
    }
    
    // Sort files alphabetically by their names
    std::sort(files.begin(), files.end());
    
    // Find the position of the current file
    auto it = std::find(files.begin(), files.end(), file_without_prefix);
    int position = std::distance(files.begin(), it) + 1;
    
    // Create the new filename
    std::ostringstream oss;
    oss << std::setfill('0') << std::setw(3) << position << "_" << file_without_prefix;
    return oss.str();
}


// Function to remove sequencial numbering from files
std::string remove_numbered_prefix(const std::string& file_string) {
    size_t pos = file_string.find_first_not_of("0123456789");

    // Check if the filename is already numbered and contains an underscore after numbering
    if (pos != std::string::npos && pos > 0 && file_string[pos] == '_' && file_string[pos - 1] != '_') {
        // Remove the number and the first underscore
        return file_string.substr(pos + 1);
    }

    // If the prefix contains a number at the beginning, remove it
    if (pos == 0) {
        size_t underscore_pos = file_string.find('_');
        if (underscore_pos != std::string::npos && underscore_pos > 0) {
            return file_string.substr(underscore_pos + 1);
        }
    }

    return file_string; // Return the original name if no number found or if number is not followed by an underscore
}


// Function to add current date to files
std::string append_date_seq(const std::string& file_string) {
    // Check if the filename already contains a date seq
    size_t dot_position = file_string.find_last_of('.');
    size_t underscore_position = file_string.find_last_of('_');
    if (dot_position != std::string::npos && underscore_position != std::string::npos && dot_position > underscore_position) {
        std::string date_seq = file_string.substr(underscore_position + 1, dot_position - underscore_position - 1);
        if (date_seq.size() == 8 && std::all_of(date_seq.begin(), date_seq.end(), ::isdigit)) {
            // Filename already contains a valid date seq, no need to append
            return file_string;
        }
    } else if (underscore_position != std::string::npos) {
        std::string date_seq = file_string.substr(underscore_position + 1);
        if (date_seq.size() == 8 && std::all_of(date_seq.begin(), date_seq.end(), ::isdigit)) {
            // Filename already contains a valid date seq, no need to append
            return file_string;
        }
    }

    auto now = std::chrono::system_clock::now();
    auto time_t_now = std::chrono::system_clock::to_time_t(now);
    std::tm* local_tm = std::localtime(&time_t_now);

    std::ostringstream oss;
    oss << std::put_time(local_tm, "%Y%m%d");
    std::string date_seq = oss.str();

    if (dot_position != std::string::npos) {
        return file_string.substr(0, dot_position) + "_" + date_seq + file_string.substr(dot_position);
    } else {
        return file_string + "_" + date_seq;
    }
}


// Function to remove date from files
std::string remove_date_seq(const std::string& file_string) {
    size_t dot_position = file_string.find_last_of('.');
    size_t underscore_position = file_string.find_last_of('_');

    if (underscore_position != std::string::npos) {
        if (dot_position != std::string::npos && dot_position > underscore_position) {
            std::string date_seq = file_string.substr(underscore_position + 1, dot_position - underscore_position - 1);
            if (date_seq.size() == 8 && std::all_of(date_seq.begin(), date_seq.end(), ::isdigit)) {
                // Valid date seq found, remove it
                return file_string.substr(0, underscore_position) + file_string.substr(dot_position);
            }
        } else {
            // No dot found after underscore, consider the substring from underscore to the end as potential date seq
            std::string date_seq = file_string.substr(underscore_position + 1);
            if (date_seq.size() == 8 && std::all_of(date_seq.begin(), date_seq.end(), ::isdigit)) {
                // Valid date seq found, remove it
                return file_string.substr(0, underscore_position);
            }
        }
    }

    // No valid date seq found, return original file_string
    return file_string;
}

 
// Folder numbering functions mv style
void rename_folders_with_sequential_numbering(const fs::path& base_directory, std::string prefix, std::atomic<int>& dirs_count, std::atomic<int>& skipped_folder_special_count, std::atomic<int>& depth, bool verbose_enabled, bool skipped, bool skipped_only, bool symlinks, size_t batch_size_folders, int num_paths) {
    // Reserve capacity for folders_to_rename
    std::vector<std::pair<fs::path, fs::path>> folders_to_rename;
    folders_to_rename.reserve(batch_size_folders);
    
    // Reserve capacity for unchanged_folder_paths
    std::vector<std::pair<fs::path, bool>> unchanged_folder_paths;
    unchanged_folder_paths.reserve(batch_size_folders);
    
    // Reserve capacity for folder_names
    std::vector<std::pair<std::string, fs::path>> folder_names;
    folder_names.reserve(batch_size_folders);

    bool renaming_needed = false;

    if (depth != 0) {
        if (depth > 0)
            --depth;

        // Collect folder paths and names
        for (const auto& folder : fs::directory_iterator(base_directory)) {
            bool skip = !symlinks && fs::is_symlink(folder);
            if (folder.is_directory() && !skip) {
                std::string folder_name = folder.path().filename().string();
                folder_names.emplace_back(folder_name, folder.path());
            }
        }

        // Sort folder names alphabetically, ignoring any existing numbering
        std::sort(folder_names.begin(), folder_names.end(), 
            [](const auto& a, const auto& b) {
                std::string name_a = a.first, name_b = b.first;
                size_t pos_a = name_a.find('_');
                size_t pos_b = name_b.find('_');
                if (pos_a != std::string::npos && std::all_of(name_a.begin(), name_a.begin() + pos_a, ::isdigit))
                    name_a = name_a.substr(pos_a + 1);
                if (pos_b != std::string::npos && std::all_of(name_b.begin(), name_b.begin() + pos_b, ::isdigit))
                    name_b = name_b.substr(pos_b + 1);
                return name_a < name_b;
            });

        // Check if renaming is needed
        int expected_counter = 1;
        for (const auto& [folder_name, folder_path] : folder_names) {
            size_t pos = folder_name.find('_');
            if (pos == std::string::npos || !std::all_of(folder_name.begin(), folder_name.begin() + pos, ::isdigit)) {
                renaming_needed = true;
                break;
            }
            int current_number = std::stoi(folder_name.substr(0, pos));
            if (current_number != expected_counter) {
                renaming_needed = true;
                break;
            }
            expected_counter++;
        }

        // Process sorted folder names only if renaming is needed
        if (renaming_needed) {
            int counter = 1;
            for (const auto& [folder_name, folder_path] : folder_names) {
                // Remove any existing numbering from the folder name
                std::string original_name = folder_name;
                size_t pos = folder_name.find('_');
                if (pos != std::string::npos && std::all_of(folder_name.begin(), folder_name.begin() + pos, ::isdigit)) {
                    original_name = folder_name.substr(pos + 1);
                }

                // Construct the new name with sequential numbering and original name
                std::stringstream ss;
                ss << std::setw(3) << std::setfill('0') << counter << "_" << original_name;
                fs::path new_name = base_directory / (prefix.empty() ? "" : (prefix + "_")) / ss.str();

                // Add folder to the vector for batch renaming only if the name has changed
                if (new_name != folder_path) {
                    folders_to_rename.emplace_back(folder_path, new_name);
                } else {
                    unchanged_folder_paths.emplace_back(folder_path, fs::is_symlink(folder_path));
                }

                counter++;
            }

            // Determine the number of threads to create
            unsigned int num_threads = std::min(static_cast<unsigned int>(folders_to_rename.size()), max_threads);

            if (num_paths > 1) {
                unsigned int max_threads_per_path = std::max(1u, max_threads / num_paths);
                num_threads = max_threads_per_path;
            }

            // Rename folders in parallel batches
            const size_t batch_size = std::min(batch_size_folders, folders_to_rename.size());
            #pragma omp parallel for shared(folders_to_rename, dirs_count) schedule(static, 1) num_threads(num_threads) if(num_threads > 1)
            for (size_t i = 0; i < folders_to_rename.size(); i += batch_size) {
                const size_t end = std::min(i + batch_size, folders_to_rename.size());
                for (size_t j = i; j < end; ++j) {
                    const auto& [old_path, new_path] = folders_to_rename[j];

                    try {
                        fs::rename(old_path, new_path);
                    } catch (const fs::filesystem_error& e) {
                        if (e.code() == std::errc::permission_denied && verbose_enabled) {
                            print_error("\033[1;91mError\033[0m: " + std::string(e.what()));
                        }
                        continue;
                    }
                    if (verbose_enabled && !skipped_only) {
                        if ((symlinks && fs::is_symlink(old_path)) || fs::is_symlink(new_path)) {
                            print_verbose_enabled("\033[0m\033[92mRenamed\033[0m\033[95m symlink_folder\033[0m " + old_path.string() + "\e[1;38;5;214m -> \033[0m" + new_path.string(), std::cout);
                        } else {
                            print_verbose_enabled("\033[0m\033[92mRenamed\033[0m\033[94m folder\033[0m " + old_path.string() + "\e[1;38;5;214m -> \033[0m" + new_path.string(), std::cout);
                        }
                    }
                    dirs_count.fetch_add(1, std::memory_order_relaxed);
                }
            }
        } else {
            // If no renaming is needed, add all folders to unchanged_folder_paths
            for (const auto& [folder_name, folder_path] : folder_names) {
                unchanged_folder_paths.emplace_back(folder_path, fs::is_symlink(folder_path));
            }
        }

        // Print folder paths that did not need renaming
        if (!unchanged_folder_paths.empty()) {
            for (const auto& [folder_path, is_symlink] : unchanged_folder_paths) {
                if (verbose_enabled && skipped) {
                    if (is_symlink) {
                        print_verbose_enabled("\033[0m\033[93mSkipped\033[0m\033[95m symlink_folder\033[0m " + folder_path.string() + " (name unchanged)", std::cout);
                    } else {
                        print_verbose_enabled("\033[0m\033[93mSkipped\033[0m\033[94m folder\033[0m " + folder_path.string() + " (name unchanged)", std::cout);
                    }
                }
                skipped_folder_special_count.fetch_add(1, std::memory_order_relaxed);
            }
        }
    }
}
