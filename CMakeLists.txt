cmake_minimum_required(VERSION 3.23.1 FATAL_ERROR)
project(earthquake_simulator LANGUAGES CXX)
set(CMAKE_VERBOSE_MAKEFILE true)

# enable pedantic errors and set warnings to all
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic-errors")

# set standard
set(CMAKE_CXX_STANDARD 20)

# coverage, testing, and sanitizer options
include(Sanitizers.cmake)

find_package(Catch2 REQUIRED)
find_package(Boost REQUIRED COMPONENTS graph)

option(ENABLE_COVERAGE, "This enables coverage information provided by Lcov" false)
if (ENABLE_COVERAGE)
	set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Set the build type." FORCE)
	include(CodeCoverage.cmake)
	APPEND_COVERAGE_COMPILER_FLAGS()
endif()

# executables
add_executable(earthquake app/earthquake.cpp)
add_executable(test_earthquake app/test_earthquake.cpp)

target_include_directories(earthquake PUBLIC ${Boost_INCLUDE_DIRS})
target_include_directories(test_earthquake PUBLIC ${Boost_INCLUDE_DIRS})

target_link_libraries(earthquake PUBLIC ${Boost_GRAPH_LIBRARY})
target_link_libraries(earthquake PUBLIC ${GLFW_LIBRARIES})
# target_link_libraries(test_earthquake PUBLIC ${Boost_GRAPH_LIBRARY})
target_link_libraries(test_earthquake earthquake_simulator Catch2::Catch2)

target_include_directories(earthquake PUBLIC include)
target_include_directories(test_earthquake PUBLIC include)

# coverage task that runs tests
if (ENABLE_COVERAGE)
	SETUP_TARGET_FOR_COVERAGE_LCOV(
		NAME coverage
		EXECUTABLE ${CMAKE_CURRENT_BINARY_DIR}/test_earthquake
		LCOV_ARGS -rc lcov_branch_coverage=1
		GENHTML_ARGS --branch-coverage
		DEPENDENCIES test_earthquake
	)
endif()
