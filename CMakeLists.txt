cmake_minimum_required (VERSION 2.6)
project (gbe)

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

include_directories(include)
add_library(lcd_controller.o lcd_controller.c)
add_library(gbe.o gbe.c)
add_executable(gbe main.c)
target_link_libraries(gbe ${SDL2_LIBRARIES} gbe.o lcd_controller.o)