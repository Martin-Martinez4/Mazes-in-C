# Windows: Need to copy SDL3.dll to ./build/Debug in order for the exe to run
# Shoutout to https://www.freepascal-meets-sdl.net/installation-of-sdl3-linux-and-windows/

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
    set(SDL3_DIR "C:/SDL3/cmake")
    set(SDL3_image_DIR "C:/SDL3_image/cmake")
  endif()
  # 1. Look for a SDL3 package, 2. look for the SDL3 component and 3. fail if none can be found
  find_package(SDL3 CONFIG REQUIRED COMPONENTS SDL3)
    
  # 1. Look for a SDL3 package, 2. Look for the SDL3maincomponent and 3. DO NOT fail when SDL3main is not available
  find_package(SDL3)

  
endif()
    
    find_package(SDL3_image REQUIRED)

    
    set(sources ${sources}
    #   ${CMAKE_CURRENT_SOURCE_DIR}/LinearMath/Vector.cpp
    
    )
    
    set(app_sources ${app_sources}
 
      ${CMAKE_SOURCE_DIR}/src/main.c
    )


if(WIN32)
    # Create your game executable target as usual
    # WIN32 will be built as a GUI application
    add_executable(
      ${CMAKE_PROJECT_NAME}
      WIN32
      ${sources}
      ${app_sources}

    )
else()
add_executable(
    ${CMAKE_PROJECT_NAME}
    ${sources} 
    ${app_sources}
  )
endif()

message("SDL3_IMAGE_INCLUDE_DIRS: ${SDL3_IMAGE_INCLUDE_DIRS}")
message("SDL3_INCLUDE_DIRS: ${SDL3_INCLUDE_DIRS}")



include_directories(
  ${CMAKE_PROJECT_NAME}
  PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/src/
  # ${CMAKE_CURRENT_SOURCE_DIR}/src/Physics/
  ${SDL3_INCLUDE_DIRS}?
  ${SDL3_IMAGE_INCLUDE_DIRS}?
)
  

# SDL3::SDL3main may or may not be available. It is e.g. required by Windows GUI applications
# if(TARGET SDL3::SDL3main)
#   # It has an implicit dependency on SDL3 functions, so it MUST be added before SDL3::SDL3 (or SDL3::SDL3-static)
#   target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE SDL3::SDL3main)
# endif()
    
# # Link to the actual SDL3 library. SDL3::SDL3 is the shared SDL library, SDL3::SDL3-static is the static SDL libarary.
# target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE SDL3::SDL3)
target_link_libraries(${CMAKE_PROJECT_NAME} PUBLIC -lm ${SDL3_LIBRARIES} ${SDL3_IMAGE_LIBRARIES})

# # Tests
# enable_testing()
# include(FetchContent)
# FetchContent_Declare(
# googletest
# URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
# )
# # For Windows: Prevent overriding the parent project's compiler/linker settings
# set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
# FetchContent_MakeAvailable(googletest)

# set(test_sources ${test_sources}
# # ${CMAKE_CURRENT_SOURCE_DIR}/tests/LinearMath/Vector_tests.cpp

# )

# add_executable(
#     ${CMAKE_PROJECT_NAME}_test 
#     ${CMAKE_SOURCE_DIR}/test/main_test.cpp
#     ${sources}
#     ${test_sources}
# )
# include_directories(
#   ${CMAKE_PROJECT_NAME}
#   PUBLIC
#   ${CMAKE_CURRENT_SOURCE_DIR}/src/Physics/
# )

# target_link_libraries( 
#     ${CMAKE_PROJECT_NAME}_test
#     PUBLIC
#     GTest::gtest_main
# )


# include(GoogleTest)
# gtest_discover_tests(${CMAKE_PROJECT_NAME}_test)
