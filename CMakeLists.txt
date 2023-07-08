cmake_minimum_required(VERSION 3.22.0)
project(INI_Parser)

# Установка исходных файлов
set(SOURCE_FILES
    source/main.cpp
    source/ini_parser/ini_parser.cpp
)

# Установка заголовочных файлов
set(HEADER_FILES
    source/ini_parser/ini_parser.h
)

# Добавление исполняемого файла
add_executable(INI_Parser ${SOURCE_FILES} ${HEADER_FILES})

# Добавление директории с заголовочными файлами для поиска
target_include_directories(INI_Parser PUBLIC source)

# Добавление директории с ini-файлами в собираемый проект
set(INI_DIR "${CMAKE_CURRENT_SOURCE_DIR}/source/ini_files")
target_compile_definitions(INI_Parser PRIVATE INI_DIR="${INI_DIR}")
