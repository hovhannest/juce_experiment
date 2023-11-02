find_package (Python COMPONENTS Interpreter)
if(Python_FOUND)
    message(STATUS "Python version ${Python_VERSION} found")
    message(STATUS "Python executable: ${Python_EXECUTABLE}")
    message(STATUS "Python standard install directory: ${Python_STDLIB}")
    message(STATUS "Python include directories: ${Python_INCLUDE_DIRS}")
    message(STATUS "Python library directories: ${Python_LIBRARY_DIRS}")
    message(STATUS "Python libraries: ${Python_LIBRARIES}")
endif()

add_test (NAME regression-tests
  COMMAND ${Python_EXECUTABLE} -m pytest ${CMAKE_CURRENT_SOURCE_DIR} --capture=tee-sys --app "${TESTING_EXECUTABLE_DIR}${TESTING_EXECUTABLE_NAME}"
  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/test/regression
)