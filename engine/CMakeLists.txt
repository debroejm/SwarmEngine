# CMake file for the Main Engine

set(ENGINE_HEADERS_CORE
        #SwarmEngine.h
        core/Definitions.h
        core/GLFWCallbacks.h
        core/Initialization.h
        core/Logging.h
        core/config/Configuration.h
        core/config/Controls.h
        core/model/Animation.h
        core/model/ModelLoading.h
        core/render/Rendering.h
        core/render/Shaders.h
        core/render/Camera.h
        core/texture/AnimatedTexture.h
        core/texture/TextureHandler.h
        core/texture/AnimatedTexture.h
        )

set(ENGINE_SOURCE_CORE
        core/GLFWCallbacks.cpp
        core/Initialization.cpp
        core/Logging.cpp
        core/config/Configuration.cpp
        core/config/Controls.cpp
        core/model/Animation.cpp
        core/model/ModelLoading.cpp
        core/render/Rendering.cpp
        core/render/Shaders.cpp
        core/render/Camera.cpp
        core/texture/AnimatedTexture.cpp
        core/texture/TextureHandler.cpp
        )

set(ENGINE_HEADERS_WORLD
        world/Terrain.h
        )

set(ENGINE_SOURCE_WORLD
        world/Terrain.cpp
        )

set(ENGINE_EXTERNAL_SOURCES
        ../external/glew/src/glew.c
        ../external/lodepng/lodepng.cpp)

include_directories("core")

# BUILD ENGINE
add_library(SwarmEngineCore STATIC
        ${ENGINE_HEADERS_CORE}
        ${ENGINE_HEADERS_WORLD}
        ${ENGINE_SOURCE_CORE}
        ${ENGINE_SOURCE_WORLD}
        ${ENGINE_EXTERNAL_SOURCES}
        )
add_custom_command(TARGET SwarmEngineCore POST_BUILD
        COMMAND ${CMAKE_SOURCE_DIR}/external/lhg/LibHeaderGen.exe -p ${CMAKE_SOURCE_DIR}/engine -r -o $<TARGET_FILE_DIR:SwarmEngineCore>
        COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_SOURCE_DIR}/external/engine/SwarmEngine.h
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE_DIR:SwarmEngineCore>/SwarmEngine.h ${CMAKE_SOURCE_DIR}/external/engine/SwarmEngine.h
        )
target_link_libraries(SwarmEngineCore ${OPENGL_LIBRARIES} glfw ${GLFW_LIBRARIES})