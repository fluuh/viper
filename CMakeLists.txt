cmake_minimum_required(VERSION 3.10)

project(viper)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED True)

add_library(viper)

add_executable(viper.cli)
set_target_properties(viper.cli PROPERTIES OUTPUT_NAME viper)

target_compile_options(viper PRIVATE -Wall -Wmissing-prototypes -Wstrict-prototypes -Wshadow=local)

include(Sources.cmake)

target_link_libraries(viper.cli viper)
