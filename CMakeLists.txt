cmake_minimum_required(VERSION 3.23.1 FATAL_ERROR)
project(earthquake_simulator LANGUAGES CXX)
set(CMAKE_VERBOSE_MAKEFILE true)

# enable pedantic errors and set warnings to all
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -lGL -lGLU -lglut -lGLEW")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

# set standard
set(CMAKE_CXX_STANDARD 20)

# coverage, testing, and sanitizer options
include(Sanitizers.cmake)

option(ENABLE_COVERAGE, "This enables coverage information provided by Lcov" false)
if (ENABLE_COVERAGE)
	set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Set the build type." FORCE)
	include(CodeCoverage.cmake)
	APPEND_COVERAGE_COMPILER_FLAGS()
endif()

find_package(Catch2 REQUIRED)
find_package(Boost REQUIRED COMPONENTS graph)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(GLUT REQUIRED)
find_package(glfw3 REQUIRED)

include_directories(
    ${GLUT_LIB_DIR}
    ${OPENGL_INCLUDE_DIR}
    ${GLUT_INCLUDE_DIR}
)
# include_directories(${OPENGL_INCLUDE_DIR})
# include_directories(${GLEW_INCLUDE_DIRS})
# include_directories(${GLUT_INCLUDE_DIRS})
# link_libraries(${GLEW_LIBRARIES})



# executables
add_executable(earth app/earthquake.cpp)
add_executable(test_earthquake app/test_earthquake.cpp)

target_include_directories(earth PUBLIC ${Boost_INCLUDE_DIRS} include)
target_include_directories(test_earthquake PUBLIC ${Boost_INCLUDE_DIRS} include)

# target_link_libraries(${PROJECT_NAME} ${Boost_GRAPH_LIBRARY} ${GLFW_LIBRARIES} ${GLEW_LIBRARIES} ${GLUT_glut_LIBRARY})
target_link_libraries(earth Boost::graph OpenGL::GL OpenGL::GLU GLUT::GLUT GLEW::GLEW glfw)
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
