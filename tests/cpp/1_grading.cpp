#include "../../include/taskdev.hpp"

int main() {

    taskdev::feedback_add("1st item");
    taskdev::feedback_add("somthing is %d, %d", 69, 6969);
    taskdev::feedback_add("3rd item");
    taskdev::grade_set(100);
    taskdev::result_print_json(4);

    taskdev::feedback_remove_at(taskdev::feedback_count() - 1);
    taskdev::grade_add(50);
    taskdev::result_print_json();

    taskdev::feedback_add("%d", 42);
    taskdev::feedback_set_at(0, "hello %s", ":)");
    taskdev::feedback_set_at(1, taskdev::feedback_get_at(0));
    taskdev::grade_add(-100);
    taskdev::result_print_json();

    taskdev::feedback_clear();
    taskdev::grade_add(-999);
    taskdev::result_print_json(-1);

    return 0;
}