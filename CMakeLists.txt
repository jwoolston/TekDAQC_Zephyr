cmake_minimum_required(VERSION 3.13.1)

set(BOARD_ROOT ${CMAKE_CURRENT_SOURCE_DIR})
set(BOARD tekdaqc_e)
set(ZEPHYR_EXTRA_MODULES ${CMAKE_CURRENT_SOURCE_DIR}/modules/tekdaqc) #${CMAKE_CURRENT_SOURCE_DIR}/modules/zephyr-rust)
#set(ZEPHYR_EXTRA_MODULES zephyr-rust/zephyr)

set(civetweb_dir ${CMAKE_CURRENT_SOURCE_DIR}/civetweb)
set(civetweb_src_dir ${civetweb_dir}/src)
set(civetweb_include_dir ${civetweb_dir}/include)

include_directories(${civetweb_include_dir})

# Find Zephyr. This also loads Zephyr's build system.
find_package(Zephyr REQUIRED)
project(TekDAQC_Zephyr)

target_sources(app PRIVATE
        src/main.c
        src/dhcp.h
        src/timestamp.h
        src/network.c
        src/network.h
        src/webserver.c
        src/webserver.h
        src/sntp.h
        ${civetweb_src_dir}/libc_extensions.c
        )

