cmake_minimum_required(VERSION 4.2)
project(trabalhopi C)

set(CMAKE_C_STANDARD 11)

add_library(trabalhopi STATIC
        atv_1/corte-mediana.c
        atv_1/linear.c
        atv_1/k-means.c
        atv_2/soma_sub.c
        atv_2/multipicacao_escalar.c
        atv_2/divisao_escalar.c)

