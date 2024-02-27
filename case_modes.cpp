#include "headers.h"

// Separate string operations

// for Files&Dirs

// Function to convert a string to sentence case
std::string sentenceCase(const std::string& string) {
    if (string.empty()) return string; // Handling empty string case
    
    std::stringstream result; // Create a stringstream to build the result
    bool newWord = true; // Flag to track if a new word begins
    
    for (char c : string) { // Loop through each character in the string
        if (newWord && std::isalpha(c)) { // If it's a new word and an alphabet character
            result << static_cast<char>(std::toupper(c)); // Convert to uppercase
            newWord = false; // Reset new word flag
        } else {
            result << static_cast<char>(std::tolower(c)); // Convert to lowercase
        }
        if (std::isspace(c) || c == '.') { // Check for space or period to identify word boundaries
            newWord = true; // Set flag for a new word
        }
    }
    
    return result.str(); // Return the final sentence case string
}


// Function to capitalize the first letter of a string
std::string capitalizeFirstLetter(const std::string& input) {
    if (input.empty()) return input; // Handling empty string case
    
    std::stringstream result; // Create a stringstream to build the result
    bool first = true; // Flag to track the first character
    
    for (char c : input) { // Loop through each character in the input string
        if (first && std::isalpha(c)) { // If it's the first character and an alphabet character
            result << static_cast<char>(std::toupper(c)); // Convert to uppercase
            first = false; // Reset first character flag
        } else {
            result << static_cast<char>(std::tolower(c)); // Convert to lowercase
        }
    }
    
    return result.str(); // Return the final string with first letter capitalized
}


// Function to swap between upper and lower case letters in a string, capitalizing the first character of folder names
std::string swap_transform(const std::string& string) {
    std::stringstream transformed; // Create a stringstream to build the transformed string
    bool capitalize = false; // Flag to toggle between upper and lower case
    bool inFolderName = true; // Flag to track if currently within a folder name
    size_t folderDelimiter = string.find_last_of("/\\"); // Find the last folder delimiter
    size_t length = string.length(); // Cache the length of the input string

    for (size_t i = 0; i < length; ++i) { // Loop through each character in the string
        char c = string[i];
        if (i < folderDelimiter || folderDelimiter == std::string::npos) { // Check if within or after folder name
            if (inFolderName) {
                transformed << static_cast<char>(std::toupper(c)); // Capitalize first character in folder name
                inFolderName = false; // Exit folder name after first character
            } else {
                if (std::isalpha(c)) { // If it's an alphabet character
                    if (capitalize) {
                        transformed << static_cast<char>(std::toupper(c)); // Convert to uppercase
                    } else {
                        transformed << static_cast<char>(std::tolower(c)); // Convert to lowercase
                    }
                    capitalize = !capitalize; // Toggle between upper and lower case for subsequent characters
                } else {
                    transformed << c; // Keep non-alphabetic characters unchanged
                }
            }
        } else {
            transformed << c; // Keep characters after the folder delimiter unchanged
        }
    }

    return transformed.str(); // Return the transformed string
}


// Function to swap between upper and lower case letters in a string, lowercasing the first character of folder names
std::string swapr_transform(const std::string& string) {
    std::stringstream transformed; // Create a stringstream to build the transformed string
    bool capitalize = false; // Flag to toggle between upper and lower case
    bool inFolderName = true; // Flag to track if currently within a folder name
    size_t folderDelimiter = string.find_last_of("/\\"); // Find the last folder delimiter
    size_t length = string.length(); // Cache the length of the input string

    for (size_t i = 0; i < length; ++i) { // Loop through each character in the string
        char c = string[i];
        if (i < folderDelimiter || folderDelimiter == std::string::npos) { // Check if within or after folder name
            if (inFolderName) {
                transformed << static_cast<char>(std::tolower(c)); // Lowercase first character in folder name
                inFolderName = false; // Exit folder name after first character
            } else {
                if (std::isalpha(c)) { // If it's an alphabet character
                    if (capitalize) {
                        transformed << static_cast<char>(std::tolower(c)); // Convert to lowercase
                    } else {
                        transformed << static_cast<char>(std::toupper(c)); // Convert to uppercase
                    }
                    capitalize = !capitalize; // Toggle between lower and upper case for subsequent characters
                } else {
                    transformed << c; // Keep non-alphabetic characters unchanged
                }
            }
        } else {
            transformed << c; // Keep characters after the folder delimiter unchanged
        }
    }

    return transformed.str(); // Return the transformed string
}



