cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED)
project(led_sensor_shell_app)

add_subdirectory(drivers/led_sensor)
target_sources(app PRIVATE src/main.c)