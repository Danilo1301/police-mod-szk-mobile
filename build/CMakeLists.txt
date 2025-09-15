cmake_minimum_required(VERSION 3.18)
project(MenuSZK LANGUAGES C CXX)

# -------------------------------
# Configuração do NDK
# -------------------------------
set(ANDROID_NDK "C:/android-ndk")            # Caminho para seu NDK
set(ANDROID_ABI "arm64-v8a")                  # ABI alvo
set(ANDROID_PLATFORM 33)                      # API alvo
set(CMAKE_TOOLCHAIN_FILE "${ANDROID_NDK}/build/cmake/android.toolchain.cmake")

# -------------------------------
# Busca todos os arquivos de código
# -------------------------------
file(GLOB_RECURSE SRC_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/../MenuSZK/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/../MenuSZK/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../MenuSZK/mod/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/../MenuSZK/mod/*.c"
)

# Se não encontrar arquivos, cria dummy para não quebrar
if(NOT SRC_FILES)
    message(WARNING "Nenhum arquivo fonte encontrado!")
    file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/dummy.cpp" "// dummy")
    list(APPEND SRC_FILES "${CMAKE_CURRENT_BINARY_DIR}/dummy.cpp")
endif()

# -------------------------------
# Cria a biblioteca compartilhada
# -------------------------------
add_library(MenuSZK SHARED ${SRC_FILES})

# -------------------------------
# Inclui headers do projeto
# -------------------------------
target_include_directories(MenuSZK PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/../MenuSZK/mod"
    "${CMAKE_CURRENT_SOURCE_DIR}/../MenuSZK"      # Para #include <mod/...>
)

# -------------------------------
# Define padrão C/C++
# -------------------------------
set_target_properties(MenuSZK PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED YES
)

# -------------------------------
# Exporta compile_commands.json
# -------------------------------
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)