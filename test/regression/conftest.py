import pytest
import subprocess
import time
import sys
import os

import pyscreeze
import PIL
__PIL_TUPLE_VERSION = tuple(int(x) for x in PIL.__version__.split("."))
pyscreeze.PIL__version__ = __PIL_TUPLE_VERSION

def pytest_addoption(parser):
    parser.addoption(
        "--app",
        action="store"
    )

@pytest.fixture(scope='session')
def app_path(request):
    name_value = request.config.option.app
    if name_value is None:
        pytest.skip()
    return name_value.replace('/', os.path.sep).replace('\\', os.path.sep)

def closeAll(app_path):
    if sys.platform == "win32":
        pg = os.path.join(os.path.dirname(__file__), "kill_process.ps1")
        subprocess.run(["powershell", pg , app_path], shell=True, check=True)
    if sys.platform == "darwin":
        quit_process = subprocess.Popen(f"osascript -e 'quit app \"{app_path}\"'", shell=True)
        quit_process.wait()
    else:
        subprocess.run(["killall", app_path], shell=False)


def get_resource(path):
    if sys.platform == "win32":
        suffix = "_win"
    elif sys.platform == "darwin":
        suffix = "_mac"
    else:
        suffix = "_linux"

    # Split the path into its components
    dir_name, base_name = os.path.split(path)

    # Split the filename and its extension
    file_name, file_extension = os.path.splitext(base_name)

    # Add the suffix to the filename
    new_file_name = file_name + suffix + file_extension

    # Join the path back together
    new_path = os.path.join(dir_name, new_file_name)

    # Check if the new_path exists
    if os.path.exists(new_path):
        return new_path
    else:
        return path

@pytest.fixture
def os_resource_getter():
    return get_resource


@pytest.fixture(scope="function")
def start_application(app_path):
    open_args = [app_path]
    if sys.platform == "darwin":
        open_args = ["open", "-W", "-a", app_path]
    closeAll(app_path)
    app_process = subprocess.Popen(open_args)
    # Allow some time for the application to start (you may need to adjust the sleep duration)
    time.sleep(2)
    yield app_process
    closeAll(app_path)
