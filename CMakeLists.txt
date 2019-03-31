# CMAKE Version
cmake_minimum_required(VERSION 3.7)

project(PhysicsEngine2D  LANGUAGES CXX)

add_custom_command(OUTPUT include/Constants.hpp
					COMMAND python scripts/inheritance.py
					DEPENDS include/Shapes.hpp
					WORKING_DIRECTORY ../../
)

set(SRC 
	${CMAKE_CURRENT_SOURCE_DIR}/src/Collision.cpp 
	${CMAKE_CURRENT_SOURCE_DIR}/src/drawUtil.cpp 
	${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp 
	${CMAKE_CURRENT_SOURCE_DIR}/src/Simulator.cpp)

add_executable(PhysicsEngine2D ${SRC})
target_include_directories(PhysicsEngine2D PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
# include_directories("./include")

set_property(TARGET PhysicsEngine2D PROPERTY CXX_STANDARD 17)
set_property(TARGET PhysicsEngine2D PROPERTY CXX_STANDARD_REQUIRED ON)
target_compile_options(PhysicsEngine2D PRIVATE -Wpedantic -Wextra -Wall -Wunused)

if(CMAKE_BUILD_TYPE STREQUAL "Profile")
	target_compile_options(PhysicsEngine2D PRIVATE -pg)
endif()


find_package(SFML 2.5 COMPONENTS graphics REQUIRED)
find_package(TGUI REQUIRED)

target_link_libraries(PhysicsEngine2D PRIVATE sfml-graphics tgui)
