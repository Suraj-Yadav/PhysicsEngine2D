set(COMMON_HEADER drawUtil.hpp)
set(COMMON_SRC drawUtil.cpp)

find_package(SFML 2.5 COMPONENTS graphics REQUIRED)
find_package(TGUI REQUIRED)

macro(add_example target)
    set(options OPTIONAL FAST)
    set(oneValueArgs DESTINATION RENAME)
    set(multiValueArgs SOURCES RESOURCES)
    cmake_parse_arguments(EXAMPLE "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    add_executable(${target} ${EXAMPLE_SOURCES} ${COMMON_SRC} ${EXAMPLE_RESOURCES})
    
    target_include_directories(${target} PRIVATE ${COMMON_HEADER})
    set_property(TARGET ${target} PROPERTY CXX_STANDARD 17)
    set_property(TARGET ${target} PROPERTY CXX_STANDARD_REQUIRED ON)
    target_compile_options(${target} PRIVATE -Wpedantic -Wextra -Wall -Wunused)
    target_link_libraries(${target} PRIVATE sfml-graphics tgui)
    target_link_libraries(${target} LINK_PUBLIC ${PROJECT_NAME})

    # Copy resource files to build directory
    foreach(RESOURCE_FILE ${EXAMPLE_RESOURCES})
        file(COPY ${RESOURCE_FILE} DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")
    endforeach()
endmacro()

add_example(viewer 
            SOURCES viewer.cpp
            RESOURCES controller.form controller-hd.form)

# add_executable(${PROJECT_NAME} MACOSX_BUNDLE ${SRC})
# add_custom_command(
#       TARGET ${PROJECT_NAME}
#       POST_BUILD
#       COMMAND plutil -replace NSHighResolutionCapable -bool false ${PROJECT_NAME}.app/Contents/Info.plist
#       )
