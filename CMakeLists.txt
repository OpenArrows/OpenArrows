cmake_minimum_required(VERSION 3.10)
project(DemocracyArrows LANGUAGES C VERSION 0.1.0)

add_subdirectory(lib/glfw)
add_subdirectory(lib/glad/cmake)

glad_add_library(glad_gl_core_mx_33 REPRODUCIBLE MX API gl:core=3.3)

add_executable(game src/main.c)
target_link_libraries(game PRIVATE glfw glad_gl_core_mx_33)
install(TARGETS game)