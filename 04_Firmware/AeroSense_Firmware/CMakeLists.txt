cmake_minimum_required(VERSION 3.22)

#
# This file is generated only once,
# and is not re-generated if converter is called multiple times.
#
# User is free to modify the file as much as necessary
#

# Setup compiler settings
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)


# Define the build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

# Set the project name
set(CMAKE_PROJECT_NAME AeroSense_Firmware)

# Enable compile command to ease indexing with e.g. clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# Core project settings
project(${CMAKE_PROJECT_NAME})
message("Build type: " ${CMAKE_BUILD_TYPE})

# Enable CMake support for ASM and C languages
enable_language(C ASM)

# Create an executable object type
add_executable(${CMAKE_PROJECT_NAME})

# Add STM32CubeMX generated sources
add_subdirectory(cmake/stm32cubemx)

# Link directories setup
target_link_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined library search paths
)

# Add sources to executable
target_sources(${CMAKE_PROJECT_NAME} PRIVATE
    # AeroSense Application
    AeroSense/App/system_manager.c

    # AeroSense Services
    AeroSense/Services/measurement_manager.c
    AeroSense/Services/diagnostic_manager.c
    AeroSense/Services/can_message_manager.c
    AeroSense/Services/scheduler.c

    # AeroSense Device Drivers
    AeroSense/DeviceDrivers/status_led.c
    AeroSense/DeviceDrivers/stts22h.c
    AeroSense/DeviceDrivers/lps22hh.c
    AeroSense/DeviceDrivers/lis2dw12.c
    AeroSense/DeviceDrivers/vin_monitor.c
    AeroSense/DeviceDrivers/can_driver.c
    AeroSense/DeviceDrivers/watchdog.c
)

# Add include paths
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    AeroSense/App
    AeroSense/Services
    AeroSense/DeviceDrivers
)

# Add project symbols (macros)
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined symbols
)

# Remove wrong libob.a library dependency when using cpp files
list(REMOVE_ITEM CMAKE_C_IMPLICIT_LINK_LIBRARIES ob)

# Add linked libraries
target_link_libraries(${CMAKE_PROJECT_NAME}
    stm32cubemx

    # Add user defined libraries
)
