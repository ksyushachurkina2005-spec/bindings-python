#pragma once
#include <cstdarg>
#include <cstdlib>
#include <stdio.h>
#include <sys/types.h>

extern "C" {

////////// Text //////////
// Trim string from trailing whitespace
void taskdev_str_trim(char *str);
// Trim string from all whitespace
void taskdev_str_remove_all_whitespace(char *str);
// Check if 'needle' is a fuzzy substring of 'haystack'
bool taskdev_str_find_fuzzy(const char *haystack, const char *needle, int max_diff);

////////// Commands //////////
// Execute system command and compare its output with expected one
bool taskdev_cmd_check(const char *command, const char *expected_output);
// Execute system command and compare its output with expected one using fuzzy sting matching
bool taskdev_cmd_check_fuzzy(const char *command, const char *expected_output, int max_diff);
// Execute system command using plain c-string ('>/dev/null 2>&1' auto-appended at the end)
bool taskdev_cmd_run_str(const char *command);
// Execute system command using formatted string ('>/dev/null 2>&1' auto-appended at the end)
bool taskdev_cmd_run(const char *format, ...);

////////// Files //////////
// Check if file exists
bool taskdev_file_exists(const char *filename);
// Check if file exists and is executable
bool taskdev_file_executable(const char *filename);
// Get file size in bytes
long taskdev_file_size(const char *filename);
// Read entire file (file size in bytes can be obtained via 'out_size' param)
char *taskdev_file_read(const char *filename, long *out_size);
// Compare file content with expected one (line-based)
bool taskdev_file_check(const char *filename, const char *expected_content);
// Compare file content with expected one (exact matching)
bool taskdev_file_check_exact(const char *filename, const char *expected_content);
// Compare two files
bool taskdev_file_compare(const char *file1, const char *file2);

////////// Devices //////////
// Get partition size in kilobytes
long taskdev_dev_partition_size(const char *device);
// Check if filesystem type matches 'expected_fs'
bool taskdev_dev_check_filesystem_type(const char *device, const char *expected_fs);
// Check if 'device' is mounted
bool taskdev_dev_mounted(const char *device, const char *mount_point);
// Check if swap on 'device' is active
bool taskdev_dev_swap_active(const char *device);
// Check if there is a specified entry in /etc/fstab
bool taskdev_dev_check_fstab(const char *device, const char *mount_point, const char *fs_type);

////////// Processes //////////
// Find pid of a process by its name (= pidof / pgrep)
pid_t taskdev_proc_find_pid(const char *name);

////////// Containers //////////
// Check if docker container is running
bool taskdev_docker_container_running(const char *container);

/////////// Environment //////////
// Get current user
const char *taskdev_env_get_user(void);
// Get current home directory
const char *taskdev_env_get_home(void);
// Combine current home directory with relative path to smth
char *taskdev_env_build_home_path(const char *rel);
// Get string value of environment variable (fallback to 'default_value' if not found)
const char *taskdev_env_get_str(const char *name, const char *default_value);
// Set environment variable 'name'='value' (existing can be overwritten with 'overwrite'=true)
bool taskdev_env_set(const char *name, const char *value, bool overwrite);
// Unset environment variable
bool taskdev_env_unset(const char *name);
// Backup environment variable (possible to snapshot nonexistent variables).
// Note: making several backups of the same variable is not possible
bool taskdev_env_backup(const char *name);
// Restore previously backed-up environment variable
bool taskdev_env_restore(const char *name);

/////////// Grade //////////
// Set grade
void taskdev_grade_set(int value);
// Add/subtract 'value' to/from grade
void taskdev_grade_add(int value);
// Get current grade
int taskdev_grade_get(void);

/////////// Feedback //////////
// Append new feedback item as a formatted string
void taskdev_feedback_add(const char *format, ...);
// Append new feedback item as a plain c-string
void taskdev_feedback_add_str(const char *str);
// Remove all feedback items
void taskdev_feedback_clear(void);
// Replace feedback item at 'index' as a formatted string
void taskdev_feedback_set_at(int index, const char *format, ...);
// Replace feedback item at 'index' as a plain c-string
void taskdev_feedback_set_at_str(int index, const char *str);
// Get feedback item from 'index'
const char *taskdev_feedback_get_at(int index);
// Remove feedback item at 'index'
void taskdev_feedback_remove_at(int index);
// Number of feedback items currently appended (can be used as an index)
int taskdev_feedback_count(void);
// Check if feedback is empty
bool taskdev_feedback_empty(void);

/////////// Output //////////
// Print result to stdout in json format.
// Note: if 'indent' < 0, output is formatted as a single line
void taskdev_result_print_json(int indent);
}

