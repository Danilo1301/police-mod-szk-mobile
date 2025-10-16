cmake_minimum_required(VERSION 3.22)
project(MyLib LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Caminho do Android NDK
set(NDK_PATH "C:/android-ndk-r26d")
set(CMAKE_TOOLCHAIN_FILE "${NDK_PATH}/build/cmake/android.toolchain.cmake")

# Inclui todos os headers globais e do mod
include_directories(
    ${CMAKE_SOURCE_DIR}/aml
    ${CMAKE_SOURCE_DIR}/include
)

# Pega todos os .cpp recursivamente do mod e src
file(GLOB_RECURSE SRC_FILES
    ${CMAKE_SOURCE_DIR}/aml/mod/*.cpp
    ${CMAKE_SOURCE_DIR}/src/*.cpp
)

# Checa se encontrou algum arquivo
if(NOT SRC_FILES)
    message(FATAL_ERROR "No source files found in mod/ or src/")
endif()

# Cria a biblioteca compartilhada
add_library(MyLib SHARED ${SRC_FILES})

# Link com a lib log do Android
find_library(LOG_LIB log)
target_link_libraries(MyLib PRIVATE ${LOG_LIB})

# Muda apenas o nome do arquivo de saída
set_target_properties(MyLib PROPERTIES OUTPUT_NAME "PoliceModSZK")

add_compile_options(-Wunused-function)

# Opcional: warnings e otimizações
target_compile_options(MyLib PRIVATE -Wall -O2)