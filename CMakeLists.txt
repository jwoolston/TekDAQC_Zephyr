cmake_minimum_required(VERSION 3.13.1)

set(BOARD_ROOT ${CMAKE_CURRENT_SOURCE_DIR})
set(BOARD tekdaqc_e)

# Find Zephyr. This also loads Zephyr's build system.
find_package(Zephyr REQUIRED)
project(TekDAQC_Zephyr)

target_sources(app PRIVATE
        src/main.c
        src/dhcp.h
        src/network.h
        src/mdns_service.h)