namespace taskdev {

// Trim string from trailing whitespace
inline void str_trim(char *str) {
    taskdev_str_trim(str);
}

// Trim string from all whitespace
inline void str_remove_all_whitespace(char *str) {
    taskdev_str_remove_all_whitespace(str);
}

// Check if 'needle' is a fuzzy substring of 'haystack'
inline bool str_find_fuzzy(const char *haystack, const char *needle, int max_diff) {
    return taskdev_str_find_fuzzy(haystack, needle, max_diff);
}

// Execute system command and compare its output with expected one
inline bool cmd_check(const char *command, const char *expected_output) {
    return taskdev_cmd_check(command, expected_output);
}

// Execute system command and compare its output with expected one using fuzzy sting matching
inline bool cmd_check_fuzzy(const char *command, const char *expected_output, int max_diff) {
    return taskdev_cmd_check_fuzzy(command, expected_output, max_diff);
}

// Execute system command as a formatted string ('>/dev/null 2>&1' auto-appended at the end)
inline bool cmd_run(const char *format, ...) {
    va_list args;
    va_start(args, format);
    char *cmd;
    int len = vasprintf(&cmd, format, args);
    va_end(args);
    if (len < 0) return false;
    bool result = taskdev_cmd_run_str(cmd);
    free(cmd);
    return result;
}

// Check if file exists
inline bool file_exists(const char *filename) {
    return taskdev_file_exists(filename);
}

// Check if file exists and is executable
inline bool file_executable(const char *filename) {
    return taskdev_file_executable(filename);
}

// Get file size in bytes
inline long file_size(const char *filename) {
    return taskdev_file_size(filename);
}

// Read entire file (file size in bytes can be obtained via 'out_size' param)
inline char *file_read(const char *filename, long *out_size) {
    return taskdev_file_read(filename, out_size);
}

// Compare file content with expected one (line-based)
inline bool file_check(const char *filename, const char *expected_content) {
    return taskdev_file_check(filename, expected_content);
}

// Compare file content with expected one (exact matching)
inline bool file_check_exact(const char *filename, const char *expected_content) {
    return taskdev_file_check_exact(filename, expected_content);
}

// Compare two files
inline bool file_compare(const char *file1, const char *file2) {
    return taskdev_file_compare(file1, file2);
}

// Get partition size in kilobytes
inline long dev_partition_size(const char *device) {
    return taskdev_dev_partition_size(device);
}

// Check if filesystem type matches 'expected_fs'
inline bool dev_check_filesystem_type(const char *device, const char *expected_fs) {
    return taskdev_dev_check_filesystem_type(device, expected_fs);
}

// Check if 'device' is mounted
inline bool dev_mounted(const char *device, const char *mount_point) {
    return taskdev_dev_mounted(device, mount_point);
}

// Check if swap on 'device' is active
inline bool dev_swap_active(const char *device) {
    return taskdev_dev_swap_active(device);
}

// Check if there is a specified entry in /etc/fstab
inline bool dev_check_fstab(const char *device, const char *mount_point, const char *fs_type) {
    return taskdev_dev_check_fstab(device, mount_point, fs_type);
}

// Find pid of a process by its name (= pidof / pgrep)
inline pid_t proc_find_pid(const char *name) {
    return taskdev_proc_find_pid(name);
}

// Check if docker container is running
inline bool docker_container_running(const char *container) {
    return taskdev_docker_container_running(container);
}

// Get current user
inline const char *env_get_user(void) {
    return taskdev_env_get_user();
}

// Get current home directory
inline const char *env_get_home(void) {
    return taskdev_env_get_home();
}

// Combine current home directory with relative path to smth
inline char *env_build_home_path(const char *rel) {
    return taskdev_env_build_home_path(rel);
}

// Get string value of environment variable (fallback to 'default_value' if not found)
inline const char *env_get_str(const char *name, const char *default_value) {
    return taskdev_env_get_str(name, default_value);
}

// Set environment variable 'name'='value' (existing can be overwritten with 'overwrite'=true)
inline bool env_set(const char *name, const char *value, bool overwrite) {
    return taskdev_env_set(name, value, overwrite);
}

// Unset environment variable
inline bool env_unset(const char *name) {
    return taskdev_env_unset(name);
}

// Backup environment variable (possible to snapshot nonexistent variables).
// Note: making several backups of the same variable is not possible
inline bool env_backup(const char *name) {
    return taskdev_env_backup(name);
}

// Restore previously backed-up environment variable
inline bool env_restore(const char *name) {
    return taskdev_env_restore(name);
}

// Set grade
inline void grade_set(int value) {
    taskdev_grade_set(value);
}

// Add/subtract 'value' to/from grade
inline void grade_add(int value) {
    taskdev_grade_add(value);
}

// Get current grade
inline int grade_get() {
    return taskdev_grade_get();
}

// Append new feedback item as a formatted string
inline void feedback_add(const char *format, ...) {
    va_list args;
    va_start(args, format);
    char *buffer;
    if (vasprintf(&buffer, format, args) >= 0) {
        taskdev_feedback_add_str(buffer);
        free(buffer);
    }
    va_end(args);
}

// Remove all feedback items
inline void feedback_clear(void) {
    taskdev_feedback_clear();
}

// Replace feedback item at 'index' as a formatted string
inline void feedback_set_at(int index, const char *format, ...) {
    va_list args;
    va_start(args, format);
    char *buffer;
    if (vasprintf(&buffer, format, args) >= 0) {
        taskdev_feedback_set_at_str(index, buffer);
        free(buffer);
    }
    va_end(args);
}

// Get feedback item from 'index'
inline const char *feedback_get_at(int index) {
    return taskdev_feedback_get_at(index);
}

// Remove feedback item at 'index'
inline void feedback_remove_at(int index) {
    taskdev_feedback_remove_at(index);
}

// Number of feedback items currently appended (can be used as an index)
inline int feedback_count(void) {
    return taskdev_feedback_count();
}

// Check if feedback is empty
inline bool feedback_empty(void) {
    return taskdev_feedback_empty();
}

// Print result to stdout in json format.
// Note: if 'indent' < 0, output is formatted as a single line
inline void result_print_json(int indent = 2) {
    taskdev_result_print_json(indent);
}

} // namespace taskdev
