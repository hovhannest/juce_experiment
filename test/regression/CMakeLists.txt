find_package (Python COMPONENTS Interpreter REQUIRED)
if(Python_FOUND)
    message(STATUS "Python version ${Python_VERSION} found")
    message(STATUS "Python executable: ${Python_EXECUTABLE}")
    message(STATUS "Python standard install directory: ${Python_STDLIB}")
    message(STATUS "Python include directories: ${Python_INCLUDE_DIRS}")
    message(STATUS "Python library directories: ${Python_LIBRARY_DIRS}")
    message(STATUS "Python libraries: ${Python_LIBRARIES}")
endif()

# detect virtualenv and set Pip args accordingly
if(DEFINED ENV{VIRTUAL_ENV} OR DEFINED ENV{CONDA_PREFIX})
  set(_pip_args)
else()
  set(_pip_args "--user")
endif()

# install PyPI Python package using pip
execute_process(COMMAND ${Python_EXECUTABLE} -m pip install -r ${CMAKE_CURRENT_SOURCE_DIR}/requirements.txt )

function(concatenate_intest_app_paths result_var base_path component)
    # Determine the platform-specific path separator
    if(CMAKE_HOST_SYSTEM_NAME MATCHES "Windows")
        set(PATH_SEPARATOR "\\")
        set(EXTENSION ".exe")
    elseif(CMAKE_HOST_SYSTEM_NAME MATCHES "Darwin")
      set(PATH_SEPARATOR "/")
      set(EXTENSION ".app")
    else()
        set(PATH_SEPARATOR "/")
        set(EXTENSION "")
    endif()

    # Trim trailing slashes from the base_path
    string(REGEX REPLACE "[/\\\\]+$" "" base_path "${base_path}")

    # Concatenate the path components with the appropriate separator
    set("${result_var}" "${base_path}${PATH_SEPARATOR}${component}${EXTENSION}" PARENT_SCOPE)
endfunction()

concatenate_intest_app_paths(TESTING_EXECUTABLE_FULL_PATH "${TESTING_EXECUTABLE_DIR}" "${TESTING_EXECUTABLE_NAME}")

add_test (NAME regression-tests
  COMMAND ${Python_EXECUTABLE} -m pytest ${CMAKE_CURRENT_SOURCE_DIR} --capture=tee-sys --app "${TESTING_EXECUTABLE_FULL_PATH}"
  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/test/regression
)