// Function to convert a string to camel case
std::string to_camel_case(const std::string& string) {
    bool hasUpperCase = false; // Flag to track if the string has uppercase characters
    bool hasSpace = false; // Flag to track if the string has spaces

    for (char c : string) { // Loop through each character in the string
        if (std::isupper(c)) { // If it's an uppercase character
            hasUpperCase = true; // Set the flag
        } else if (c == ' ') { // If it's a space
            hasSpace = true; // Set the flag
        }
        if (c == '.') { // Stop processing after encountering a period
            break;
        }
    }

    if (!hasSpace && hasUpperCase) { // If there are no spaces and there are uppercase characters
        return string; // Return the original string
    }

    std::string result; // Create a string to store the result
    result.reserve(string.size() + 10); // Reserve memory for the result, adjust as needed

    bool capitalizeNext = false; // Flag to track if the next character should be capitalized
    bool afterDot = false; // Flag to track if currently after a dot

    for (char c : string) { // Loop through each character in the string
        if (c == '.') { // If it's a dot
            afterDot = true; // Set the flag
        }
        if (afterDot) { // If currently after a dot, keep characters unchanged
            result += c;
        } else { // If not after a dot
            if (std::isalpha(c)) { // If it's an alphabet character
                result += capitalizeNext ? std::toupper(c) : std::tolower(c); // Convert to lowercase or uppercase based on the flag
                capitalizeNext = false; // Reset the flag
            } else if (c == ' ') { // If it's a space
                capitalizeNext = true; // Set the flag for the next character to be capitalized
            } else { // For other characters
                result += c; // Keep them unchanged
            }
        }
    }

    return result; // Return the camel case string
}


// Function to convert a camel case string to a spaced string
std::string from_camel_case(const std::string& string) {
    std::string result; // Create a string to store the result
    result.reserve(string.size() + std::count_if(string.begin(), string.end(), ::isupper)); // Reserve space for the result string

    for (char c : string) { // Loop through each character in the input string
        if (std::isupper(c)) { // If it's an uppercase character
            result += ' '; // Add a space
            result += std::tolower(c); // Convert the character to lowercase and add it to the result
        } else { // If it's not an uppercase character
            result += c; // Add the character to the result
        }
    }

    return result; // Return the spaced string
}



// Function to convert a string to PascalCase
std::string to_pascal(const std::string& string) {
    bool hasUpperCase = false; // Flag to track if the string has uppercase characters
    bool hasSpace = false; // Flag to track if the string has spaces

    for (char c : string) { // Loop through each character in the string
        if (std::isupper(c)) { // If it's an uppercase character
            hasUpperCase = true; // Set the flag
        } else if (c == ' ') { // If it's a space
            hasSpace = true; // Set the flag
        }
        if (c == '.') { // Stop processing after encountering a period
            break;
        }
    }

    if (!hasSpace && hasUpperCase) { // If there are no spaces and there are uppercase characters
        return string; // Return the original string
    }

    std::string result; // Create a string to store the result
    result.reserve(string.size() + 10); // Reserve memory for the result, adjust as needed

    bool capitalizeNext = true; // Flag to track if the next character should be capitalized
    bool afterDot = false; // Flag to track if currently after a dot

    for (char c : string) { // Loop through each character in the string
        if (c == '.') { // If it's a dot
            afterDot = true; // Set the flag
        }
        if (afterDot) { // If currently after a dot, keep characters unchanged
            result += c;
            capitalizeNext = true; // Reset the flag for a new word after period
        } else { // If not after a dot
            if (std::isalpha(c)) { // If it's an alphabet character
                result += capitalizeNext ? std::toupper(c) : std::tolower(c); // Convert to uppercase or lowercase based on the flag
                capitalizeNext = false; // Reset the flag
            } else if (c == ' ') { // If it's a space
                capitalizeNext = true; // Set the flag for the next character to be capitalized
            } else { // For other characters
                result += c; // Keep them unchanged
            }
        }
    }

    return result; // Return the PascalCase string
}



