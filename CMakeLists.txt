cmake_minimum_required(VERSION 3.11)
project(potator)

set(CMAKE_CXX_STANDARD 11)

find_package(sdl REQUIRED)

include_directories(src ${SDL_INCLUDE_DIR})


add_executable(potator
        src/m6502/codes.h
        src/m6502/m6502.c
        src/m6502/m6502.h
        src/m6502/tables.h
        src/controls.c
        src/controls.h
        src/gpu.c
        src/gpu.h
        src/interrupts.c
        src/interrupts.h
        src/io.c
        src/io.h
        src/log.c
        src/log.h
        src/memory.c
        src/memory.h
        src/memorymap.c
        src/memorymap.h
        src/sound.c
        src/sound.h
        src/supervision.h
        src/timer.c
        src/timer.h
        src/types.h
        src/version.c
        src/version.h
        src/watara.c
        src/main.c
)


target_link_libraries(potator ${SDL_LIBRARY})