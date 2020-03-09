cmake_minimum_required(VERSION 3.15)
project(SignatureGenerator)

include("cmake/config.cmake")

add_subdirectory(libSignatureGenerator)
add_subdirectory(SignatureGenerator)
add_subdirectory(tests)