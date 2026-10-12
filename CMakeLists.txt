cmake_minimum_required(VERSION 3.23)

set(PROJECT_NAME gbdmge)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(${PROJECT_NAME} LANGUAGES CXX)

include(FetchContent)

set(SDL_REPO https://github.com/libsdl-org/SDL.git)
set(SDL_RELEASE release-3.4.16)

FetchContent_Declare(
        SDL3
        GIT_REPOSITORY ${SDL_REPO}
        GIT_TAG ${SDL_RELEASE}
        GIT_SHALLOW TRUE
        GIT_PROGRESS TRUE
)

FetchContent_MakeAvailable(SDL3)

add_executable(${PROJECT_NAME})

set(SOURCES
        src/Audio.cpp
        src/BitOps.cpp
        src/Bus.cpp
        src/Cartridge.cpp
        src/Console.cpp
        src/CPU.cpp
        src/Joypad.cpp
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

target_link_libraries(${PROJECT_NAME}
        PRIVATE
        SDL3::SDL3
)

if (WIN32)
    add_custom_command(
            TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND "${CMAKE_COMMAND}" -E copy_if_different
            "$<TARGET_FILE:SDL3::SDL3>"
            "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
            VERBATIM
    )
endif ()