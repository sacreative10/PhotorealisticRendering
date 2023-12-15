cmake_minimum_required(VERSION 3.26)
project(PhotoRealsiticRendering)

set(CMAKE_CXX_STANDARD 17)


add_subdirectory(external/Cashew)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(${PROJECT_NAME} 
  src/core/phr.h 
  src/core/geometry.h
  src/core/geometry.cpp 
  src/core/transform.h
  src/core/transform.cpp
  src/main.cpp)
target_include_directories(${PROJECT_NAME} PUBLIC
  external/Cashew/CashewLib/src/)
target_link_libraries(${PROJECT_NAME} Cashew)
