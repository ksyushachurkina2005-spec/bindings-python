#include "../../include/taskdev.h"

int main() {

    taskdev_feedback_add("1st item");
    taskdev_feedback_add("somthing is %d, %d", 69, 6969);
    taskdev_feedback_add("3rd item");
    taskdev_grade_set(100);
    taskdev_result_print_json(4);

    taskdev_feedback_remove_at(taskdev_feedback_count() - 1);
    taskdev_grade_add(50);
    taskdev_result_print_json(2);

    taskdev_feedback_add("%d", 42);
    taskdev_feedback_set_at(0, "hello %s", ":)");
    taskdev_feedback_set_at(1, taskdev_feedback_get_at(0));
    taskdev_grade_add(-100);
    taskdev_result_print_json(2);

    taskdev_feedback_clear();
    taskdev_grade_add(-999);
    taskdev_result_print_json(-1);

    return 0;
}
