cmake_minimum_required(VERSION 3.10)

project(viper)

include_directories(.)

add_compile_options(-Wall -Werror -Wfatal-errors -Wmissing-prototypes -Wstrict-prototypes -Wshadow=local)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED True)

add_subdirectory(vp)
add_subdirectory(cli)

