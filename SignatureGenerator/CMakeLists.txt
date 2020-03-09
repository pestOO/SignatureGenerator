project(SignatureGeneratorApplication)

add_executable(${PROJECT_NAME} main.cpp details/Application.cpp details/Application.h)

target_link_libraries(${PROJECT_NAME} Boost::program_options)
