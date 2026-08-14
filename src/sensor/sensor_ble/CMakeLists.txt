cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

project(sensor_ble)

file(GLOB_RECURSE SOURCE CONFIGGURE_DEPENDS "src/*.c")

target_sources(app PRIVATE ${SOURCE})
