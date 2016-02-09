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
# ADD GLEW
###############################################################
include_directories("external/glew/include")

###############################################################
# ADD GLFW
###############################################################
add_subdirectory("external/glfw")
include_directories("external/glfw/include")

###############################################################
# ADD GLM
###############################################################
include_directories("external/glm")

###############################################################
# ADD MISC
###############################################################
include_directories("external/lodepng")

set(ENGINE_LIBRARIES
        glfw
        ${GLFW_LIBRARIES}
)

add_subdirectory(engine)

set(SOURCE_FILES
        main.cpp
        engine/SwarmEngine.h)
add_executable(SwarmEngineTest ${SOURCE_FILES})
target_link_libraries(SwarmEngineTest ${OPENGL_LIBRARIES} ${ENGINE_LIBRARIES} SwarmEngineCore)
