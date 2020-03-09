project(tests)
enable_testing()

add_executable(${PROJECT_NAME} libSignatureGeneratorTest.cpp)

target_include_directories(${PROJECT_NAME} PRIVATE ${BOOST_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR})
target_link_libraries(${PROJECT_NAME} PRIVATE Boost::headers libSignatureGenerator)

add_test(NAME "All tests" COMMAND ${PROJECT_NAME} )