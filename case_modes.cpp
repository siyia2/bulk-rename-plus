#include "headers.h"

namespace fs = std::filesystem;

// Separate string operations

// for Files&Dirs

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


std::string capitalizeFirstLetter(const std::string& input) {
    if (input.empty()) return input; // Handling empty string case
    
    std::stringstream result;
    bool first = true;
    
    for (char c : input) {
        if (first && std::isalpha(c)) {
            result << static_cast<char>(std::toupper(c));
            first = false;
        } else {
            result << c; // Keep non-alphabetic characters unchanged
        }
    }
    
    return result.str();
}

std::string swap_transform(const std::string& string) {
    std::stringstream transformed;
    bool capitalize = false; // Start by capitalizing
    bool inFolderName = true; // Start within folder name
    size_t folderDelimiter = string.find_last_of("/\\"); // Find the last folder delimiter
    size_t length = string.length(); // Cache the length of the string string

    for (size_t i = 0; i < length; ++i) {
        char c = string[i];
        if (i < folderDelimiter || folderDelimiter == std::string::npos) { // Ignore transformation after folder delimiter
            if (inFolderName) {
                transformed << static_cast<char>(std::toupper(c)); // Capitalize first character in folder name
                inFolderName = false; // Exit folder name after first character
            } else {
                if (std::isalpha(c)) {
                    if (capitalize) {
                        transformed << static_cast<char>(std::toupper(c));
                    } else {
                        transformed << static_cast<char>(std::tolower(c));
                    }
                    capitalize = !capitalize; // Toggle between upper and lower case
                } else {
                    transformed << c; // Keep non-alphabetic characters unchanged
                }
            }
        } else {
            transformed << c; // Keep characters after the folder delimiter unchanged
        }
    }

    return transformed.str();
}


