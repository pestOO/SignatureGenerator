cmake_minimum_required(VERSION 3.15)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_BINARY_DIR ${CMAKE_BINARY_DIR}/build)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

find_package( Threads )
find_package(Boost COMPONENTS program_options filesystem REQUIRED)

if (${Boost_INCLUDE_DIRS})
    message(FATAL_ERROR "Boost include dir is missing")
endif()