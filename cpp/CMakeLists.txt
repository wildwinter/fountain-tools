cmake_minimum_required(VERSION 3.15)
project(ScreenplayTools LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Export compile commands for IDE support
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Source files
file(GLOB LIB_SOURCES "src/*.cpp" "src/fountain/*.cpp" "src/fdx/*.cpp")
file(GLOB LIB_HEADERS "include/screenplay_tools/*.h" "include/screenplay_tools/fountain/*.h" "include/screenplay_tools/fdx/*.h")

# Create the library (static or shared)
option(BUILD_SHARED_LIBS "Build shared libraries instead of static" ON)
add_library(${PROJECT_NAME} ${LIB_SOURCES} ${LIB_HEADERS})

# Set include directories for the target
target_include_directories(${PROJECT_NAME} PUBLIC include)

# Set properties for export
set_target_properties(${PROJECT_NAME} PROPERTIES
    PUBLIC_HEADER "${LIB_HEADERS}"
)

# Install target
install(TARGETS ${PROJECT_NAME}
    LIBRARY DESTINATION lib  # For shared libraries
    ARCHIVE DESTINATION lib  # For static libraries
    PUBLIC_HEADER DESTINATION include/screenplay_tools
)

# Add test executable
add_executable(tests 
    test/catch_amalgamated.cpp
    test/fountain/test_parser.cpp
    test/fountain/test_format_helper.cpp
    test/fountain/test_callback_parser.cpp
    test/fountain/test_writer.cpp
    test/fdx/test_parser.cpp
    test/test_utils.cpp)

# Link the library to the test executable
target_link_libraries(tests PRIVATE ScreenplayTools)

# Include Catch2 header (if not installed globally)
target_include_directories(tests PRIVATE include)

# Enable testing (CMake's built-in test support)
enable_testing()

# Register the test executable
add_test(NAME ScreenplayToolsTests COMMAND tests -r console)