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
        core/headers/Configuration.h
        core/headers/Animation.h
        core/headers/world/Terrain.h
        )

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
        core/source/Configuration.cpp
        core/source/Animation.cpp
        core/source/world/Terrain.cpp
        )

set(ENGINE_EXTERNAL_SOURCES
        ../external/glew/src/glew.c
        ../external/lodepng/lodepng.cpp)

# BUILD ENGINE
add_library(SwarmEngineCore STATIC ${ENGINE_HEADERS} ${ENGINE_SOURCE} ${ENGINE_EXTERNAL_SOURCES})
add_custom_command(TARGET SwarmEngineCore POST_BUILD
        COMMAND ${CMAKE_SOURCE_DIR}/external/lhg/LibHeaderGen.exe -p ${CMAKE_SOURCE_DIR}/engine/core -r -o ${CMAKE_SOURCE_DIR}/engine
        COMMAND ${CMAKE_COMMAND} -E remove $<TARGET_FILE_DIR:SwarmEngineCore>/SwarmEngine.h
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/engine/SwarmEngine.h $<TARGET_FILE_DIR:SwarmEngineCore>/SwarmEngine.h
        )
target_link_libraries(SwarmEngineCore ${OPENGL_LIBRARIES} glfw ${GLFW_LIBRARIES})