import pytest
import subprocess
import time

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
    return name_value+".app"

@pytest.fixture(scope="function")
def start_application(app_path):
    quit_process = subprocess.Popen(f"osascript -e 'quit app \"{app_path}\"'", shell=True)
    quit_process.wait()
    app_process = subprocess.Popen(["open", "-W", "-a", app_path])
    # Allow some time for the application to start (you may need to adjust the sleep duration)
    time.sleep(1)
    yield app_process
    # Teardown: Terminate the application after the test
    quit_process = subprocess.Popen(f"osascript -e 'quit app \"{app_path}\"'", shell=True)
    quit_process.wait()