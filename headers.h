#ifndef HEADERS_H
#define HEADERS_H

#include <iostream>
#include <filesystem>
#include <algorithm>
#include <vector>
#include <mutex>
#include <chrono>
#include <queue>
#include <future>
#include <omp.h>
#include <execution>
#include <unordered_map>
#include <unordered_set>

namespace fs = std::filesystem;

// Global and shared mutexes

extern std::mutex dirs_count_mutex;
extern std::mutex cout_mutex;

// Global variable to set or not to set verbose output for skipped files/folders
extern bool skipped;

// Function prototypes

// Case modes

// General
std::string sentenceCase(const std::string& string);
std::string to_pascal(const std::string& string);
std::string from_pascal_case(const std::string& string);
std::string append_numbered_prefix(const std::filesystem::path& parent_path, const std::string& file_string);
std::string capitalizeFirstLetter(const std::string& string);
std::string swap_transform(const std::string& string);
std::string swapr_transform(const std::string& string);
std::string to_camel_case(const std::string& string);
std::string from_camel_case(const std::string& string);
std::string append_numbered_prefix(const std::filesystem::path& parent_path, const std::string& file_string);
std::string remove_numbered_prefix(const std::string& file_string);
std::string append_date_seq(const std::string& file_string);
std::string remove_date_seq(const std::string& file_string);
// Mv style for folder renaming only
void rename_folders_with_sequential_numbering(const fs::path& base_directory, std::string prefix, int& dirs_count, int depth, bool verbose_enabled, bool symlinks, size_t batch_size_folders);
void rename_folders_with_sequential_numbering(const fs::path& base_directory, std::string prefix, int& dirs_count, int depth, bool verbose_enabled, bool symlinks, size_t batch_size_folders);
void rename_folders_with_date_suffix(const fs::path& base_directory, int& dirs_count, bool verbose_enabled = false, bool symlinks = false, size_t batch_size_folders = 50, int depth = -1);
// Simplified folder renaming only
std::string get_renamed_folder_name_without_numbering(const fs::path& folder_path);
std::string get_renamed_folder_name_without_date(const fs::path& folder_path);

// main

// General
void print_error(const std::string& error, std::ostream& os = std::cerr);
void print_verbose_enabled(const std::string& message, std::ostream& os = std::cout);
void printVersionNumber(const std::string& version);
void print_help();
// For file extensions
void batch_rename_extension(const std::vector<std::pair<fs::path, fs::path>>& data, bool verbose_enabled, int& files_count);
void rename_extension(const std::vector<fs::path>& item_paths, const std::string& case_input, bool verbose_enabled, int& files_count, size_t batch_size_files, bool symlinks);
void rename_extension_path(const std::vector<std::string>& paths, const std::string& case_input, bool verbose_enabled, int depth, int& files_count, size_t batch_size_files, bool symlinks);
// For file renaming
void rename_file(const fs::path& item_path, const std::string& case_input, bool is_directory, bool verbose_enabled, bool transform_dirs, bool transform_files, int& files_count, int& dirs_count, size_t batch_size_files, bool symlinks);
void rename_batch(const std::vector<std::pair<fs::path, std::string>>& data, bool verbose_enabled, int& files_count, int& dirs_count);
// For folder renaming
void rename_directory(const fs::path& directory_path, const std::string& case_input, bool rename_parents, bool verbose_enabled, bool transform_dirs, bool transform_files, int& files_count, int& dirs_count, int depth, size_t batch_size_files, size_t batch_size_folders, bool symlinks);
void rename_path(const std::vector<std::string>& paths, const std::string& case_input, bool rename_parents, bool verbose_enabled, bool transform_dirs, bool transform_files, int depth, int dirs_count, int files_count, size_t batch_size_files, size_t batch_size_folders, bool symlinks);

#endif // HEADERS_H
