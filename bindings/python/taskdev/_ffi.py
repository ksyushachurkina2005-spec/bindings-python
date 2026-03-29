from cffi import FFI

ffi = FFI()

ffi.cdef("""  
    typedef int pid_t;

    // Text functions
    void taskdev_str_trim(char *str);
    void taskdev_str_remove_all_whitespace(char *str);
    bool taskdev_str_find_fuzzy(const char *haystack, const char *needle, int max_diff);

    // Commands
    bool taskdev_cmd_check(const char *command, const char *expected_output);
    bool taskdev_cmd_check_fuzzy(const char *command, const char *expected_output, int max_diff);
    bool taskdev_cmd_run_str(const char *command);

    // Files
    bool taskdev_file_exists(const char *filename);
    bool taskdev_file_executable(const char *filename);
    long taskdev_file_size(const char *filename);
    char *taskdev_file_read(const char *filename, long *out_size);
    bool taskdev_file_check(const char *filename, const char *expected_content);
    bool taskdev_file_check_exact(const char *filename, const char *expected_content);
    bool taskdev_file_compare(const char *file1, const char *file2);

    // Devices
    long taskdev_dev_partition_size(const char *device);
    bool taskdev_dev_check_filesystem_type(const char *device, const char *expected_fs);
    bool taskdev_dev_mounted(const char *device, const char *mount_point);
    bool taskdev_dev_swap_active(const char *device);
    bool taskdev_dev_check_fstab(const char *device, const char *mount_point, const char *fs_type);

    // Processes
    pid_t taskdev_proc_find_pid(const char *name);

    // Containers
    bool taskdev_docker_container_running(const char *container);

    // Environment
    const char *taskdev_env_get_user(void);
    const char *taskdev_env_get_home(void);
    char *taskdev_env_build_home_path(const char *rel);
    const char *taskdev_env_get_str(const char *name, const char *default_value);
    bool taskdev_env_set(const char *name, const char *value, bool overwrite);
    bool taskdev_env_unset(const char *name);
    bool taskdev_env_backup(const char *name);
    bool taskdev_env_restore(const char *name);

    // Grade
    void taskdev_grade_set(int value);
    void taskdev_grade_add(int value);
    int taskdev_grade_get(void);

    // Feedback
    void taskdev_feedback_add_str(const char *str);
    void taskdev_feedback_clear(void);
    void taskdev_feedback_set_at_str(int index, const char *str);
    const char *taskdev_feedback_get_at(int index);
    void taskdev_feedback_remove_at(int index);
    int taskdev_feedback_count(void);
    bool taskdev_feedback_empty(void);

    // Output
    void taskdev_result_print_json(int indent);
""")

lib = ffi.dlopen("libtaskdev.so")