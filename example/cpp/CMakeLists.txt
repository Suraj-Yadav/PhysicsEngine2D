set(COMMON_HEADER ./)
set(COMMON_SRC drawUtil.cpp fontManager.cpp)

include(../../scripts/common.cmake)
get_external(glfw https://github.com/glfw/glfw)

get_external(imgui https://github.com/ocornut/imgui
  DOES_NOT_HAVE_CMAKE
  SOURCES imgui.cpp imgui_widgets.cpp imgui_draw.cpp imgui_tables.cpp backends/imgui_impl_glfw.cpp backends/imgui_impl_opengl3.cpp
  LIBS glfw
  GIT_TAG docking
)
find_package(OpenGL REQUIRED)

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
  link_libraries(Dwrite)
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
  link_libraries(fontconfig)
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
  add_library(profiler SHARED IMPORTED) # or STATIC instead of SHARED
  set_target_properties(
    profiler PROPERTIES IMPORTED_LOCATION
    "/usr/local/Cellar/gperftools/2.7/lib/libprofiler.a")
endif()

macro(add_example target)
  set(options OPTIONAL FAST)
  set(oneValueArgs DESTINATION RENAME)
  set(multiValueArgs SOURCES)
  cmake_parse_arguments(EXAMPLE "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

  add_executable(${target} ${EXAMPLE_SOURCES} ${COMMON_SRC}
    ${EXAMPLE_RESOURCES})

  target_include_directories(${target} PRIVATE ${COMMON_HEADER} ${OPENGL_INCLUDE_DIRS})
  set_property(TARGET ${target} PROPERTY CXX_STANDARD 17)
  set_property(TARGET ${target} PROPERTY CXX_STANDARD_REQUIRED ON)
  target_compile_options(${target} PRIVATE "-Werror=multichar" -Wextra -Wall -Wunused -g -Ofast)
  target_link_libraries(${target} PRIVATE glfw imgui ${OPENGL_LIBRARIES})
  target_link_libraries(${target} LINK_PUBLIC ${PROJECT_NAME})

  if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    target_link_libraries(${target} PRIVATE profiler)
  endif()

  if(CMAKE_BUILD_TYPE STREQUAL "Profile")
    if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
      target_compile_options(${target} PRIVATE -pg)
      target_link_options(${target} PRIVATE -pg)
    endif()
  endif()
endmacro()

add_example(viewer SOURCES viewer.cpp)

# add_executable(${PROJECT_NAME} MACOSX_BUNDLE ${SRC}) add_custom_command(
# TARGET ${PROJECT_NAME} POST_BUILD COMMAND plutil -replace
# NSHighResolutionCapable -bool false ${PROJECT_NAME}.app/Contents/Info.plist )
