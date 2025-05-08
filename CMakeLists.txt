cmake_minimum_required(VERSION 3.31)
project(vector_test C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

file(GLOB_RECURSE SOURCES Src/*.*)

#add_library(${PROJECT_NAME} SHARED ${SOURCES})
add_executable(${PROJECT_NAME} ${SOURCES})

set_target_properties(${PROJECT_NAME} PROPERTIES
        PREFIX ""
        OUTPUT_NAME "vector_test"
)

target_include_directories(${PROJECT_NAME} PRIVATE
  Src/Vector/
  Src/Player/
  Src/Hashmap/
  Src/Type/
)
