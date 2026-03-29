from ._ffi import ffi, lib


class TaskDevError(Exception):
    """Исключение для возникших ошибок"""

    pass


class TaskDev:
    """Обертка для библиотечных функций"""

    """ Text functions """

    @staticmethod
    def str_trim(s):
        """Обрезает пробелы в конце строки"""
        if not s:
            return s
        buf = ffi.new("char[]", s.encode())
        lib.taskdev_str_trim(buf)
        return ffi.string(buf).decode()

    @staticmethod
    def str_remove_whitespace(s):
        """Удаляет все пробельные символы"""
        if not s:
            return s
        buf = ffi.new("char[]", s.encode())
        lib.taskdev_str_remove_all_whitespace(buf)
        return ffi.string(buf).decode()

    @staticmethod
    def str_find_fuzzy(haystack, needle, max_diff):
        """Нечеткий поиск подстроки"""
        return bool(
            lib.taskdev_str_find_fuzzy(
                haystack.encode(), needle.encode(), max_diff)
        )

    """ Command functions """

    @staticmethod
    def cmd_check(command, expected_output):
        """Выполняет команду и проверяет, содержится ли expected_output в выводе"""
        return bool(lib.taskdev_cmd_check(command.encode(), expected_output.encode()))

    @staticmethod
    def cmd_check_fuzzy(command, expected_output, max_diff):
        """Выполняет команду и проверяет вывод нечетким поиском"""
        return bool(
            lib.taskdev_cmd_check_fuzzy(
                command.encode(), expected_output.encode(), max_diff
            )
        )

    @staticmethod
    def cmd_run(command):
        """Выполняет команду без вывода"""
        if not command:
            return False
        return bool(lib.taskdev_cmd_run_str(command.encode()))

    """  File  functions """

    @staticmethod
    def file_exists(filename):
        """Проверяет существование файла или директории"""
        if not filename:
            return False
        return bool(lib.taskdev_file_exists(filename.encode()))

    @staticmethod
    def file_executable(filename):
        """Проверяет, является ли файл исполняемым"""
        if not filename:
            return False
        return bool(lib.taskdev_file_executable(filename.encode()))

    @staticmethod
    def file_size(filename):
        """Возвращает размер файла в байтах"""
        if not filename:
            return -1
        return lib.taskdev_file_size(filename.encode())

    @staticmethod
    def file_read(filename):
        """Читает весь файл, возвращает (содержимое, размер)"""
        if not filename:
            return None, None
        size_ptr = ffi.new("long *")
        data = lib.taskdev_file_read(filename.encode(), size_ptr)
        if data == ffi.NULL:
            return None, None
        size = size_ptr[0]
        content = ffi.string(data, size).decode()
        ffi.C.free(data)
        return content, size

    @staticmethod
    def file_check(filename, expected_content):
        """Проверяет, содержится ли подстрока в файле"""
        if not filename or not expected_content:
            return False
        return bool(
            lib.taskdev_file_check(
                filename.encode(), expected_content.encode())
        )

    @staticmethod
    def file_check_exact(filename, expected_content):
        """Проверяет точное совпадение содержимого файла"""
        if not filename or not expected_content:
            return False
        return bool(
            lib.taskdev_file_check_exact(
                filename.encode(), expected_content.encode())
        )

    @staticmethod
    def file_compare(file1, file2):
        """Сравнивает два файла"""
        if not file1 or not file2:
            return False
        return bool(lib.taskdev_file_compare(file1.encode(), file2.encode()))

        """ Device functions """

    @staticmethod
    def dev_partition_size(device):
        """Возвращает размер раздела в килобайтах"""
        if not device:
            return -1
        return lib.taskdev_dev_partition_size(device.encode())

    @staticmethod
    def dev_check_filesystem_type(device, expected_fs):
        """Проверяет тип файловой системы устройства"""
        return bool(
            lib.taskdev_dev_check_filesystem_type(
                device.encode(), expected_fs.encode())
        )

    @staticmethod
    def dev_mounted(device, mount_point):
        """Проверяет, примонтировано ли устройство"""
        return bool(lib.taskdev_dev_mounted(device.encode(), mount_point.encode()))

    @staticmethod
    def dev_swap_active(device):
        """Проверяет, активен ли swap на устройстве"""
        if not device:
            return False
        return bool(lib.taskdev_dev_swap_active(device.encode()))

    @staticmethod
    def dev_check_fstab(device, mount_point, fs_type):
        """Проверяет наличие записи в /etc/fstab"""
        mp = mount_point.encode() if mount_point else ffi.NULL
        return bool(lib.taskdev_dev_check_fstab(device.encode(), mp, fs_type.encode()))

        """  Process Functions """

    @staticmethod
    def proc_find_pid(name):
        """Находит PID процесса по имени"""
        if not name:
            return -1
        return lib.taskdev_proc_find_pid(name.encode())

        """  Container Functions """

    @staticmethod
    def docker_container_running(container):
        """Проверяет, запущен ли Docker контейнер"""
        if not container:
            return False
        return bool(lib.taskdev_docker_container_running(container.encode()))

        """  Environment Functions """

    @staticmethod
    def env_get_user():
        """Возвращает имя текущего пользователя"""
        return ffi.string(lib.taskdev_env_get_user()).decode()

    @staticmethod
    def env_get_home():
        """Возвращает путь к домашней директории"""
        return ffi.string(lib.taskdev_env_get_home()).decode()

    @staticmethod
    def env_build_home_path(rel):
        """Строит полный путь от домашней директории"""
        if not rel:
            return TaskDev.env_get_home()
        path = lib.taskdev_env_build_home_path(rel.encode())
        result = ffi.string(path).decode()
        ffi.C.free(path)
        return result

    @staticmethod
    def env_get_str(name, default=None):
        """Получает значение переменной окружения"""
        if not name:
            return default
        default_bytes = default.encode() if default else ffi.NULL
        value = lib.taskdev_env_get_str(name.encode(), default_bytes)
        if value == ffi.NULL:
            return None
        return ffi.string(value).decode()

    @staticmethod
    def env_set(name, value, overwrite=True):
        """Устанавливает переменную окружения"""
        if not name or not value:
            return False
        return bool(lib.taskdev_env_set(name.encode(), value.encode(), overwrite))

    @staticmethod
    def env_unset(name):
        """Удаляет переменную окружения"""
        if not name:
            return False
        return bool(lib.taskdev_env_unset(name.encode()))

    @staticmethod
    def env_backup(name):
        """Создает бэкап переменной окружения"""
        if not name:
            return False
        return bool(lib.taskdev_env_backup(name.encode()))

    @staticmethod
    def env_restore(name):
        """Восстанавливает переменную окружения из бэкапа"""
        if not name:
            return False
        return bool(lib.taskdev_env_restore(name.encode()))

        """  Grade Functions """

    @staticmethod
    def grade_set(value):
        """Устанавливает оценку"""
        lib.taskdev_grade_set(value)

    @staticmethod
    def grade_add(value):
        """Добавляет/вычитает баллы к текущей оценке"""
        lib.taskdev_grade_add(value)

    @staticmethod
    def grade_get():
        """Возвращает текущую оценку"""
        return lib.taskdev_grade_get()

        """  Feedback functions """

    @staticmethod
    def feedback_add(message):
        """Добавляет сообщение обратной связи"""
        if not message:
            return
        lib.taskdev_feedback_add_str(message.encode())

    @staticmethod
    def feedback_add_formatted(format_str, *args):
        """Добавляет форматированное сообщение"""
        if not format_str:
            return
        message = format_str % args
        lib.taskdev_feedback_add_str(message.encode())

    @staticmethod
    def feedback_clear():
        """Очищает все сообщения"""
        lib.taskdev_feedback_clear()

    @staticmethod
    def feedback_set_at(index, message):
        """Заменяет сообщение по индексу"""
        lib.taskdev_feedback_set_at_str(index, message.encode())

    @staticmethod
    def feedback_get_at(index):
        """Возвращает сообщение по индексу"""
        msg = lib.taskdev_feedback_get_at(index)
        if msg == ffi.NULL:
            return None
        return ffi.string(msg).decode()

    @staticmethod
    def feedback_remove_at(index):
        """Удаляет сообщение по индексу"""
        lib.taskdev_feedback_remove_at(index)

    @staticmethod
    def feedback_count():
        """Возвращает количество сообщений"""
        return lib.taskdev_feedback_count()

    @staticmethod
    def feedback_empty():
        """Проверяет, есть ли сообщения"""
        return bool(lib.taskdev_feedback_empty())

    @staticmethod
    def get_all_feedback():
        """Возвращает список всех сообщений"""
        result = []
        count = TaskDev.feedback_count()
        for i in range(count):
            msg = TaskDev.feedback_get_at(i)
            if msg:
                result.append(msg)
        return result

        """  Output functions """

    @staticmethod
    def result_print_json(indent=2):
        """Выводит результат в JSON формате"""
        lib.taskdev_result_print_json(indent)


