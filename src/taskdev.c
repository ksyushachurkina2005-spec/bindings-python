#include <ctype.h>
#include <limits.h>
#include <mntent.h>
#include <pwd.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "../include/taskdev.h"
#include "da.h"

static struct {
    int grade;

    struct {
        char **items;
        size_t count;
        size_t capacity;
    } feedback;
} result;

typedef struct {
    char *name;
    char *value;
} Env_Backup;

static struct {
    Env_Backup *items;
    size_t count;
    size_t capacity;
} env_backups;

void taskdev_str_trim(char *str) {
    size_t len = strlen(str);
    while (len > 0 && isspace(str[len - 1])) {
        str[--len] = '\0';
    }
}

void taskdev_str_remove_all_whitespace(char *str) {
    char *dst = str;
    for (char *src = str; *src; src++) {
        if (!isspace(*src)) {
            *dst++ = *src;
        }
    }
    *dst = '\0';
}

bool taskdev_str_find_fuzzy(const char *haystack, const char *needle, int max_diff) {
    int h_len = strlen(haystack);
    int n_len = strlen(needle);

    if (n_len == 0) return true;

    for (int start = 0; start <= h_len - (n_len - max_diff); start++) {
        int errors = 0;
        int n_index = 0;
        int h_index = start;

        while (n_index < n_len && h_index < h_len && errors <= max_diff) {
            if (haystack[h_index] == needle[n_index]) {
                n_index++;
                h_index++;
            } else {
                errors++;
                if (h_index + 1 < h_len && haystack[h_index + 1] == needle[n_index]) {
                    h_index++; // Skip one character in haystack (insertion)
                } else if (n_index + 1 < n_len && haystack[h_index] == needle[n_index + 1]) {
                    n_index++; // Skip one character in needle (deletion)
                } else {
                    // Skip in both (substitution)
                    n_index++;
                    h_index++;
                }
            }
        }

        if (n_index == n_len && errors <= max_diff) return true;
    }

    return false;
}

bool taskdev_cmd_check(const char *command, const char *expected_output) {
    FILE *f = popen(command, "r");
    if (!f) return false;

    char *line = NULL;
    size_t len = 0;
    ssize_t nread;
    bool found = false;

    while ((nread = getline(&line, &len, f)) != -1) {
        if (expected_output && strstr(line, expected_output)) {
            found = true;
            break;
        }
    }

    free(line);
    pclose(f);
    return found;
}

bool taskdev_cmd_check_fuzzy(const char *command, const char *expected_output, int max_diff) {
    FILE *f = popen(command, "r");
    if (!f) return false;

    char *line = NULL;
    size_t len = 0;
    ssize_t nread;
    bool found = false;

    while ((nread = getline(&line, &len, f)) != -1) {
        if (expected_output && taskdev_str_find_fuzzy(line, expected_output, max_diff)) {
            found = true;
            break;
        }
    }

    free(line);
    pclose(f);
    return found;
}

bool taskdev_cmd_run_str(const char *command) {
    char *cmd;
    if (asprintf(&cmd, "%s >/dev/null 2>&1", command) < 0) return false;
    int ret = system(cmd);
    free(cmd);
    return ret == 0;
}

