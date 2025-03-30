cmake_minimum_required(VERSION 3.28)
project(EmptyWindow)
set(CXX_STANDARD 17)

find_package(glfw3 3 REQUIRED)

set(SRC
    src/main.cpp
)

add_executable(${PROJECT_NAME} ${SRC})

target_link_libraries(${PROJECT_NAME} glfw)
