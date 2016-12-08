cmake_minimum_required(VERSION 3.3)

project(SwarmEngineAll)

# Library Type for the Main Engine
set (LIB_TYPE STATIC)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -static-libgcc -static-libstdc++")

option(SWARM_BUILD_TESTS "Build the Swarm test programs" ON)

set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

###############################################################
# ADD OPENGL
###############################################################
find_package(OpenGL REQUIRED)
include_directories(${OpenGL_INCLUDE_DIRS})
link_directories(${OpenGL_LIBRARY_DIRS})
add_definitions(${OpenGL_DEFINITIONS})
if(NOT OPENGL_FOUND)
    message(ERROR " OPENGL not found!")
endif(NOT OPENGL_FOUND)


###############################################################
# ADD GLFW
###############################################################
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(external/glfw)
include_directories(external/glfw/include)


###############################################################
# ADD GLFW
###############################################################
include_directories(external/glm)


###############################################################
# ADD OPENCL
###############################################################
include_directories(external/CL)
file(COPY ${CMAKE_SOURCE_DIR}/external/CL DESTINATION ${CMAKE_SOURCE_DIR}/external/CL-IDC-Loader/inc)
add_subdirectory(external/CL-IDC-Loader)


###############################################################
# ADD MISC
###############################################################
include_directories(external/glew/include)
include_directories(external/lodepng)
include_directories(external)


set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)


###############################################################
# BUILD ENGINE
###############################################################
add_subdirectory(engine)


###############################################################
# BUILD TESTS
###############################################################
if(SWARM_BUILD_TESTS)
    include_directories(engine)
    add_subdirectory(tests/generic)
    add_subdirectory(tests/model)
    add_subdirectory(tests/CL)
endif()