bool taskdev_cmd_run(const char *format, ...) {
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

bool taskdev_file_exists(const char *filename) {
    struct stat st;
    return stat(filename, &st) == 0;
}

bool taskdev_file_executable(const char *filename) {
    struct stat st;
    return stat(filename, &st) == 0 && (st.st_mode & S_IXUSR);
}

long taskdev_file_size(const char *filename) {
    struct stat st;
    if (stat(filename, &st) == 0) return st.st_size;
    return -1;
}

char *taskdev_file_read(const char *filename, long *out_size) {
    FILE *f = fopen(filename, "rb");
    if (!f) return NULL;

    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    rewind(f);

    if (size < 0) {
        fclose(f);
        return NULL;
    }

    char *buf = malloc(size + 1);
    if (!buf) {
        fclose(f);
        return NULL;
    }

    size_t read = fread(buf, 1, size, f);
    fclose(f);

    if (read != (size_t)size) {
        free(buf);
        return NULL;
    }

    buf[size] = '\0';
    // Cast to long for consistency with 'taskdev_file_size()'
    if (out_size) *out_size = (long)size;
    return buf;
}

bool taskdev_file_check(const char *filename, const char *expected_content) {
    FILE *f = fopen(filename, "r");
    if (!f) return false;

    char *line = NULL;
    size_t len = 0;
    ssize_t nread;
    bool found = false;

    while ((nread = getline(&line, &len, f)) != -1) {
        taskdev_str_trim(line);
        if (strstr(line, expected_content)) {
            found = true;
            break;
        }
    }

    free(line);
    fclose(f);
    return found;
}

bool taskdev_file_check_exact(const char *filename, const char *expected_content) {
    char *content = taskdev_file_read(filename, NULL);
    char *expected = strdup(expected_content);

    if (!content || !expected) {
        free(content);
        free(expected);
        return false;
    }

    taskdev_str_remove_all_whitespace(content);
    taskdev_str_remove_all_whitespace(expected);

    bool equal = (strcmp(content, expected) == 0);
    free(content);
    free(expected);
    return equal;
}

bool taskdev_file_compare(const char *file1, const char *file2) {
    char *content1 = taskdev_file_read(file1, NULL);
    if (!content1) return false;
    bool equal = taskdev_file_check_exact(file2, content1);
    free(content1);
    return equal;
}

long taskdev_dev_partition_size(const char *device) {
    char command[256];
    char buffer[128];
    long size_kb = -1;

    snprintf(command, sizeof(command), "blockdev --getsize64 %s 2>/dev/null", device);
    FILE *f = popen(command, "r");
    if (f) {
        if (fgets(buffer, sizeof(buffer), f)) {
            unsigned long long size_bytes;
            if (sscanf(buffer, "%llu", &size_bytes) == 1) {
                size_kb = size_bytes / 1024;
            }
        }
        pclose(f);
    }
    return size_kb;
}

bool taskdev_dev_check_filesystem_type(const char *device, const char *expected_fs) {
    char command[256];
    snprintf(command, sizeof(command), "blkid -s TYPE -o value %s", device);
    return taskdev_cmd_check(command, expected_fs);
}

bool taskdev_dev_mounted(const char *device, const char *mount_point) {
    FILE *mtab = setmntent("/etc/mtab", "r");
    if (!mtab) return false;

    struct mntent *mnt;
    bool mounted = false;

    while ((mnt = getmntent(mtab)) != NULL) {
        if ((device && strcmp(mnt->mnt_fsname, device) == 0)
            || (mount_point && strcmp(mnt->mnt_dir, mount_point) == 0)) {
            mounted = true;
            break;
        }
    }

    endmntent(mtab);
    return mounted;
}

bool taskdev_dev_swap_active(const char *device) {
    return taskdev_cmd_check("swapon --show=name --noheadings", device);
}

bool taskdev_dev_check_fstab(const char *device, const char *mount_point, const char *fs_type) {
    FILE *fstab = fopen("/etc/fstab", "r");
    if (!fstab) return false;

    char line[512];
    bool found = false;

    while (fgets(line, sizeof(line), fstab)) {
        if (line[0] == '#' || line[0] == '\n') continue;

        char dev[256], mp[256], type[256], options[256], dump[256], pass[256];

        if (sscanf(line, "%255s %255s %255s %255s %255s %255s", dev, mp, type, options, dump, pass) >= 3) {
            if ((strcmp(dev, device) == 0) || (mount_point && strcmp(mp, mount_point) == 0)) {
                if (strcmp(type, fs_type) == 0) {
                    found = true;
                    break;
                }
            }
        }
    }

    fclose(fstab);
    return found;
}

pid_t taskdev_proc_find_pid(const char *name) {
    char command[256];
    snprintf(command, sizeof(command), "pgrep %s", name);

    FILE *f = popen(command, "r");
    if (!f) return -1;

    pid_t pid = -1;
    if (fscanf(f, "%d", &pid) != 1) return -1;

    pclose(f);
    return pid;
}

bool taskdev_docker_container_running(const char *container) {
    char command[256];
    snprintf(command, sizeof(command), "docker inspect -f '{{.State.Running}}' %s", container);
    return taskdev_cmd_check(command, "true");
}

const char *taskdev_env_get_user(void) {
    struct passwd *pw = getpwuid(getuid());
    return (pw) ? pw->pw_name : NULL;
}

const char *taskdev_env_get_home(void) {
    struct passwd *pw = getpwuid(getuid());
    return (pw) ? pw->pw_dir : NULL;
}

char *taskdev_env_build_home_path(const char *rel) {
    const char *home = taskdev_env_get_home();
    if (!home) return NULL;
    char *path;
    if (asprintf(&path, "%s%s", home, rel) < 0) return NULL;
    return path;
}

const char *taskdev_env_get_str(const char *name, const char *default_value) {
    const char *value = getenv(name);
    return value ? value : default_value;
}

bool taskdev_env_set(const char *name, const char *value, bool overwrite) {
    return setenv(name, value, overwrite) == 0;
}

bool taskdev_env_unset(const char *name) {
    return unsetenv(name) == 0;
}

static int find_backup(const char *name) {
    for (size_t i = 0; i < env_backups.count; ++i) {
        if (strcmp(env_backups.items[i].name, name) == 0) return (int)i;
    }
    return -1;
}

bool taskdev_env_backup(const char *name) {
    if (!name) return false;
    if (find_backup(name) != -1) return false;

    const char *current = getenv(name);
    char *value_copy = NULL;

    if (current) {
        value_copy = strdup(current);
        if (!value_copy) {
            free(value_copy);
            return false;
        }
    }

    char *name_copy = strdup(name);
    if (!name_copy) {
        free(value_copy);
        return false;
    }

    Env_Backup b = {.name = name_copy, .value = value_copy};
    da_append(&env_backups, b);
    return true;
}

bool taskdev_env_restore(const char *name) {
    if (!name) return false;
    int i = find_backup(name);
    if (i == -1) return false;

    Env_Backup *b = &env_backups.items[i];
    if (b->value)
        setenv(b->name, b->value, 1);
    else
        unsetenv(b->name);

    free(b->name);
    free(b->value);
    da_remove_unordered(&env_backups, (size_t)i);
    return true;
}

void taskdev_grade_set(int value) {
    if (value < 0) return;
    result.grade = value;
}

void taskdev_grade_add(int value) {
    if (result.grade + value < 0)
        result.grade = 0;
    else
        result.grade += value;
}

int taskdev_grade_get(void) {
    return result.grade;
}

void taskdev_feedback_add(const char *format, ...) {
    va_list args;
    va_start(args, format);

    char *buffer;
    int len = vasprintf(&buffer, format, args);
    va_end(args);

    if (len < 0) return;

    da_append(&result.feedback, buffer);
}

void taskdev_feedback_add_str(const char *str) {
    taskdev_feedback_add(str);
}

void taskdev_feedback_clear(void) {
    result.feedback.count = 0;
    result.feedback.capacity = 0;
    for (size_t i = 0; i < result.feedback.count; i++) {
        free(result.feedback.items[i]);
    }
    da_free(&result.feedback);
}

void taskdev_feedback_set_at(int index, const char *format, ...) {
    if (index >= (int)result.feedback.count) return;

    va_list args;
    va_start(args, format);

    char *buffer;
    int len = vasprintf(&buffer, format, args);
    va_end(args);

    if (len < 0) return;

    free(result.feedback.items[index]);
    result.feedback.items[index] = buffer;
}

void taskdev_feedback_set_at_str(int index, const char *str) {
    taskdev_feedback_set_at(index, str);
}

const char *taskdev_feedback_get_at(int index) {
    if (index >= (int)result.feedback.count) return NULL;
    return result.feedback.items[index];
}

void taskdev_feedback_remove_at(int index) {
    if (index >= (int)result.feedback.count) return;
    free(result.feedback.items[index]);
    da_remove_unordered(&result.feedback, index);
}

int taskdev_feedback_count(void) {
    return (int)result.feedback.count;
}

bool taskdev_feedback_empty(void) {
    return result.feedback.count == 0;
}

void taskdev_result_print_json(int indent) {
    if (indent >= 0) {
        // Pretty-print with indentation
        printf("%*s{\n", indent * 0, "");
        printf("%*s\"grade\": %d,\n", indent * 1, "", result.grade);

        if (result.feedback.count == 0) {
            printf("%*s\"feedback\": []\n", indent * 1, "");
        } else {
            printf("%*s\"feedback\": [\n", indent * 1, "");

            da_foreach(char *, it, &result.feedback) {
                printf("%*s\"%s\"", indent * 2, "", *it);
                if (it != result.feedback.items + result.feedback.count - 1) {
                    printf(",\n");
                } else {
                    printf("\n");
                }
            }

            printf("%*s]\n", indent * 1, "");
        }
        printf("%*s}\n", indent * 0, "");
    } else {
        // Single line mode
        printf("{\"grade\":%d,\"feedback\":[", result.grade);

        da_foreach(char *, it, &result.feedback) {
            printf("\"%s\"", *it);
            if (it != result.feedback.items + result.feedback.count - 1) {
                printf(",");
            }
        }

        printf("]}\n");
    }
}