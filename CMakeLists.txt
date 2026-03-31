cmake_minimum_required(VERSION 3.10)

project(main LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

find_package(glfw3 REQUIRED)
find_package(GLEW REQUIRED)
find_package(OpenGL REQUIRED)

add_executable(${CMAKE_PROJECT_NAME}
    nn.cpp
    main.cpp
)

target_link_libraries(${CMAKE_PROJECT_NAME} glfw GLEW::GLEW OpenGL::OpenGL)

target_compile_options(${CMAKE_PROJECT_NAME} PRIVATE
    -Wall
    -Wextra
    -Werror
    -Wpedantic
    -g
    -O0
)