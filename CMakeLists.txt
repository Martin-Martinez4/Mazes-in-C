# Windows: Need to copy SDL3.dll to ./build/Debug in order for the exe to run
# Shoutout to https://www.freepascal-meets-sdl.net/installation-of-sdl3-linux-and-windows/

# ==============================
# Mazes-in-C - CMake Configuration
# Cross-platform: Windows + Linux
# ==============================

cmake_minimum_required(VERSION 3.10)
project(Mazes LANGUAGES C)

# Generate compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Create an option to switch between a system sdl library and a vendored sdl library
option(VENDORED "Use vendored libraries" OFF)



# https://wiki.libsdl.org/SDL3/README/cmake
if(VENDORED)
    add_subdirectory(vendored/sdl EXCLUDE_FROM_ALL)
else()
  if(WIN32)
    set(SDL3_DIR "C:/SDL3/cmake" CACHE PATH "Path to SDL3Config.cmake")
    set(SDL3_image_DIR "C:/SDL3_image/cmake" CACHE PATH "Path to SDL3_imageConfig.cmake")
  endif()
  # 1. Look for a SDL3 package, 2. look for the SDL3 component and 3. fail if none can be found
  find_package(SDL3 CONFIG QUIET COMPONENTS SDL3)
  if(NOT SDL3_FOUND)
  # Fallback to manual include/lib detection
    find_path(SDL3_INCLUDE_DIR SDL3/SDL.h)
    find_library(SDL3_LIBRARY SDL3)
    if(NOT SDL3_INCLUDE_DIR OR NOT SDL3_LIBRARY)
        message(FATAL_ERROR "SDL3 not found! Please install SDL3 or set SDL3_DIR.")
    endif()
    set(SDL3_INCLUDE_DIRS ${SDL3_INCLUDE_DIR})
    set(SDL3_LIBRARIES ${SDL3_LIBRARY})
  endif()
  
  find_package(SDL3_image CONFIG QUIET)
  if(NOT SDL3_image_FOUND)
    find_path(SDL3_IMAGE_INCLUDE_DIR SDL3/SDL_image.h)
    find_library(SDL3_IMAGE_LIBRARY SDL3_image)
    if(NOT SDL3_IMAGE_INCLUDE_DIR OR NOT SDL3_IMAGE_LIBRARY)
        message(FATAL_ERROR "SDL3_image not found! Please install SDL3_image or set SDL3_image_DIR.")
    endif()
    set(SDL3_IMAGE_INCLUDE_DIRS ${SDL3_IMAGE_INCLUDE_DIR})
    set(SDL3_IMAGE_LIBRARIES ${SDL3_IMAGE_LIBRARY})
  endif()

endif()
    

    set(sources
    ${CMAKE_SOURCE_DIR}/src/utils/grid_utils.c
    ${CMAKE_SOURCE_DIR}/src/cell.c
    ${CMAKE_SOURCE_DIR}/src/rb_tree.c
    ${CMAKE_SOURCE_DIR}/src/sets.c
    ${CMAKE_SOURCE_DIR}/src/draw_cells.c
    ${CMAKE_SOURCE_DIR}/src/kruskals.c
    

    
    )
    
    set(app_sources
 
      ${CMAKE_SOURCE_DIR}/src/main.c
      
    )


if(WIN32)
    # Create your game executable target as usual
    # WIN32 will be built as a GUI application
    add_executable(${CMAKE_PROJECT_NAME} WIN32 ${sources} ${app_sources})
else()
  add_executable(
      ${CMAKE_PROJECT_NAME}
      ${sources} 
      ${app_sources}
    )
endif()

message("SDL3_IMAGE_INCLUDE_DIRS: ${SDL3_IMAGE_INCLUDE_DIRS}")
message("SDL3_INCLUDE_DIRS: ${SDL3_INCLUDE_DIRS}")



target_include_directories(
    ${CMAKE_PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${SDL3_INCLUDE_DIRS}
    ${SDL3_IMAGE_INCLUDE_DIRS}
)

target_link_libraries(${CMAKE_PROJECT_NAME} PUBLIC -lm ${SDL3_LIBRARIES} ${SDL3_IMAGE_LIBRARIES})

#--- Test executable ---
set(test_sources ${test_sources}
# ${CMAKE_CURRENT_SOURCE_DIR}/tests/LinearMath/Vector_tests.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/test/Tests_draw_cells.c
  ${CMAKE_CURRENT_SOURCE_DIR}/test/Tests_rb_tree.c
  ${CMAKE_CURRENT_SOURCE_DIR}/test/Tests_sets.c

)
add_executable(
    ${CMAKE_PROJECT_NAME}_test 
    ${CMAKE_SOURCE_DIR}/test/main.c
    ${sources}
    ${test_sources}
)
target_include_directories(
  ${CMAKE_PROJECT_NAME}_test
  PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/src/
  ${SDL3_IMAGE_INCLUDE_DIRS}
)

target_link_libraries(${CMAKE_PROJECT_NAME}_test PUBLIC -lm ${SDL3_LIBRARIES} ${SDL3_IMAGE_LIBRARIES})




