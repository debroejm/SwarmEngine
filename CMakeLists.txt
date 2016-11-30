cmake_minimum_required(VERSION 3.3)
project(SwarmEngine)

# Versioning
set(SwarmEngine_VERSION_MAJOR 1)
set(SwarmEngine_VERSION_MINOR 0)

# Library Type for the Main Engine
set (LIB_TYPE STATIC)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -static-libgcc -static-libstdc++")


###############################################################
# FIND OPENGL
###############################################################
find_package(OpenGL REQUIRED)
include_directories(${OpenGL_INCLUDE_DIRS})
link_directories(${OpenGL_LIBRARY_DIRS})
add_definitions(${OpenGL_DEFINITIONS})
if(NOT OPENGL_FOUND)
    message(ERROR " OPENGL not found!")
endif(NOT OPENGL_FOUND)


###############################################################
# ADD INCLUDES
###############################################################
include_directories("external/glew/include")
include_directories("external/glfw/include")
include_directories("external/glm")
include_directories("external/lodepng")
include_directories("external")


###############################################################
# BUILD FUNCTION
###############################################################
function(BuildEngine SystemName)
    set(CMAKE_SYSTEM_NAME ${SystemName})
    #add_subdirectory("external/lhg" out-${SystemName})
    #add_subdirectory("external/glfw" out-${SystemName})
    #add_subdirectory("engine" out-${SystemName})
    #add_subdirectory("test" out-${SystemName})
    add_subdirectory("external/glfw")
    add_subdirectory("engine")
    include_directories("engine")
    add_subdirectory("tests/generic")
    add_subdirectory("tests/model")
endfunction(BuildEngine)


BuildEngine(${CMAKE_SYSTEM_NAME})
# I'll get these working at some point...
#BuildEngine("Windows")
#BuildEngine("Linux")
#BuildEngine("Darwin")