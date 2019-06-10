cmake_minimum_required (VERSION 3.1)

project (elevatormadness)

add_executable (game ./simulation/game.cpp)
add_executable (solution ./example/solution.cpp)

set_property(TARGET game PROPERTY CXX_STANDARD 17)
set_property(TARGET solution PROPERTY CXX_STANDARD 17)

find_package (Threads)

target_link_libraries (game ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries (solution ${CMAKE_THREAD_LIBS_INIT})

include(CheckCXXCompilerFlag)

CHECK_CXX_COMPILER_FLAG("-march=native" HAS_FLAG_MARCH_NATIVE)
if (HAS_FLAG_MARCH_NATIVE)
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -march=native")
endif()

CHECK_CXX_COMPILER_FLAG("-mtune=native" HAS_FLAG_MTUNE_NATIVE)
if (HAS_FLAG_MTUNE_NATIVE)
    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -mtune=native")
endif()

CHECK_CXX_COMPILER_FLAG("-pthread" HAS_FLAG_PTHREAD)
if (HAS_FLAG_PTHREAD)
    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -pthread")
endif()

CHECK_CXX_COMPILER_FLAG("-Wall" HAS_FLAG_WALL)
if (HAS_FLAG_WALL)
    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -pthread")
endif()

CHECK_CXX_COMPILER_FLAG("-Wextra" HAS_FLAG_WEXTRA)
if (HAS_FLAG_WEXTRA)
    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -pthread")
endif()
