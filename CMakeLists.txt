cmake_minimum_required(VERSION 3.23)
project(MCPatcher)

set(CMAKE_CXX_STANDARD 20)

add_executable(MCPatcher main.cpp patcher.cpp patcher.h utils.cpp utils.h logger.h)

include_directories(include)

target_link_libraries(
        MCPatcher
        "${PROJECT_SOURCE_DIR}/lib/fmt.lib"
)