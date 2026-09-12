cmake_minimum_required(VERSION 3.16)
project(alignx LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(alignx_core STATIC src/smith_waterman.cpp)
target_include_directories(alignx_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_compile_options(alignx_core PRIVATE -Wall -Wextra -Wpedantic)

add_executable(alignx src/main.cpp)
target_link_libraries(alignx PRIVATE alignx_core)

install(TARGETS alignx RUNTIME DESTINATION bin)
