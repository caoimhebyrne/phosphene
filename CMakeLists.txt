cmake_minimum_required(VERSION 3.22)

# Use the aarch64 elf cross compiler
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY")
set(CMAKE_C_COMPILER aarch64-elf-gcc CACHE PATH "")
set(CMAKE_CXX_COMPILER aarch64-elf-g++ CACHE PATH "")
# set(CMAKE_ASM_COMPILER arch64-elf-gcc CACHE PATH "")

project(phosphene CXX ASM)

# Use C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Collect all .cpp files in the `src` directory
file(GLOB_RECURSE SOURCES src/*.cpp)
file(GLOB LINKER_SCRIPT src/linker.ld)

# Include our assembly files
set(SOURCES
    ${SOURCES}
    src/boot/boot.S
)

set_source_files_properties(${SOURCES} PROPERTIES COMPILE_FLAGS  "-mstrict-align -fno-rtti -Wno-int-to-pointer-cast -fno-threadsafe-statics -fno-exceptions")
add_executable(phosphene ${SOURCES})
target_link_options(phosphene PRIVATE LINKER:-T ${LINKER_SCRIPT} -nostdlib -nodefaultlibs)

# Use our custom linker script

# Create a kernel8.img after building
add_custom_command(
    TARGET phosphene
    POST_BUILD
    COMMAND aarch64-elf-objcopy phosphene -O binary ${CMAKE_CURRENT_BINARY_DIR}/kernel8.img
)
