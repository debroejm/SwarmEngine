# CMake file for the Main Engine

set(ENGINE_HEADERS
    #../external/glew/include/GL/glew.h
    SwarmEngine.h
    core/Definitions.h
    core/headers/Initialization.h
    core/headers/Logging.h
    core/headers/Shaders.h
    core/headers/Controls.h
    core/headers/Rendering.h
    core/headers/TextureHandler.h
    core/headers/ModelLoading.h
    core/headers/AnimatedTexture.h
    core/headers/GLFWCallbacks.h
    core/headers/Configuration.h)

set(ENGINE_SOURCE
        #../external/glew/src/glew.c
        core/source/AnimatedTexture.cpp
        core/source/Controls.cpp
        core/source/GLFWCallbacks.cpp
        core/source/Initialization.cpp
        core/source/Logging.cpp
        core/source/ModelLoading.cpp
        core/source/Rendering.cpp
        core/source/Shaders.cpp
        core/source/TextureHandler.cpp
        core/source/Configuration.cpp)

set(EXTERNAL_SOURCES
        ../external/glew/src/glew.c
        ../external/lodepng/lodepng.cpp)

add_library(SwarmEngineCore ${LIB_TYPE} ${ENGINE_HEADERS} ${ENGINE_SOURCE} ${EXTERNAL_SOURCES})
#add_executable(SwarmEngineCore ${ENGINE_HEADERS} ${ENGINE_SOURCE})
target_link_libraries(SwarmEngineCore ${OPENGL_LIBRARIES} ${ENGINE_LIBRARIES})