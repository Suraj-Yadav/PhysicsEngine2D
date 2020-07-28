set(COMMON_HEADER ./)
set(COMMON_SRC drawUtil.cpp fontManager.cpp)

find_package(SFML 2.5 COMPONENTS graphics REQUIRED)
find_package(TGUI REQUIRED)

link_libraries(Dwrite)
#add_library(profiler SHARED IMPORTED) # or STATIC instead of SHARED
#set_target_properties(profiler PROPERTIES
#  IMPORTED_LOCATION "/usr/local/Cellar/gperftools/2.7/lib/libprofiler.a"
#)


macro(add_example target)
    set(options OPTIONAL FAST)
    set(oneValueArgs DESTINATION RENAME)
    set(multiValueArgs SOURCES RESOURCES)
    cmake_parse_arguments(EXAMPLE "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

	add_executable(${target} ${EXAMPLE_SOURCES} ${COMMON_SRC} ${EXAMPLE_RESOURCES})
    
    target_include_directories(${target} PRIVATE ${COMMON_HEADER})
    set_property(TARGET ${target} PROPERTY CXX_STANDARD 17)
    set_property(TARGET ${target} PROPERTY CXX_STANDARD_REQUIRED ON)
	target_compile_options(${target} PRIVATE "-Werror=multichar" -Wextra -Wall -Wunused -g -Ofast)
	target_link_libraries(${target} PRIVATE sfml-graphics tgui)
	target_link_libraries(${target} LINK_PUBLIC ${PROJECT_NAME})
	
	if(CMAKE_BUILD_TYPE STREQUAL "Profile")
		if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
			target_compile_options(${target} PRIVATE -pg)
			target_link_options(${target} PRIVATE -pg)
		endif()
	endif()

    # Copy resource files to build directory
    foreach(RESOURCE_FILE ${EXAMPLE_RESOURCES})
        file(COPY ${RESOURCE_FILE} DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")
    endforeach()
endmacro()

add_example(viewer 
            SOURCES viewer.cpp
            RESOURCES controller.form)

# add_executable(${PROJECT_NAME} MACOSX_BUNDLE ${SRC})
# add_custom_command(
#       TARGET ${PROJECT_NAME}
#       POST_BUILD
#       COMMAND plutil -replace NSHighResolutionCapable -bool false ${PROJECT_NAME}.app/Contents/Info.plist
#       )
