# CMAKE Version
cmake_minimum_required(VERSION 3.7)

set(PROJECT_NAME PhysicsEngine2D)

project(${PROJECT_NAME} LANGUAGES CXX)

add_custom_target(
  createConstantsHpp
  COMMAND python3 ${CMAKE_CURRENT_SOURCE_DIR}/scripts/inheritance.py
          ${PROJECT_NAME}
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/include/${PROJECT_NAME}/Shapes.hpp
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

set(SRC ${CMAKE_CURRENT_SOURCE_DIR}/src/${PROJECT_NAME}/Simulator.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/${PROJECT_NAME}/Collisions.cpp)

add_library(${PROJECT_NAME} ${SRC})

target_include_directories(${PROJECT_NAME}
                           PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
add_dependencies(${PROJECT_NAME} createConstantsHpp)

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD_REQUIRED ON)
target_compile_options(${PROJECT_NAME} PRIVATE "-Werror=multichar" -Wextra
                                               -Wall -Wunused -g -Ofast)

if(CMAKE_BUILD_TYPE STREQUAL "Profile")
  if(CMAKE_SYSTEM_NAME STREQUAL "Windowss")
    target_compile_options(${PROJECT_NAME} PRIVATE -pg)
    target_link_options(${PROJECT_NAME} PRIVATE -pg)
  endif()
endif()

add_subdirectory(example)

enable_testing()
add_subdirectory(test)
