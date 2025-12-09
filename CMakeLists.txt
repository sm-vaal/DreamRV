cmake_minimum_required(VERSION 4.0)
project(DreamRV C)

set(CMAKE_C_STANDARD 11)

include(FetchContent)

# Raylib master
FetchContent_Declare(
        raylib
        GIT_REPOSITORY https://github.com/raysan5/raylib.git
        GIT_TAG master
)
FetchContent_MakeAvailable(raylib)

add_executable(DreamRV src/main.c
        src/RV32_emu/RV32_core.c
        src/RV32_emu/RV32_core.h
        src/RV32_emu/memory.c
        src/RV32_emu/memory.h
        src/debug.c
        src/RV32_emu/RV32_I.h
        src/RV32_emu/RV32_extensions/RV32M.c
        src/RV32_emu/RV32_extensions/RV32M.h
        src/RV32_emu/RV32_I.c
        src/user_interfaces/ui.c
        src/user_interfaces/ui.h
        src/user_interfaces/gfx.c
        src/user_interfaces/gfx.h
        src/user_interfaces/input.c
        src/user_interfaces/input.h
        src/command_parser.c
        src/command_parser.h
        src/user_interfaces/screen_only.c
        src/user_interfaces/screen_only.h
        src/user_interfaces/audio.c
        src/user_interfaces/audio.h
)

target_link_libraries(DreamRV PRIVATE raylib)