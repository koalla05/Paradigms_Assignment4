cmake_minimum_required(VERSION 3.28)
project(Paradigms_Assignment4)

set(CMAKE_CXX_STANDARD 17)

# Add the cezar.cpp file and build it as a shared library
add_library(cezar SHARED cezar.cpp)

# Add executable
add_executable(Paradigms_Assignment4 main.cpp)

# Link the cezar shared library
target_link_libraries(Paradigms_Assignment4 PRIVATE cezar)

# Path to the DLL file
set(CEZAR_DLL_PATH "C:/Users/kovko/CLionProjects/Paradigms_Assignment4/cezar.dll")

# Ensure the DLL is copied to the build directory
add_custom_command(TARGET Paradigms_Assignment4 POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CEZAR_DLL_PATH} $<TARGET_FILE_DIR:Paradigms_Assignment4>)