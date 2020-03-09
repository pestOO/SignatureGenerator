cmake_minimum_required(VERSION 3.15)
project(libSignatureGenerator)

set(CMAKE_CXX_STANDARD 14)

add_library(${PROJECT_NAME} SHARED
        Details/Utilities/CommonTypes.h
        Details/Utilities/MessageQueue.cpp
        Details/Utilities/MessageQueue.h
        Details/InputDataLayer/InputDataProvider.cpp
        Details/InputDataLayer/InputDataProvider.h
        Details/ProcessingLayer/SignatureGenerator.cpp
        Details/ProcessingLayer/SignatureGenerator.h
        Details/OutputDataLayer/OutputDataConsumer.cpp
        Details/OutputDataLayer/OutputDataConsumer.h
        Library.cpp
        Library.h )

target_link_libraries(${PROJECT_NAME} ${CMAKE_THREAD_LIBS_INIT})

if (${Boost_INCLUDE_DIRS})
    message(FATAL_ERROR "Boost include dir is missing")
endif()
include_directories(${Boost_INCLUDE_DIRS} Details)

#Boost::interprocess