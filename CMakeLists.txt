cmake_minimum_required(VERSION 3.23)

set(PROJECT_NAME gbdmge)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(${PROJECT_NAME} LANGUAGES CXX)

add_executable(${PROJECT_NAME})

set(SOURCES
    src/main.cpp
)

target_sources(${PROJECT_NAME}
    PRIVATE
    ${SOURCES}
)

target_include_directories(${PROJECT_NAME}
    PRIVATE
    include
)

