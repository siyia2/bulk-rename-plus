#ifndef HEADERS_H
#define HEADERS_H

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
#include <unordered_set>

namespace fs = std::filesystem;

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
void remove_sequential_numbering_from_folders(const std::filesystem::path& base_directory, int& dirs_count, bool verbose_enabled);
void rename_folders_with_sequential_numbering(const std::filesystem::path& base_directory, std::string prefix, int& dirs_count, bool verbose_enabled);
void rename_folders_with_sequential_numbering(const std::filesystem::path& base_directory, int& dirs_count, bool verbose_enabled);
void rename_folders_with_date_suffix(const std::filesystem::path& base_directory, int& dirs_count, bool verbose_enabled);
void remove_date_suffix_from_folders(const std::filesystem::path& base_directory, int& dirs_count, bool verbose_enabled);

// main

void print_error(const std::string& error);
void print_verbose_enabled(const std::string& message);
void printVersionNumber(const std::string& version);
void print_help();
void rename_extension(const std::vector<fs::path>& item_paths, const std::string& case_input, bool verbose_enabled, int& files_count);
void rename_extension_path(const std::vector<std::string>& paths, const std::string& case_input, bool verbose_enabled, int depth, int& files_count);
void rename_file(const std::filesystem::path& item_path, const std::string& case_input, bool is_directory, bool verbose_enabled, bool transform_dirs, bool transform_files, int& files_count, int& dirs_count);
void rename_directory(const std::filesystem::path& directory_path, const std::string& case_input, bool rename_parents, bool verbose_enabled, bool transform_dirs, bool transform_files, int& files_count, int& dirs_count, int depth);
void rename_path(const std::vector<std::string>& paths, const std::string& case_input, bool rename_parents, bool verbose_enabled, bool transform_dirs, bool transform_files, int depth);

#endif // HEADERS_H