// Function to convert a PascalCase string to a spaced string
std::string from_pascal_case(const std::string& string) {
    std::string result; // Create a string to store the result
    result.reserve(string.size()); // Reserve space for the result string

    for (size_t i = 0; i < string.size(); ++i) { // Loop through each character in the string
        char c = string[i];
        if (std::isupper(c)) { // If the current character is uppercase
            // If it's not the first character and the previous character was lowercase,
            // add a space before appending the uppercase character
            if (i != 0 && std::islower(string[i - 1])) {
                result += ' ';
            }
            result += c; // Add the uppercase character to the result
        } else { // If the current character is not uppercase
            result += std::tolower(c); // Convert the character to lowercase and add it to the result
        }
    }

    return result; // Return the spaced string
}


// For Files

// Function to append a numbered prefix to a file string based on existing numbered files in the parent directory
std::string append_numbered_prefix(const std::filesystem::path& parent_path, const std::string& file_string) {
    static std::unordered_map<std::filesystem::path, int> counter_map; // Static counter map to track numbering per parent path

    // Initialize counter if not already initialized
    if (counter_map.find(parent_path) == counter_map.end()) {
        // Find the highest existing numbered file
        int max_counter = 0;
        std::unordered_set<int> existing_numbers;

        for (const auto& entry : std::filesystem::directory_iterator(parent_path)) { // Iterate through files in parent directory
            if (entry.is_regular_file()) { // If it's a regular file
                std::string filename = entry.path().filename().string(); // Get the filename
                if (!filename.empty() && std::isdigit(filename[0])) { // If the filename starts with a digit
                    int number = std::stoi(filename.substr(0, filename.find('_'))); // Extract the number
                    existing_numbers.insert(number); // Add the number to existing numbers set
                    if (number > max_counter) {
                        max_counter = number; // Update max_counter if necessary
                    }
                }
            }
        }

        // Find the first gap in the sequence of numbers
        int gap = 1;
        while (existing_numbers.find(gap) != existing_numbers.end()) {
            gap++;
        }

        counter_map[parent_path] = gap - 1; // Initialize counter with the first gap
    }

    int& counter = counter_map[parent_path]; // Reference to the counter for the parent path

    if (!file_string.empty() && std::isdigit(file_string[0])) { // If the file_string starts with a digit, return it unchanged
        return file_string;
    }

    std::ostringstream oss; // Create a string stream to build the prefixed string
    counter++; // Increment counter before using its current value
    oss << std::setfill('0') << std::setw(3) << counter; // Format the counter with leading zeros up to three digits

    oss << "_" << file_string; // Append the file_string with the prefixed counter

    return oss.str(); // Return the prefixed string
}


// Function to remove a numbered prefix from a file string
std::string remove_numbered_prefix(const std::string& file_string) {
    size_t pos = file_string.find_first_not_of("0123456789"); // Find the first non-digit character

    // Check if the filename is already numbered and contains an underscore after numbering
    if (pos != std::string::npos && pos > 0 && file_string[pos] == '_' && file_string[pos - 1] != '_') {
        // Remove the number and the first underscore
        return file_string.substr(pos + 1);
    }

    // If the prefix contains a number at the beginning, remove it
    if (pos == 0) {
        size_t underscore_pos = file_string.find('_'); // Find the position of the first underscore
        if (underscore_pos != std::string::npos && underscore_pos > 0) {
            return file_string.substr(underscore_pos + 1); // Remove the number and the underscore
        }
    }

    return file_string; // Return the original name if no number found or if number is not followed by an underscore
}


// Function to append a date sequence to a file string if not already present
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

    auto now = std::chrono::system_clock::now(); // Get current system time
    auto time_t_now = std::chrono::system_clock::to_time_t(now); // Convert system time to time_t
    std::tm* local_tm = std::localtime(&time_t_now); // Convert time_t to local time struct

    std::ostringstream oss; // Create a string stream to build the date sequence
    oss << std::put_time(local_tm, "%Y%m%d"); // Format the current date as YYYYMMDD
    std::string date_seq = oss.str(); // Get the formatted date sequence

    if (dot_position != std::string::npos) { // If the file string contains an extension
        // Insert the date sequence between the filename and the extension
        return file_string.substr(0, dot_position) + "_" + date_seq + file_string.substr(dot_position);
    } else { // If the file string does not contain an extension
        // Append the date sequence to the end of the file string
        return file_string + "_" + date_seq;
    }
}


