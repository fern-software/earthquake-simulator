cmake_minimum_required(VERSION 3.23.1 FATAL_ERROR)
project(earthquake_simulator LANGUAGES CXX)
set(CMAKE_VERBOSE_MAKEFILE true)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

# enable pedantic errors and set warnings to all
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

# set standard
set(CMAKE_CXX_STANDARD 20)

# coverage, testing, and sanitizer options
include(Sanitizers)

option(ENABLE_COVERAGE, "This enables coverage information provided by Lcov" false)
if (ENABLE_COVERAGE)
	set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Set the build type." FORCE)
	include(CodeCoverage)
	APPEND_COVERAGE_COMPILER_FLAGS()
endif()

find_package(Catch2 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(glfw3 REQUIRED)
find_package(Cairo REQUIRED)
find_package(PkgConfig REQUIRED)
find_package(Pango REQUIRED)
pkg_check_modules(GLIB REQUIRED glib-2.0)

pkg_check_modules(GTK2 REQUIRED gtk+-2.0)

include_directories(${OPENGL_INCLUDE_DIR})


# executables
add_executable(earth app/earthquake.cpp)
add_executable(test_earthquake app/test_earthquake.cpp)

target_include_directories(earth PUBLIC include ${Pango_INCLUDE_DIR} ${GLIB_INCLUDE_DIRS} ${CAIRO_INCLUDE_DIRS})
target_include_directories(test_earthquake PUBLIC include)


target_link_libraries(earth OpenGL::GL OpenGL::GLU GLEW::GLEW glfw ${CAIRO_LIBRARIES} ${GTK2_LIBRARIES} ${GLIB_LIBRARIES} ${Pango_LIBRARY})
target_link_libraries(test_earthquake PUBLIC Catch2::Catch2)



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
