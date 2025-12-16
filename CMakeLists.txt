# =============================================
# Mazes-in-C - CMake Configuration
# Cross-platform: Windows + Linux
# =============================================

cmake_minimum_required(VERSION 3.10)
project(Mazes LANGUAGES C)

# Generate compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Option to switch between system and vendored SDL libraries
option(VENDORED "Use vendored libraries" OFF)

# ----------------------
# SDL Setup
# ----------------------
if(VENDORED)
    add_subdirectory(vendored/sdl EXCLUDE_FROM_ALL)
else()
    if(WIN32)
        # Paths to SDL3, SDL3_image, SDL3_ttf CMake config directories
        set(SDL3_DIR "C:/SDL3/cmake" CACHE PATH "Path to SDL3Config.cmake")
        set(SDL3_image_DIR "C:/SDL3_image/cmake" CACHE PATH "Path to SDL3_imageConfig.cmake")
        set(SDL3_TTF_DIR "C:/SDL3_ttf/cmake" CACHE PATH "Path to SDL3_ttfConfig.cmake")
    endif()

    # Find SDL3, SDL3_image, SDL3_ttf packages
    find_package(SDL3 CONFIG REQUIRED COMPONENTS SDL3)
    find_package(SDL3_image CONFIG REQUIRED)
    find_package(SDL3_ttf CONFIG REQUIRED)
endif()

# ----------------------
# Clang-format target
# ----------------------
find_program(CLANG_FORMAT_EXECUTABLE clang-format)
if(CLANG_FORMAT_EXECUTABLE)
    message(STATUS "clang-format found: ${CLANG_FORMAT_EXECUTABLE}")
    file(GLOB_RECURSE ALL_C_FILES
        "${CMAKE_SOURCE_DIR}/src/*.c"
        "${CMAKE_SOURCE_DIR}/src/*.h"
        "${CMAKE_SOURCE_DIR}/include/*.h"
        "${CMAKE_SOURCE_DIR}/test/*.c"
        "${CMAKE_SOURCE_DIR}/test/*.h"
    )
    add_custom_target(format
        COMMAND ${CLANG_FORMAT_EXECUTABLE} -i -style=file ${ALL_C_FILES}
        COMMENT "Formatting all C and H files with clang-format"
        VERBATIM
    )
else()
    message(WARNING "clang-format not found! 'format' target will be unavailable.")
endif()

# ----------------------
# Source files
# ----------------------
set(sources
    ${CMAKE_SOURCE_DIR}/src/utils/grid_utils.c
    ${CMAKE_SOURCE_DIR}/src/utils/rand_utils.c
    ${CMAKE_SOURCE_DIR}/src/aabb.c
    ${CMAKE_SOURCE_DIR}/src/rooms_bvh.c
    ${CMAKE_SOURCE_DIR}/src/rooms.c
    ${CMAKE_SOURCE_DIR}/src/maze_gen/backtracking.c
    ${CMAKE_SOURCE_DIR}/src/maze_gen/cell.c
    ${CMAKE_SOURCE_DIR}/src/utils/rb_tree.c
    ${CMAKE_SOURCE_DIR}/src/utils/sets.c
    ${CMAKE_SOURCE_DIR}/src/draw_cells.c
    ${CMAKE_SOURCE_DIR}/src/maze_gen/kruskals.c
    ${CMAKE_SOURCE_DIR}/src/maze_gen/prims.c
)

set(app_sources
    ${CMAKE_SOURCE_DIR}/src/main.c
    # ${CMAKE_SOURCE_DIR}/src/ui/clay_renderer_SDL3.c
)

# ----------------------
# Executable target
# ----------------------
if(WIN32)
    add_executable(${CMAKE_PROJECT_NAME} WIN32 ${sources} ${app_sources})
else()
    add_executable(${CMAKE_PROJECT_NAME} ${sources} ${app_sources})
endif()

# Include directories
set(sources_directories
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ui
    ${CMAKE_CURRENT_SOURCE_DIR}/src/utils
    ${CMAKE_CURRENT_SOURCE_DIR}/src/maze_gen
)

target_include_directories(
    ${CMAKE_PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/external/nuklear
    ${sources_directories}
)

# Link SDL3 libraries
# target_link_libraries(
#     ${CMAKE_PROJECT_NAME} PUBLIC
#     SDL3::SDL3
#     SDL3_image::SDL3_image
#     SDL3_ttf::SDL3_ttf
#     m
# )

if(WIN32)
    target_link_libraries(${CMAKE_PROJECT_NAME} PUBLIC SDL3::SDL3 SDL3_image::SDL3_image SDL3_ttf::SDL3_ttf)
else()
    target_link_libraries(${CMAKE_PROJECT_NAME} PUBLIC SDL3::SDL3 SDL3_image::SDL3_image SDL3_ttf::SDL3_ttf m)
endif()

# ----------------------
# Test executable
# ----------------------
set(test_sources
    ${CMAKE_CURRENT_SOURCE_DIR}/test/Tests_draw_cells.c
    ${CMAKE_CURRENT_SOURCE_DIR}/test/Tests_aabb.c
    ${CMAKE_CURRENT_SOURCE_DIR}/test/Tests_rb_tree.c
    ${CMAKE_CURRENT_SOURCE_DIR}/test/Tests_sets.c
    ${CMAKE_CURRENT_SOURCE_DIR}/test/utils/Tests_grid_utils.c
)

add_executable(
    ${CMAKE_PROJECT_NAME}_test
    ${CMAKE_SOURCE_DIR}/test/main.c
    ${sources}
    ${test_sources}
)

target_include_directories(
    ${CMAKE_PROJECT_NAME}_test PUBLIC
    ${sources_directories}
    ${CMAKE_CURRENT_SOURCE_DIR}/test/myAssert
    ${CMAKE_CURRENT_SOURCE_DIR}/test/utils
)

if(WIN32)
    target_link_libraries(${CMAKE_PROJECT_NAME}_test PUBLIC SDL3::SDL3 SDL3_image::SDL3_image SDL3_ttf::SDL3_ttf)
else()
    target_link_libraries(${CMAKE_PROJECT_NAME}_test PUBLIC SDL3::SDL3 SDL3_image::SDL3_image SDL3_ttf::SDL3_ttf m)
endif()
