cmake_minimum_required(VERSION 3.31)
project(hook_test C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

file(GLOB_RECURSE SOURCES Src/*.*)

add_library(${PROJECT_NAME} SHARED ${SOURCES})
#add_executable(${PROJECT_NAME} ${SOURCES})

set_target_properties(${PROJECT_NAME} PROPERTIES
        PREFIX ""
        OUTPUT_NAME "hook_testing"
)

target_include_directories(${PROJECT_NAME} PRIVATE
  Src  
)
