cmake_minimum_required(VERSION 3.15)

set(CMAKE_CXX_STANDARD 14)
set(COMPILE_WARNING_FLAGS "-Werror -Wall -Wextra -Weffc++ -pedantic \
    -pedantic-errors -Wextra -Wunreachable-code -Wunused \
    -Wunused-function  -Wunused-label  -Wunused-parameter \
    -Wunused-value  -Wunused-variable")
# TODO: add stripping for all any compiler
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2")
# enable one of the following lines for testing with sanitizer
# TODO(EZ): enable leak as a part of address sanitizer
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fsanitize-address-use-after-scope -g")
# warning: test time increase 10 times (from 30 sec to 5 min)
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=thread  -g -O1")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=undefined -fno-sanitize-recover=all -g")
# not working for clang with darwin19
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=memory -fPIE -pie -fno-omit-frame-pointer -g")

set(CMAKE_BINARY_DIR ${CMAKE_BINARY_DIR}/build)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

find_package( Threads )
find_package(Boost COMPONENTS program_options filesystem iostreams REQUIRED)

if (${Boost_INCLUDE_DIRS})
    message(FATAL_ERROR "Boost include dir is missing")
endif()