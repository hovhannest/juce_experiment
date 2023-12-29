cmake_minimum_required(VERSION 3.15)

project(HelloWorldApp VERSION 0.0.1)

# Set the C++ standard to C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(cmake/cpm.cmake)

if (CMAKE_SYSTEM_NAME MATCHES "Windows")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
endif()

CPMAddPackage(
    NAME JUCE
    GITHUB_REPOSITORY juce-framework/JUCE
    GIT_TAG 7.0.8
    VERSION 7.0.8
    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/juce
)

# `juce_add_gui_app` adds an executable target with the name passed as the first argument
# (GuiAppExample here). This target is a normal CMake target, but has a lot of extra properties set
# up by default. This function accepts many optional arguments. Check the readme at
# `docs/CMake API.md` in the JUCE repo for the full list.

juce_add_gui_app(hello_world
    # VERSION ...                       # Set this if the app version is different to the project version
    # ICON_BIG ...                      # ICON_* arguments specify a path to an image file to use as an icon
    # ICON_SMALL ...
    # DOCUMENT_EXTENSIONS ...           # Specify file extensions that should be associated with this app
    # COMPANY_NAME ...                  # Specify the name of the app's author
    MICROPHONE_PERMISSION_ENABLED 1
    MICROPHONE_PERMISSION_TEXT "Microphone access is Needed."
    PRODUCT_NAME "hello_world"
)         # The name of the final executable, which can differ from the target name


target_sources(hello_world
    PRIVATE
        src/main.cpp
        src/gui/MainComponent.cpp
)

# Add a function library
add_library(my_functions src/foo.cpp)

# treat all warnings as errors
if (MSVC)
    target_compile_options(hello_world PRIVATE /Wall /WX /wd4668 /wd4711 /wd4710)
    target_compile_options(my_functions PRIVATE /Wall /WX )
else()
    target_compile_options(hello_world PRIVATE -Wall -Wextra -Wpedantic -Werror)
    target_compile_options(my_functions PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

# Include directories for headers
target_include_directories(my_functions PUBLIC include)

target_compile_definitions(hello_world
    PRIVATE
        # JUCE_WEB_BROWSER and JUCE_USE_CURL would be on by default, but you might not need them.
        JUCE_WEB_BROWSER=0  # If you remove this, add `NEEDS_WEB_BROWSER TRUE` to the `juce_add_gui_app` call
        JUCE_USE_CURL=0     # If you remove this, add `NEEDS_CURL TRUE` to the `juce_add_gui_app` call
        JUCE_APPLICATION_NAME_STRING="$<TARGET_PROPERTY:hello_world,JUCE_PRODUCT_NAME>"
        JUCE_APPLICATION_VERSION_STRING="$<TARGET_PROPERTY:hello_world,JUCE_VERSION>"
)

target_link_libraries(hello_world
    PRIVATE
        # GuiAppData            # If we'd created a binary data target, we'd link to it here
        juce::juce_gui_extra
        juce::juce_audio_devices
        juce::juce_audio_utils
        my_functions
        -static-libstdc++
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)

# Option to enable testing
option(ENABLE_TESTING "Enable testing" ON)

# Option to specify the GTest installation directory
set(GTEST_INSTALL_DIR "" CACHE PATH "Path to Google Test installation directory")

if (ENABLE_TESTING)
    enable_testing()

    CPMAddPackage(
        NAME googletest
        GITHUB_REPOSITORY google/googletest
        GIT_TAG release-1.12.1
        VERSION 1.12.1
        OPTIONS "INSTALL_GTEST OFF" "gtest_force_shared_crt"
    )
    # Get the executable path of the hello_world target
    get_target_property(TESTING_EXECUTABLE_DIR hello_world RUNTIME_OUTPUT_DIRECTORY)
    get_target_property(TESTING_EXECUTABLE_NAME hello_world OUTPUT_NAME)

    add_subdirectory(test) # Include the test subdirectory
endif()