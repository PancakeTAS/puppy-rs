cmake_minimum_required(VERSION 3.29)
project(purrify)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

file(GLOB SOURCES "src/*.c")

add_compile_options(-Wall -Wextra -Werror -Wno-unused-parameter -pedantic -fmacro-prefix-map=${CMAKE_SOURCE_DIR}/src/=)
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  add_compile_options(-g)
else()
  add_compile_options(-O3 -march=native -mtune=native)
  add_link_options(-flto=auto)
endif()

include_directories(nekos-best.c/src)

add_executable(purrify ${SOURCES})

target_link_libraries(purrify discord curl cjson)
