
add_subdirectory(unittest)      # Include the unittest   subdirectory
if(NOT DEFINED ENV{CI})         # Only if not running on GitHub actions. We don't have display there
    add_subdirectory(regression)    # Include the regression subdirectory 
endif()