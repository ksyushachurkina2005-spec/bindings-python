import sys

sys.path.insert(0, '.')

try:
    import taskdev
except ImportError as e:
    print(f"ERROR: Cannot import taskdev module: {e}")
    print("Make sure you have built and installed the module:")
    print("cd ~/bindings-python/bindings/python")
    print("python3 build_taskdev.py")
    print("pip install .")
    sys.exit(1)


def test_file_exists():
    """Test file_exists function"""
    result = taskdev.file_exists("/etc/passwd")
    print(f"file_exists('/etc/passwd'): {'PASS' if result else 'FAIL'}")
    result = taskdev.file_exists("/nonexistent")
    print(f"file_exists('/nonexistent'): {'PASS' if not result else 'FAIL'}")


def test_cmd_run():
    """Test cmd_run function"""
    result = taskdev.cmd_run_str("echo hello")
    print(f"cmd_run_str('echo hello'): {'PASS' if result else 'FAIL'}")


def test_grade():
    """Test grade functions"""
    taskdev.grade_set(75)
    result = taskdev.grade_get()
    print(f"grade_set(75), grade_get(): {'PASS' if result == 75 else 'FAIL'} (got {result})")


def test_feedback():
    """Test feedback functions"""
    taskdev.feedback_clear()
    taskdev.feedback_add_str("test message")
    count = taskdev.feedback_count()
    print(f"feedback_add_str(), feedback_count(): {'PASS' if count == 1 else 'FAIL'} (got {count})")


def test_result_json():
    """Test result_print_json function"""
    print("result_print_json() output:")
    taskdev.result_print_json()


def main():
    print("Testing taskdev Python bindings\n")
    
    test_file_exists()
    test_cmd_run()
    test_grade()
    test_feedback()
    print()
    test_result_json()
    
    print("\n All tests completed")


if __name__ == "__main__":
    main()
