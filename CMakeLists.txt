cmake_minimum_required(VERSION 3.23)
project(MCPatcher)

set(CMAKE_CXX_STANDARD 20)

add_executable(MCPatcher main.cpp patcher.cpp patcher.h utils.cpp utils.h)

include_directories(include)

target_link_directories(MCPatcher PRIVATE lib)
target_link_libraries(MCPatcher PRIVATE spdlog)