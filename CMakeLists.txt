# CMAKE Version
cmake_minimum_required(VERSION 3.7)

SET(CMAKE_CXX_FLAGS_PROFILE "${CMAKE_CXX_FLAGS_RELEASE} -pg")

SET(CMAKE_C_FLAGS_PROFILE "${CMAKE_C_FLAGS_RELEASE} -pg")
	
SET(CMAKE_EXE_LINKER_FLAGS_PROFILE "${CMAKE_EXE_LINKER_FLAGS_RELEASE}")
	
SET(CMAKE_SHARED_LINKER_FLAGS_PROFILE "${CMAKE_SHARED_LINKER_FLAGS_RELEASE}")

MARK_AS_ADVANCED(
	CMAKE_CXX_FLAGS_PROFILE
	CMAKE_C_FLAGS_PROFILE
	CMAKE_EXE_LINKER_FLAGS_PROFILE
	CMAKE_SHARED_LINKER_FLAGS_PROFILE )

# Define sources and executable

add_custom_command(OUTPUT include/Constants.hpp
					COMMAND python scripts/inheritance.py
					DEPENDS include/Shapes.hpp
					WORKING_DIRECTORY ../../
)

add_custom_target(run ALL DEPENDS include/Constants.hpp)


file(GLOB SRCS "src/*.cpp")
file(GLOB HDRS "include/*.hpp")

set(EXECUTABLE_NAME "PhysicsEngine2D")
project (${EXECUTABLE_NAME})
add_executable(${EXECUTABLE_NAME} ${SRCS} ${HDRS})
include_directories("./include")
set_property(TARGET ${EXECUTABLE_NAME} PROPERTY CXX_STANDARD 17)
set_property(TARGET ${EXECUTABLE_NAME} PROPERTY CXX_STANDARD_REQUIRED ON)
target_compile_options(${EXECUTABLE_NAME} PRIVATE "-Wpedantic")
target_compile_options(${EXECUTABLE_NAME} PRIVATE "-Wextra")
target_compile_options(${EXECUTABLE_NAME} PRIVATE "-Wall")
target_compile_options(${EXECUTABLE_NAME} PRIVATE "-Wunused")

# Detect and add SFML
set(CMAKE_MODULE_PATH "C:/ExtLibs/SFML-2.4.2/cmake/Modules" ${CMAKE_MODULE_PATH})
set(CMAKE_MODULE_PATH "C:/ExtLibs/TGUI-0.8-dev/cmake/Modules" ${CMAKE_MODULE_PATH})
#Find any version 2.X of SFML
#See the FindSFML.cmake file for additional details and instructions
find_package(SFML 2 REQUIRED network audio graphics window system)
find_package(TGUI REQUIRED)
if(SFML_FOUND AND TGUI_FOUND)
	include_directories(${SFML_INCLUDE_DIR} ${TGUI_INCLUDE_DIR})
	target_link_libraries(${EXECUTABLE_NAME} ${SFML_LIBRARIES} ${TGUI_LIBRARY})
endif()