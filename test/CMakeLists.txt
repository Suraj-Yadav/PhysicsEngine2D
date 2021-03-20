# ##############################################################################
# Setup
# ##############################################################################
include(FetchContent)

# doctest
FetchContent_Declare(
  doctest
  GIT_REPOSITORY https://github.com/onqtam/doctest
  GIT_TAG master)
FetchContent_MakeAvailable(doctest)
include(${doctest_SOURCE_DIR}/scripts/cmake/doctest.cmake)

# google/benchmark
FetchContent_Declare(
  googlebenchmark
  GIT_REPOSITORY https://github.com/google/benchmark
  GIT_TAG master)

# Can't Simply use FetchContent_MakeAvailable because we dont want to add all
# targets from benchmark in our build, set type of build as release for
# google/benchmark only and we need to pass BENCHMARK_ENABLE_GTEST_TESTS option
FetchContent_GetProperties(googlebenchmark)
if(NOT googlebenchmark_POPULATED)
  FetchContent_Populate(googlebenchmark)
  set(BENCHMARK_ENABLE_GTEST_TESTS
      OFF
      CACHE INTERNAL "Disable Tests for benchmark library")
  # set(CMAKE_BUILD_TYPE_ORIG "${CMAKE_BUILD_TYPE}") set(CMAKE_BUILD_TYPE
  # Release CACHE INTERNAL "Build Type")
  add_subdirectory(${googlebenchmark_SOURCE_DIR} ${googlebenchmark_BINARY_DIR}
                   EXCLUDE_FROM_ALL)
  # set(CMAKE_BUILD_TYPE "${CMAKE_BUILD_TYPE_ORIG}")
endif()

FetchContent_MakeAvailable(googlebenchmark)

# ##############################################################################
# Benchmarks
# ##############################################################################
message("${CMAKE_BUILD_TYPE}")
file(GLOB_RECURSE BENCHMARK_FILES CONFIGURE_DEPENDS "*_benchmark.cpp")
add_executable(Benchmarks ${BENCHMARK_FILES} TestUtil.cpp)
if(CMAKE_BUILD_TYPE STREQUAL "Profile")
  target_compile_options(Benchmarks PRIVATE -pg -Ofast)
  target_link_options(Benchmarks PRIVATE -pg)
endif()
set_property(TARGET Benchmarks PROPERTY CXX_STANDARD 17)
set_property(TARGET Benchmarks PROPERTY CXX_STANDARD_REQUIRED ON)
target_link_libraries(Benchmarks PRIVATE benchmark::benchmark)
target_link_libraries(Benchmarks LINK_PUBLIC ${PROJECT_NAME})

# ##############################################################################
# Tests
# ##############################################################################
file(GLOB_RECURSE TEST_FILES CONFIGURE_DEPENDS "*_test.cpp")
add_executable(Tests ${TEST_FILES} TestUtil.cpp)
set_property(TARGET Tests PROPERTY CXX_STANDARD 17)
set_property(TARGET Tests PROPERTY CXX_STANDARD_REQUIRED ON)
target_include_directories(Tests PUBLIC ${doctest_SOURCE_DIR}/doctest)
target_link_libraries(Tests LINK_PUBLIC ${PROJECT_NAME})
doctest_discover_tests(Tests)