// Function to remove a date sequence from a file string if present
std::string remove_date_seq(const std::string& file_string) {
    size_t dot_position = file_string.find_last_of('.'); // Find the position of the last dot
    size_t underscore_position = file_string.find_last_of('_'); // Find the position of the last underscore

    if (underscore_position != std::string::npos) { // If an underscore is found in the file string
        if (dot_position != std::string::npos && dot_position > underscore_position) { // If a dot is found after the underscore
            std::string date_seq = file_string.substr(underscore_position + 1, dot_position - underscore_position - 1); // Extract the potential date sequence
            if (date_seq.size() == 8 && std::all_of(date_seq.begin(), date_seq.end(), ::isdigit)) { // Check if the extracted sequence is a valid date sequence
                // Valid date sequence found, remove it
                return file_string.substr(0, underscore_position) + file_string.substr(dot_position);
            }
        } else { // If no dot is found after the underscore
            // Consider the substring from underscore to the end as a potential date sequence
            std::string date_seq = file_string.substr(underscore_position + 1);
            if (date_seq.size() == 8 && std::all_of(date_seq.begin(), date_seq.end(), ::isdigit)) { // Check if the potential date sequence is valid
                // Valid date sequence found, remove it
                return file_string.substr(0, underscore_position);
            }
        }
    }

    // No valid date sequence found, return original file_string
    return file_string;
}


// Function to remove sequential numbering from folders within a base directory
void remove_sequential_numbering_from_folders(const fs::path& base_directory, int& dirs_count, bool verbose_enabled = false) {
    for (const auto& folder : fs::directory_iterator(base_directory)) { // Iterate through each entry in the base directory
        if (folder.is_directory()) { // If the entry is a directory
            std::string folder_name = folder.path().filename().string(); // Get the name of the directory

            // Check if the folder is numbered
            size_t underscore_pos = folder_name.find('_'); // Find the position of the underscore in the folder name
            if (underscore_pos != std::string::npos && std::isdigit(folder_name[0])) { // If an underscore is found and the first character is a digit
                // Extract the original name without the numbering
                std::string original_name = folder_name.substr(underscore_pos + 1);

                // Construct the new name without numbering
                fs::path new_name = base_directory / original_name;

                // Check if the folder is already renamed to the new name
                if (folder.path() != new_name) { // If the current name is different from the new name
                    // Move the contents of the source directory to the destination directory
                    try {
                        fs::rename(folder.path(), new_name); // Rename the folder
                    } catch (const fs::filesystem_error& e) { // Catch any filesystem errors
                        if (e.code() == std::errc::permission_denied) { // If permission denied error occurs
							std::lock_guard<std::mutex> lock(dirs_mutex);
                            std::cerr << "Error renaming folder due to permission denied: " << e.what() << std::endl; // Print error message
                        }
                        continue; // Skip renaming if moving fails
                    }
                    if (verbose_enabled) { // If verbose mode is enabled
                        // Print the renaming information
                        std::lock_guard<std::mutex> lock(dirs_mutex);
                        std::cout << "\033[0m\033[92mRenamed\033[0m\033[94m directory\033[0m " << folder.path() << " to " << new_name << std::endl;
                    }
                    std::lock_guard<std::mutex> lock(dirs_count_mutex);
                    ++dirs_count; // Increment dirs_count after each successful rename
                }

                // Recursively process subdirectories
                remove_sequential_numbering_from_folders(new_name, dirs_count, verbose_enabled);
            }
        }
    }
}


// Folder numbering functions mv style

