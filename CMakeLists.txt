cmake_minimum_required(VERSION 3.20.0)

# set(BOARD esp32s3_devkitm)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(hyundai_remote)

target_sources(app PRIVATE src/main.c)