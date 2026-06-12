cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

project(mpu6500_struct_driver)

target_sources(app PRIVATE
    src/main.c
    src/mpu6500.c
)