// Function to rename folders within a base directory with sequential numbering and optional prefix
void rename_folders_with_sequential_numbering(const fs::path& base_directory, std::string prefix, int& dirs_count, bool verbose_enabled = false) {
    int counter = 1; // Counter for immediate subdirectories
    std::unordered_set<int> existing_numbers; // Store existing numbers for gap detection

    for (const auto& folder : fs::directory_iterator(base_directory)) { // Iterate through each entry in the base directory
        if (folder.is_directory()) { // If the entry is a directory
            std::string folder_name = folder.path().filename().string(); // Get the name of the directory

            // Extract number from the folder name if it is already numbered
            int number = 0;
            if (folder_name.find('_') != std::string::npos && std::isdigit(folder_name[0])) { // If an underscore is found and the first character is a digit
                number = std::stoi(folder_name.substr(0, folder_name.find('_'))); // Extract the number from the folder name
                existing_numbers.insert(number); // Add existing number to set
            } else { // If not already numbered
                // Skip if not already numbered
                continue;
            }

            // Find the first gap in the sequence of numbers
            int gap = 1;
            while (existing_numbers.find(gap) != existing_numbers.end()) { // While the gap is found in existing numbers
                gap++; // Increment gap to find the first missing number
            }

            // Increment counter to start from the first gap
            counter = gap;

            break; // Break loop after finding the first gap
        }
    }

    for (const auto& folder : fs::directory_iterator(base_directory)) { // Iterate through each entry in the base directory
        if (folder.is_directory()) { // If the entry is a directory
            std::string folder_name = folder.path().filename().string(); // Get the name of the directory

            // Check if the folder is already numbered
            if (folder_name.find('_') != std::string::npos && std::isdigit(folder_name[0])) { // If an underscore is found and the first character is a digit
                // Skip renaming if already numbered
                continue;
            }

            // Construct the new name with sequential numbering and original name
            std::stringstream ss; // Create a string stream to build the new name
            ss << std::setw(3) << std::setfill('0') << counter << "_" << folder_name; // Append original name to the numbering
            fs::path new_name = base_directory / (prefix.empty() ? "" : (prefix + "_")) / ss.str(); // Construct the new path

            // Check if the folder is already renamed to the new name
            if (folder.path() != new_name) { // If the current name is different from the new name
                // Move the contents of the source directory to the destination directory
                try {
                    fs::rename(folder.path(), new_name); // Rename the folder
                } catch (const fs::filesystem_error& e) { // Catch any filesystem errors
                    if (e.code() == std::errc::permission_denied) { // If permission denied error occurs
						std::lock_guard<std::mutex> lock(dirs_mutex);
                        std::cerr << "\033[1;91mError\033[0m: " << e.what() << std::endl; // Print error message
                    }
                    continue; // Skip renaming if moving fails
                }
                if (verbose_enabled) { // If verbose mode is enabled
                    // Print the renaming information
                    std::lock_guard<std::mutex> lock(dirs_mutex);
                    std::cout << "\033[0m\033[92mRenamed\033[0m\033[94m directory\033[0m " << folder.path() << " to " << new_name << std::endl;
                }
                std::lock_guard<std::mutex> lock(dirs_count_mutex);
                ++dirs_count; // Increment dirs_count after each successful rename
            }

            // Recursively process subdirectories with updated prefix
            rename_folders_with_sequential_numbering(new_name, prefix + ss.str(), dirs_count, verbose_enabled); // Recursively call the function with updated prefix
            counter++; // Increment counter after each directory is processed
        }
    }
}

// Overloaded function with default prefix = "" and verbose_enabled = false
void rename_folders_with_sequential_numbering(const fs::path& base_directory, int& dirs_count, bool verbose_enabled = false) {
    rename_folders_with_sequential_numbering(base_directory, "", dirs_count, verbose_enabled); // Call the main function with default prefix
}


