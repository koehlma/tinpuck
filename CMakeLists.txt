cmake_minimum_required(VERSION 3.3)

set(CMAKE_SYSTEM_NAME "Generic")

project(Tin-Puck)

set(DEVICE "30F6014A")

set(CMAKE_FIND_ROOT_PATH /opt/microchip/xc16/v1.26)
set(CMAKE_C_COMPILER /opt/microchip/xc16/v1.26/bin/xc16-gcc)

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)

set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")

set(CMAKE_C_FLAGS "-mcpu=${DEVICE} -mlarge-code -mlarge-data -O0 -fmessage-length=0")
set(CMAKE_ASM_FLAGS ${CMAKE_C_FLAGS})
set(CMAKE_EXE_LINKER_FLAGS "-Wl,--heap=128,--stack=512,--report-mem,-Map=${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CMAKE_PROJECT_NAME}.map,--script=/opt/microchip/xc16/v1.26/support/dsPIC30F/gld/p30F6014A.gld")

include_directories(include)

set(SOURCE_FILES
        src/adc.c
        src/com.c
        src/core.c
        src/leds.c
        src/motors.c
        src/rs232.c
        src/scheduler.c
        src/selector.c
        src/time.c
        src/utils.c)

add_library(${CMAKE_PROJECT_NAME} ${SOURCE_FILES})