"""  Удобные функции для прямого импорта """
""" Text functions """
str_trim = TaskDev.str_trim
str_remove_whitespace = TaskDev.str_remove_whitespace
str_find_fuzzy = TaskDev.str_find_fuzzy

""" Command functions """
cmd_check = TaskDev.cmd_check
cmd_check_fuzzy = TaskDev.cmd_check_fuzzy
cmd_run = TaskDev.cmd_run

""" File functions """
file_exists = TaskDev.file_exists
file_executable = TaskDev.file_executable
file_size = TaskDev.file_size
file_read = TaskDev.file_read
file_check = TaskDev.file_check
file_check_exact = TaskDev.file_check_exact
file_compare = TaskDev.file_compare

""" Device functions """
dev_partition_size = TaskDev.dev_partition_size
dev_check_filesystem_type = TaskDev.dev_check_filesystem_type
dev_mounted = TaskDev.dev_mounted
dev_swap_active = TaskDev.dev_swap_active
dev_check_fstab = TaskDev.dev_check_fstab

""" Process functions """
proc_find_pid = TaskDev.proc_find_pid

""" Container functions """
docker_container_running = TaskDev.docker_container_running

""" Environment functions """
env_get_user = TaskDev.env_get_user
env_get_home = TaskDev.env_get_home
env_build_home_path = TaskDev.env_build_home_path
env_get_str = TaskDev.env_get_str
env_set = TaskDev.env_set
env_unset = TaskDev.env_unset
env_backup = TaskDev.env_backup
env_restore = TaskDev.env_restore

""" Grade functions """
grade_set = TaskDev.grade_set
grade_add = TaskDev.grade_add
grade_get = TaskDev.grade_get

""" Feedback functions """
feedback_add = TaskDev.feedback_add
feedback_add_formatted = TaskDev.feedback_add_formatted
feedback_clear = TaskDev.feedback_clear
feedback_set_at = TaskDev.feedback_set_at
feedback_get_at = TaskDev.feedback_get_at
feedback_remove_at = TaskDev.feedback_remove_at
feedback_count = TaskDev.feedback_count
feedback_empty = TaskDev.feedback_empty
get_all_feedback = TaskDev.get_all_feedback

""" Output functions """
result_print_json = TaskDev.result_print_json