std::string to_camel_case(const std::string& string) {
    bool hasUpperCase = false;
    bool hasSpace = false;

    for (char c : string) {
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

    if (!hasSpace && hasUpperCase) {
        return string;
    }

    std::string result;
    result.reserve(string.size() + 10); // Adjust the reserve size as needed

    bool capitalizeNext = false;
    bool afterDot = false;

    for (char c : string) {
        if (c == '.') {
            afterDot = true;
        }
        if (afterDot) {
            result += c;
        } else {
            if (std::isalpha(c)) {
                result += capitalizeNext ? toupper(c) : tolower(c); // Use toupper directly
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

std::string from_camel_case(const std::string& string) {
    std::string result;
    result.reserve(string.size() + std::count_if(string.begin(), string.end(), ::isupper)); // Reserve space for the result string

    for (char c : string) {
        if (std::isupper(c)) {
            result += ' ';
            result += std::tolower(c);
        } else {
            result += c;
        }
    }

    return result;
}


std::string to_pascal(const std::string& string) {
    bool hasUpperCase = false;
    bool hasSpace = false;

    for (char c : string) {
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

    if (!hasSpace && hasUpperCase) {
        return string;
    }
    

    std::string result;
    result.reserve(string.size() + 10); // Adjust the reserve size as needed

    bool capitalizeNext = true;
    bool afterDot = false;

    for (char c : string) {
        if (c == '.') {
            afterDot = true;
        }
        if (afterDot) {
            result += c;
        } else {
            if (std::isalpha(c)) {
                result += capitalizeNext ? toupper(c) : tolower(c); // Use toupper directly
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


std::string from_pascal_case(const std::string& string) {
    std::string result;
    result.reserve(string.size() + std::count_if(string.begin(), string.end(), ::isupper)); // Reserve space for the result string

    for (char c : string) {
        if (std::isupper(c)) {
            result += ' ';
            result += std::tolower(c);
        } else {
            result += c;
        }
    }

    return result;
}


// For Files

std::string append_numbered_prefix(const std::filesystem::path& parent_path, const std::string& file_string) {
    static std::unordered_map<std::filesystem::path, int> counter_map;

    // Initialize counter if not already initialized
    if (counter_map.find(parent_path) == counter_map.end()) {
        counter_map[parent_path] = 1;
    }

    int& counter = counter_map[parent_path];

    if (!file_string.empty() && std::isdigit(file_string[0])) {
        return file_string;
    }

    std::ostringstream oss;
    oss << std::setfill('0') << std::setw(3) << counter; // Width set to 3 for leading zeros up to three digits
    counter++; // Increment counter after using its current value

    oss << "_" << file_string;

    return oss.str();
}


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

void remove_sequential_numbering_from_folders(const fs::path& base_directory, int& dirs_count, bool verbose_enabled =false) {
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
                    if (verbose_enabled) {
                        std::cout << "\033[0m\033[92mRenamed\033[0m\033[94m directory\033[0m " << folder.path() << " to " << new_name << std::endl;
                    }
                    ++dirs_count; // Increment dirs_count after each successful rename
                }

                // Recursively process subdirectories
                remove_sequential_numbering_from_folders(new_name, dirs_count, verbose_enabled);
            }
        }
    }
}

// Folder numbering functions mv style


void rename_folders_with_sequential_numbering(const fs::path& base_directory, std::string prefix, int& dirs_count, bool verbose_enabled = false) {
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
                        std::cerr << "\033[1;91mError\033[0m: " << e.what() << std::endl;
                    }
                    continue; // Skip renaming if moving fails
                }
                if (verbose_enabled) {
                    std::cout << "\033[0m\033[92mRenamed\033[0m\033[94m directory\033[0m " << folder.path() << " to " << new_name << std::endl;
                }
                ++dirs_count; // Increment dirs_count after each successful rename
            }

            // Recursively process subdirectories with updated prefix
            rename_folders_with_sequential_numbering(new_name, prefix + ss.str(), dirs_count, verbose_enabled);
            counter++; // Increment counter after each directory is processed
        }
    }
}

// Overloaded function with default verbose_enabled = false
void rename_folders_with_sequential_numbering(const fs::path& base_directory, int& dirs_count, bool verbose_enabled) {
    rename_folders_with_sequential_numbering(base_directory, "", dirs_count, verbose_enabled);
}


void rename_folders_with_date_suffix(const fs::path& base_directory, int& dirs_count, bool verbose_enabled = false) {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    struct std::tm* parts = std::localtime(&time);
    
    for (const auto& folder : fs::directory_iterator(base_directory)) {
        if (folder.is_directory()) {
            std::string folder_name = folder.path().filename().string();

            // Check if the folder name ends with the date suffix format "_YYYYMMDD"
            bool has_date_suffix = false;
            if (folder_name.length() >= 9) { // Minimum length required for "_YYYYMMDD"
                bool has_underscore = folder_name[folder_name.length() - 9] == '_';
                bool is_date_suffix = true;
                for (size_t i = folder_name.length() - 8; i < folder_name.length(); ++i) {
                    if (!std::isdigit(folder_name[i])) {
                        is_date_suffix = false;
                        break;
                    }
                }
                has_date_suffix = has_underscore && is_date_suffix;
            }

            if (has_date_suffix)
                continue;

            // Construct the new name with date suffix
            std::stringstream ss;
            ss << folder_name; // Keep the original folder name
            ss << "_" << (parts->tm_year + 1900) << std::setw(2) << std::setfill('0') << (parts->tm_mon + 1) << std::setw(2) << std::setfill('0') << parts->tm_mday;
            std::string new_name = ss.str();

            // Check if the folder is already renamed to the new name
            fs::path new_path = folder.path().parent_path() / new_name;
            if (folder.path() != new_path) {
                // Rename the folder
                try {
                    fs::rename(folder.path(), new_path);
                } catch (const fs::filesystem_error& e) {
                    if (e.code() == std::errc::permission_denied) {
                        std::cerr << "\033[1;91mError\033[0m: " << e.what() << std::endl;
                    }
                    continue; // Skip renaming if moving fails
                }
                if (verbose_enabled) {
                    std::cout << "\033[0m\033[92mRenamed\033[0m\033[94m directory\033[0m " << folder.path() << " to " << new_path << std::endl;
                }
                ++dirs_count; // Increment dirs_count after each successful rename
            }

            // Recursively process subdirectories
            rename_folders_with_date_suffix(new_path, dirs_count, verbose_enabled);
        }
    }
}


void remove_date_suffix_from_folders(const fs::path& base_directory, int& dirs_count, bool verbose_enabled = false) {
    for (const auto& folder : fs::directory_iterator(base_directory)) {
        if (folder.is_directory()) {
            std::string folder_name = folder.path().filename().string();

            // Check if the folder name ends with the date suffix format "_YYYYMMDD"
            if (folder_name.size() < 9 || folder_name.substr(folder_name.size() - 9, 1) != "_")
                continue;

            bool is_date_suffix = true;
            for (size_t i = folder_name.size() - 8; i < folder_name.size(); ++i) {
                if (!std::isdigit(folder_name[i])) {
                    is_date_suffix = false;
                    break;
                }
            }

            if (!is_date_suffix)
                continue;

            // Remove the date suffix from the folder name
            std::string new_folder_name = folder_name.substr(0, folder_name.size() - 9);

            // Check if the folder is already renamed to the new name
            fs::path new_path = folder.path().parent_path() / new_folder_name;
            if (folder.path() != new_path) {
                // Rename the folder
                try {
                    fs::rename(folder.path(), new_path);
                } catch (const fs::filesystem_error& e) {
                    if (e.code() == std::errc::permission_denied) {
                        std::cerr << "\033[1;91mError\033[0m: " << e.what() << std::endl;
                    }
                    continue; // Skip renaming if moving fails
                }
                if (verbose_enabled) {
                    std::cout << "\033[0m\033[92mRenamed\033[0m\033[94m directory\033[0m " << folder.path() << " to " << new_path << std::endl;
                }
                ++dirs_count; // Increment dirs_count after each successful rename
            }

            // Recursively process subdirectories
            remove_date_suffix_from_folders(new_path, dirs_count, verbose_enabled);
        }
    }
}
