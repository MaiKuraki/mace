set(CMAKE_SYSTEM_PROCESSOR arm)

set(CMAKE_C_COMPILER "${GCC_ARM_ROOT}/arm-none-eabi-gcc")
set(CMAKE_CXX_COMPILER "${GCC_ARM_ROOT}/arm-none-eabi-g++")
set(CMAKE_AR "${GCC_ARM_ROOT}/arm-none-eabi-ar" CACHE FILEPATH "Archiver")
set(CMAKE_LINKER "${GCC_ARM_ROOT}/arm-none-eabi-ld")
set(CMAKE_EXE_LINKER_FLAGS "--specs=nosys.specs" CACHE INTERNAL "")

set(MACE_MICRO_ARM_NONE ON)

set(CMAKE_FIND_ROOT_PATH "${GCC_ARM_ROOT}")
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)