// Function to rename folders with a date suffix based on the current date
void rename_folders_with_date_suffix(const fs::path& base_directory, int& dirs_count, bool verbose_enabled = false) {
    auto now = std::chrono::system_clock::now(); // Get the current system time
    auto time = std::chrono::system_clock::to_time_t(now); // Convert system time to time_t
    struct std::tm* parts = std::localtime(&time); // Convert time_t to local time struct

    for (const auto& folder : fs::directory_iterator(base_directory)) { // Iterate through each entry in the base directory
        if (folder.is_directory()) { // If the entry is a directory
            std::string folder_name = folder.path().filename().string(); // Get the name of the directory

            // Check if the folder name ends with the date suffix format "_YYYYMMDD"
            bool has_date_suffix = false;
            if (folder_name.length() >= 9) { // Minimum length required for "_YYYYMMDD"
                bool has_underscore = folder_name[folder_name.length() - 9] == '_'; // Check if the character before the date suffix is an underscore
                bool is_date_suffix = true;
                for (size_t i = folder_name.length() - 8; i < folder_name.length(); ++i) { // Check if the last 8 characters are all digits
                    if (!std::isdigit(folder_name[i])) {
                        is_date_suffix = false;
                        break;
                    }
                }
                has_date_suffix = has_underscore && is_date_suffix; // Folder name has a date suffix if it ends with "_YYYYMMDD"
            }

            if (has_date_suffix) // If the folder already has a date suffix, skip renaming
                continue;

            // Construct the new name with date suffix
            std::stringstream ss;
            ss << folder_name; // Keep the original folder name
            ss << "_" << (parts->tm_year + 1900) << std::setw(2) << std::setfill('0') << (parts->tm_mon + 1) << std::setw(2) << std::setfill('0') << parts->tm_mday; // Append date suffix in format "_YYYYMMDD"
            std::string new_name = ss.str(); // Get the new folder name

            // Check if the folder is already renamed to the new name
            fs::path new_path = folder.path().parent_path() / new_name; // Construct the full path for the new folder
            if (folder.path() != new_path) { // If the current name is different from the new name
                // Rename the folder
                try {
                    fs::rename(folder.path(), new_path); // Rename the folder
                } catch (const fs::filesystem_error& e) { // Catch any filesystem errors
                    if (e.code() == std::errc::permission_denied) { // If permission denied error occurs
						std::lock_guard<std::mutex> lock(dirs_mutex);
                        std::cerr << "\033[1;91mError\033[0m: " << e.what() << std::endl; // Print error message
                    }
                    continue; // Skip renaming if moving fails
                }
                if (verbose_enabled) { // If verbose mode is enabled
                    // Print the renaming information
                    std::lock_guard<std::mutex> lock(dirs_mutex);
                    std::cout << "\033[0m\033[92mRenamed\033[0m\033[94m directory\033[0m " << folder.path() << " to " << new_path << std::endl;
                }
                std::lock_guard<std::mutex> lock(dirs_count_mutex);
                ++dirs_count; // Increment dirs_count after each successful rename
            }

            // Recursively process subdirectories
            rename_folders_with_date_suffix(new_path, dirs_count, verbose_enabled); // Recursively call the function for the new folder
        }
    }
}


// Function to remove date suffix from folders within a base directory
void remove_date_suffix_from_folders(const fs::path& base_directory, int& dirs_count, bool verbose_enabled = false) {
    for (const auto& folder : fs::directory_iterator(base_directory)) { // Iterate through each entry in the base directory
        if (folder.is_directory()) { // If the entry is a directory
            std::string folder_name = folder.path().filename().string(); // Get the name of the directory

            // Check if the folder name ends with the date suffix format "_YYYYMMDD"
            if (folder_name.size() < 9 || folder_name.substr(folder_name.size() - 9, 1) != "_") // If the folder name is not long enough or does not end with "_"
                continue;

            bool is_date_suffix = true; // Assume it's a date suffix
            for (size_t i = folder_name.size() - 8; i < folder_name.size(); ++i) { // Check if the last 8 characters are all digits
                if (!std::isdigit(folder_name[i])) { // If any character is not a digit
                    is_date_suffix = false; // It's not a date suffix
                    break;
                }
            }

            if (!is_date_suffix) // If it's not a date suffix, continue to the next folder
                continue;

            // Remove the date suffix from the folder name
            std::string new_folder_name = folder_name.substr(0, folder_name.size() - 9); // Remove the last 9 characters (date suffix)

            // Check if the folder is already renamed to the new name
            fs::path new_path = folder.path().parent_path() / new_folder_name; // Construct the full path for the new folder
            if (folder.path() != new_path) { // If the current name is different from the new name
                // Rename the folder
                try {
                    fs::rename(folder.path(), new_path); // Rename the folder
                } catch (const fs::filesystem_error& e) { // Catch any filesystem errors
                    if (e.code() == std::errc::permission_denied) { // If permission denied error occurs
						std::lock_guard<std::mutex> lock(dirs_mutex);
                        std::cerr << "\033[1;91mError\033[0m: " << e.what() << std::endl; // Print error message
                    }
                    continue; // Skip renaming if moving fails
                }
                if (verbose_enabled) { // If verbose mode is enabled
                    // Print the renaming information
                    std::lock_guard<std::mutex> lock(dirs_mutex);
                    std::cout << "\033[0m\033[92mRenamed\033[0m\033[94m directory\033[0m " << folder.path() << " to " << new_path << std::endl;
                }
                std::lock_guard<std::mutex> lock(dirs_count_mutex);
                ++dirs_count; // Increment dirs_count after each successful rename
            }

            // Recursively process subdirectories
            remove_date_suffix_from_folders(new_path, dirs_count, verbose_enabled); // Recursively call the function for the new folder
        }
    }
}

