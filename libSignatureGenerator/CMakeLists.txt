cmake_minimum_required(VERSION 3.15)
project(libSignatureGenerator)

set(CMAKE_CXX_STANDARD 14)

#TODO: split to components cmake
add_library(${PROJECT_NAME} SHARED
        # Common
        Components/Utilities/CommonTypes.h
        Components/Utilities/MessageQueue.h
        Components/Utilities/MessageQueue.cpp
        # In layer
        Components/InputDataLayer/details/InDataChunk.h
        Components/InputDataLayer/details/InDataListener.h
        Components/InputDataLayer/details/InDataChunkImpl.h
        Components/InputDataLayer/details/InDataChunkImpl.cpp
        Components/InputDataLayer/InputDataProvider.h
        Components/InputDataLayer/InputDataProvider.cpp
        # processing layer
        Components/ProcessingLayer/details/ChunkSignature.h
        Components/ProcessingLayer/details/DataAvailableListener.h
        Components/ProcessingLayer/details/ChunkSignatureImpl.h
        Components/ProcessingLayer/details/ChunkSignatureImpl.cpp
        Components/ProcessingLayer/SignatureGenerator.h
        Components/ProcessingLayer/SignatureGenerator.cpp
        # out layer
        Components/OutputDataLayer/OutputDataConsumer.cpp
        Components/OutputDataLayer/OutputDataConsumer.h
        # Library Facade
        Library.h
        Library.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE ${CMAKE_THREAD_LIBS_INIT} Boost::headers Boost::filesystem Boost::iostreams)
target_include_directories(${PROJECT_NAME} PRIVATE ${Boost_INCLUDE_DIRS} Components)
add_compile_options(${COMPILE_WARNING_FLAGS})
