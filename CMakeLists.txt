cmake_minimum_required(VERSION 3.26)
project(PhotoRealsiticRendering)

set(CMAKE_CXX_STANDARD 17)


add_subdirectory(external/Cashew)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# make warnings errors
if (MSVC)
    add_compile_options(/W4 /WX)
else ()
    add_compile_options(-Wall -Wextra -pedantic)
endif ()

add_executable(${PROJECT_NAME}
        src/core/util/MemoryArena.h
        src/core/util/MemoryArena.cpp
        src/core/AllocAligned.h
        src/core/AllocAligned.cpp
        src/core/phr.h
        src/core/geometry.h
        src/core/geometry.cpp
        src/core/transform.h
        src/core/transform.cpp
        src/core/interaction.h
        src/core/interaction.cpp
        src/core/shape.h
        src/core/shape.cpp
        src/shapes/sphere.h
        src/shapes/sphere.cpp
        src/core/Efloat.h
        src/core/Efloat.cpp
        src/core/primitive.h
        src/core/primitive.cpp
        src/accelerators/bvh.h
        src/accelerators/bvh.cpp
        src/core/spectrums/coefficientSpectrum.h
        src/core/spectrums/coefficientSpectrum.cpp
        src/main.cpp
        src/core/spectrums/spectrum.h
)

target_include_directories(${PROJECT_NAME} PRIVATE src/)

target_include_directories(${PROJECT_NAME} PUBLIC
        external/Cashew/CashewLib/src/)
target_link_libraries(${PROJECT_NAME} Cashew)
