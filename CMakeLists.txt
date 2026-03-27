cmake_minimum_required(VERSION 4.2)
project(trabalhopi C)

set(CMAKE_C_STANDARD 11)

add_library(trabalhopi STATIC library.c
        corte-mediana.c
        linear.c
        k-means.c)
