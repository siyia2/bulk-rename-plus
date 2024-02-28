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
#include <execution>
#include <unordered_map>
#include <unordered_set>

namespace fs = std::filesystem;

// Mutexes

extern std::mutex dirs_count_mutex;
extern std::mutex files_count_mutex;
extern std::mutex files_mutex;
extern std::mutex cout_mutex;

// Function prototypes

// case modes

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
void remove_sequential_numbering_from_folders(const std::filesystem::path& base_directory, int& dirs_count, bool verbose_enabled, bool symlinks);
void rename_folders_with_sequential_numbering(const std::filesystem::path& base_directory, std::string prefix, int& dirs_count, bool verbose_enabled, bool symlinks);
void rename_folders_with_sequential_numbering(const std::filesystem::path& base_directory, int& dirs_count, bool verbose_enabled, bool symlinks);
void rename_folders_with_date_suffix(const std::filesystem::path& base_directory, int& dirs_count, bool verbose_enabled, bool symlinks);
void remove_date_suffix_from_folders(const std::filesystem::path& base_directory, int& dirs_count, bool verbose_enabled, bool symlinks);

// main

void print_error(const std::string& error, std::ostream& os = std::cerr);
void print_verbose_enabled(const std::string& message, std::ostream& os = std::cout);
void printVersionNumber(const std::string& version);
void print_help();
void rename_extension(const std::vector<fs::path>& item_paths, const std::string& case_input, bool verbose_enabled, int& files_count, size_t batch_size, bool symlinks);
void batch_rename_extension(const std::vector<std::pair<fs::path, fs::path>>& data, bool verbose_enabled, int& files_count, bool symlinks);
void rename_extension_path(const std::vector<std::string>& paths, const std::string& case_input, bool verbose_enabled, int depth, int& files_count, bool symlinks);
void rename_file(const fs::path& item_path, const std::string& case_input, bool is_directory, bool verbose_enabled, bool transform_dirs, bool transform_files, int& files_count, int& dirs_count, size_t batch_size, bool symlinks);
void rename_batch(const std::vector<std::pair<fs::path, std::string>>& data, bool verbose_enabled, int& files_count, int& dirs_count, bool symlinks, bool transform_files);
void process_forParents(const fs::directory_entry& entry, const std::string& case_input, bool verbose_enabled, bool transform_dirs, bool transform_files, int& files_count, int& dirs_count, int depth, bool symlinks);
void rename_directory(const std::filesystem::path& directory_path, const std::string& case_input, bool rename_parents, bool verbose_enabled, bool transform_dirs, bool transform_files, int& files_count, int& dirs_count, int depth, bool symlinks);
void rename_path(const std::vector<std::string>& paths, const std::string& case_input, bool rename_parents, bool verbose_enabled, bool transform_dirs, bool transform_files, int depth, bool symlinks);

#endif // HEADERS_H
