project(SignatureGeneratorApplication)

add_executable(${PROJECT_NAME} details/Application.cpp details/Application.h main.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE Boost::program_options libSignatureGenerator)
