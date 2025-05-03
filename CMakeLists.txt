cmake_minimum_required(VERSION 3.7)
project(DemocracyArrows LANGUAGES C VERSION 0.1.0)

add_executable(game src/main.c)
install(TARGETS game)