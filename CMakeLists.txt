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

option(ENABLE_COVERAGE, "This enables coverage information provided by Lcov" false)
if (ENABLE_COVERAGE)
	set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Set the build type." FORCE)
	include(CodeCoverage.cmake)
	APPEND_COVERAGE_COMPILER_FLAGS()
endif()

# executables
# TODO: change this
add_executable(test_random app/test_random.cpp)
target_link_libraries(test_random ra Catch2::Catch2)

# coverage task that runs tests
if (ENABLE_COVERAGE)
	SETUP_TARGET_FOR_COVERAGE_LCOV(
		NAME coverage
		EXECUTABLE ${CMAKE_CURRENT_BINARY_DIR}/test_random # TODO: change this
		LCOV_ARGS -rc lcov_branch_coverage=1
		GENHTML_ARGS --branch-coverage
		DEPENDENCIES test_random # TODO: change this
	)
endif()
