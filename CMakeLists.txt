cmake_minimum_required(VERSION 3.26)
project(Spark)

set(CMAKE_C_STANDARD 90)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

add_compile_options(-ansi -Wall -Wextra -Werror)

add_executable(Spark
        main.c
        spark/types/bool.h
        spark/types/int.h
        spark/types/float.h
        spark/types/type.h
)
