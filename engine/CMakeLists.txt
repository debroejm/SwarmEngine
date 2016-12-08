# CMake file for the Main Engine

project(SwarmEngine)

set(SwarmEngine_VERSION_MAJOR "0")
set(SwarmEngine_VERSION_MINOR "1")
set(SwarmEngine_VERSION_PATCH "0")

set(ENGINE_HEADERS_CORE
        Core.h
        Render.h
        CLEngine.h
        core/GLFWCallbacks.h
        Util.h
)

set(ENGINE_SOURCE_CORE
        core/GLFWCallbacks.cpp
        core/Initialization.cpp
        core/Logging.cpp
        core/MiscUtil.cpp
        core/config/Configuration.cpp
        core/config/Controls.cpp
        core/config/Keybindings.cpp
        render/model/Animation.cpp
        render/model/Model.cpp
        render/model/ModelLoading.cpp
        render/model/RawModelData.cpp
        render/Renderer.cpp
        render/RenderObject.cpp
        render/Shaders.cpp
        render/Camera.cpp
        render/Uniforms.cpp
        render/texture/Texture.cpp
        render/texture/AnimatedTexture.cpp
        render/texture/SingleTexture.cpp
        render/texture/TextureHandler.cpp
        cl/init.cpp
        cl/context.cpp
)

set(ENGINE_EXTERNAL_SOURCES
        ../external/glew/src/glew.c
        ../external/lodepng/lodepng.cpp)

# BUILD ENGINE
add_library(SwarmEngineCore STATIC
        ${ENGINE_HEADERS_CORE}
        ${ENGINE_SOURCE_CORE}
        ${ENGINE_EXTERNAL_SOURCES}
)
# add_custom_command(TARGET SwarmEngineCore POST_BUILD
#         COMMAND ${CMAKE_SOURCE_DIR}/external/lhg/LibHeaderGen.exe -p ${CMAKE_SOURCE_DIR}/engine -r -o $<TARGET_FILE_DIR:SwarmEngineCore>
#         COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_SOURCE_DIR}/external/engine/SwarmEngine.h
#         COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE_DIR:SwarmEngineCore>/SwarmEngine.h ${CMAKE_SOURCE_DIR}/external/engine/SwarmEngine.h
#         )
target_link_libraries(SwarmEngineCore ${OPENGL_LIBRARIES} glfw ${GLFW_LIBRARIES} OpenCL)