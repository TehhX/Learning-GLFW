cmake_minimum_required(VERSION 3.28)
project(PointsAboutOrigin LANGUAGES C)

add_executable(PointsAboutOrigin main.c)

target_link_libraries(PointsAboutOrigin PRIVATE m)
