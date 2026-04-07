from _taskdev import ffi, lib

def str_trim(s):
    """Trim trailing whitespace from string"""
    if not s:
        return s
    buf = ffi.new("char[]", s.encode())
    lib.taskdev_str_trim(buf)
    return ffi.string(buf).decode()


def str_remove_all_whitespace(s):
    """Remove all whitespace characters from string"""
    if not s:
        return s
    buf = ffi.new("char[]", s.encode())
    lib.taskdev_str_remove_all_whitespace(buf)
    return ffi.string(buf).decode()


def str_find_fuzzy(haystack, needle, max_diff):
    """Fuzzy substring search using Levenshtein distance"""
    return bool(lib.taskdev_str_find_fuzzy(
        haystack.encode(),
        needle.encode(),
        max_diff
    ))


def file_exists(filename):
    """Check if file or directory exists"""
    if not filename:
        return False
    return bool(lib.taskdev_file_exists(filename.encode()))


def file_executable(filename):
    """Check if file exists and is executable"""
    if not filename:
        return False
    return bool(lib.taskdev_file_executable(filename.encode()))


def file_size(filename):
    """Get file size in bytes, returns -1 on error"""
    if not filename:
        return -1
    return lib.taskdev_file_size(filename.encode())


def file_read(filename):
    """Read entire file, returns (content, size) or (None, None) on error"""
    if not filename:
        return None, None
    size_ptr = ffi.new("long *")
    data = lib.taskdev_file_read(filename.encode(), size_ptr)
    if data == ffi.NULL:
        return None, None
    size = size_ptr[0]
    content = ffi.string(data, size).decode()
    return content, size



def file_check(filename, expected_content):
    """Check if file contains expected_content (line-based, trimmed)"""
    if not filename or not expected_content:
        return False
    return bool(lib.taskdev_file_check(
        filename.encode(),
        expected_content.encode()
    ))


def file_check_exact(filename, expected_content):
    """Check if file content exactly matches expected (whitespace ignored)"""
    if not filename or not expected_content:
        return False
    return bool(lib.taskdev_file_check_exact(
        filename.encode(),
        expected_content.encode()
    ))


def file_compare(file1, file2):
    """Compare two files (whitespace ignored)"""
    if not file1 or not file2:
        return False
    return bool(lib.taskdev_file_compare(
        file1.encode(),
        file2.encode()
    ))

def dev_partition_size(device):
    """Get partition size in kilobytes, returns -1 on error"""
    if not device:
        return -1
    return lib.taskdev_dev_partition_size(device.encode())


def dev_check_filesystem_type(device, expected_fs):
    """Check if device has expected filesystem type"""
    return bool(lib.taskdev_dev_check_filesystem_type(
        device.encode(),
        expected_fs.encode()
    ))


def dev_mounted(device, mount_point):
    """Check if device is mounted at mount_point"""
    return bool(lib.taskdev_dev_mounted(
        device.encode(),
        mount_point.encode()
    ))


def dev_swap_active(device):
    """Check if swap on device is active"""
    if not device:
        return False
    return bool(lib.taskdev_dev_swap_active(device.encode()))


def dev_check_fstab(device, mount_point, fs_type):
    """Check if fstab entry exists for device/mount_point with given fs_type"""
    mp = mount_point.encode() if mount_point else ffi.NULL
    return bool(lib.taskdev_dev_check_fstab(
        device.encode(),
        mp,
        fs_type.encode()
    ))

def proc_find_pid(name):
    """Find PID of process by name, returns -1 if not found"""
    if not name:
        return -1
    return lib.taskdev_proc_find_pid(name.encode())

def docker_container_running(container):
    """Check if Docker container is running"""
    if not container:
        return False
    return bool(lib.taskdev_docker_container_running(container.encode()))

def env_get_user():
    """Get current username"""
    return ffi.string(lib.taskdev_env_get_user()).decode()


def env_get_home():
    """Get current home directory path"""
    return ffi.string(lib.taskdev_env_get_home()).decode()


def env_build_home_path(rel):
    """Build full path from home directory: ~/rel"""
    if not rel:
        return env_get_home()
    path = lib.taskdev_env_build_home_path(rel.encode())
    result = ffi.string(path).decode()
    return result

def env_get_str(name, default=None):
    """Get environment variable value, returns default if not found"""
    if not name:
        return default
    default_bytes = default.encode() if default else ffi.NULL
    value = lib.taskdev_env_get_str(name.encode(), default_bytes)
    if value == ffi.NULL:
        return None
    return ffi.string(value).decode()


def env_set(name, value, overwrite=True):
    """Set environment variable"""
    if not name or not value:
        return False
    return bool(lib.taskdev_env_set(
        name.encode(),
        value.encode(),
        overwrite
    ))


def env_unset(name):
    """Unset environment variable"""
    if not name:
        return False
    return bool(lib.taskdev_env_unset(name.encode()))


def env_backup(name):
    """Backup environment variable"""
    if not name:
        return False
    return bool(lib.taskdev_env_backup(name.encode()))


def env_restore(name):
    """Restore environment variable from backup"""
    if not name:
        return False
    return bool(lib.taskdev_env_restore(name.encode()))

def grade_set(value):
    """Set grade (0-100)"""
    if value < 0:
        return
    lib.taskdev_grade_set(value)


def grade_add(value):
    """Add/subtract points from current grade"""
    lib.taskdev_grade_add(value)


def grade_get():
    """Get current grade"""
    return lib.taskdev_grade_get()

def feedback_add_str(message):
    """Add feedback message as plain string"""
    if not message:
        return
    lib.taskdev_feedback_add_str(message.encode())


def feedback_add(format_str, *args):
    """Add formatted feedback message"""
    if not format_str:
        return
    message = format_str % args
    lib.taskdev_feedback_add_str(message.encode())


def feedback_clear():
    """Clear all feedback messages"""
    lib.taskdev_feedback_clear()


def feedback_set_at_str(index, message):
    """Replace feedback message at index with plain string"""
    lib.taskdev_feedback_set_at_str(index, message.encode())


def feedback_set_at(index, format_str, *args):
    """Replace feedback message at index with formatted string"""
    message = format_str % args
    lib.taskdev_feedback_set_at_str(index, message.encode())


def feedback_get_at(index):
    """Get feedback message at index"""
    msg = lib.taskdev_feedback_get_at(index)
    if msg == ffi.NULL:
        return None
    return ffi.string(msg).decode()


def feedback_remove_at(index):
    """Remove feedback message at index"""
    lib.taskdev_feedback_remove_at(index)


def feedback_count():
    """Get number of feedback messages"""
    return lib.taskdev_feedback_count()


def feedback_empty():
    """Check if feedback is empty"""
    return bool(lib.taskdev_feedback_empty())


def get_all_feedback():
    """Get all feedback messages as list"""
    result = []
    count = feedback_count()
    for i in range(count):
        msg = feedback_get_at(i)
        if msg:
            result.append(msg)
    return result

def result_print_json(indent=2):
    """Print result in JSON format"""
    lib.taskdev_result_print_json(indent)
