set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_VERSION 1)
set(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY")
cmake_minimum_required(VERSION 3.20)

set(PREFIX "arm-none-eabi-")

if (WIN32)
    MESSAGE(STATUS "Now is windows!")
    set(CMAKE_C_COMPILER ${PREFIX}gcc.exe)
    set(CMAKE_CXX_COMPILER ${PREFIX}g++.exe)
    set(CMAKE_ASM_COMPILER ${PREFIX}gcc.exe)
    set(CMAKE_AR ${PREFIX}ar.exe)
    set(CMAKE_OBJCOPY ${PREFIX}objcopy.exe)
    set(CMAKE_OBJDUMP ${PREFIX}objdump.exe)
    set(SIZE ${PREFIX}size.exe)
elseif (UNIX)
    MESSAGE(STATUS "Now is UNIX-like OS!")
    set(CMAKE_C_COMPILER ${PREFIX}gcc)
    set(CMAKE_CXX_COMPILER ${PREFIX}g++)
    set(CMAKE_ASM_COMPILER ${PREFIX}gcc)
    set(CMAKE_AR ${PREFIX}ar)
    set(CMAKE_OBJCOPY ${PREFIX}objcopy)
    set(CMAKE_OBJDUMP ${PREFIX}objdump)
    set(SIZE ${PREFIX}size)
else ()
    MESSAGE(STATUS "Unsupported system!")
endif ()

project(g4-foc C CXX ASM)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 11)

add_compile_options(
        -mcpu=cortex-m4
        -mfpu=fpv4-sp-d16
        -mfloat-abi=hard
        -mthumb
)

add_compile_options(
        -ffunction-sections
        -fdata-sections
        -funsigned-char
        -fno-common
        -ffast-math
        --specs=nano.specs
        --specs=nosys.specs
        -MMD
)

if ("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
    message(STATUS "Maximum optimization for speed")
    add_compile_options(-Ofast -flto)
    add_link_options(-flto)
elseif ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    message(STATUS "Debug mode")
    add_compile_options(-Og -g)
else ()
    message(STATUS "Minimal optimization, debug info included")
    add_compile_options(-Og -g)
endif ()

add_compile_options(-Wall)

include_directories(
        Core/Inc

        Drivers/Bsp
        Drivers/CMSIS/Include
        Drivers/CMSIS/Device/ST/STM32G4xx/Include
        Drivers/CMSIS/DSP/Include
        Drivers/CMSIS/DSP/PrivateInclude
        Drivers/STM32G4xx_HAL_Driver/Inc

        Middleware/FreeRTOS/include
        Middleware/FreeRTOS/portable/GCC/ARM_CM4F

        User/Application
)

add_definitions(
        -DSTM32G431xx
        -DUSE_HAL_DRIVER
        -DUSE_FULL_LL_DRIVER
)

file(GLOB_RECURSE SOURCES
        "Core/Src/*.c"
        "Core/Startup/startup_stm32g431cbux.s"

        "Drivers/Bsp/*.c"
        "Drivers/CMSIS/DSP/Source/BasicMathFunctions/BasicMathFunctions.c"
        "Drivers/CMSIS/DSP/Source/ControllerFunctions/ControllerFunctions.c"
        "Drivers/STM32G4xx_HAL_Driver/Src/*.c"

        "Middleware/FreeRTOS/*.c"
        "User/Application/*.c"
)

set(LINKER_SCRIPT ${CMAKE_SOURCE_DIR}/STM32G431CBUX_FLASH.ld)

add_link_options(
        -mcpu=cortex-m4
        -mfpu=fpv4-sp-d16
        -mfloat-abi=hard
        -mthumb
        -Wl,--gc-sections
        -Wl,--print-memory-usage
        --specs=nano.specs
        --specs=nosys.specs
        -Wl,-Map=${PROJECT_NAME}.map
)

add_link_options(-T ${LINKER_SCRIPT})

add_executable(${PROJECT_NAME}.elf ${SOURCES} ${LINKER_SCRIPT})

target_link_libraries(${PROJECT_NAME}.elf)

set(HEX_FILE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.hex)
set(BIN_FILE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.bin)
set(LST_FILE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.lst)

add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD
        COMMAND ${CMAKE_OBJCOPY} -Oihex $<TARGET_FILE:${PROJECT_NAME}.elf> ${HEX_FILE}
        COMMAND ${CMAKE_OBJCOPY} -Obinary $<TARGET_FILE:${PROJECT_NAME}.elf> ${BIN_FILE}
        COMMAND ${CMAKE_OBJDUMP} --all-headers --demangle --disassemble $<TARGET_FILE:${PROJECT_NAME}.elf> > ${LST_FILE}
        COMMENT "Building ${HEX_FILE}\nBuilding ${BIN_FILE}"
)