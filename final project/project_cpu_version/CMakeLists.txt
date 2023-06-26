cmake_minimum_required(VERSION 3.16)
project(CS171-final-project)

set(CMAKE_CXX_STANDARD 17)

find_package(OpenMP)

add_subdirectory(libs)
add_subdirectory(src)

add_executable(${PROJECT_NAME}-main main.cpp)

target_link_libraries(${PROJECT_NAME}-main
        PRIVATE
        renderer)