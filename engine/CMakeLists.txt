# CMake file for the Main Engine

project(SwarmEngine)

set(SwarmEngine_VERSION_MAJOR "0")
set(SwarmEngine_VERSION_MINOR "1")
set(SwarmEngine_VERSION_PATCH "0")

set(ENGINE_API_HEADERS
        api/CLEngine.h
        api/common_header.h
        api/Core.h
        api/Exception.h
        api/Logging.h
        api/Render.h
        api/Synchronization.h
        api/Util.h
        api/VHE.h
)

set(ENGINE_HEADERS_CORE
        cl/CLInternal.h
        render/RenderInternal.h
        vhe/BytecodeDefines.h
        vhe/Compiler.h
        vhe/Optimizer.h
        vhe/VHEInternal.h
)

set(ENGINE_SOURCE_CORE

        core/init.cpp
        logging/Logging.cpp

        core/config/Configuration.cpp
        core/config/Keybindings.cpp

        exception/ex_cl_object_creation.cpp
        exception/ex_model_loading.cpp
        exception/ex_parsing.cpp
        exception/ex_render_program.cpp
        exception/ex_render_shader.cpp
        exception/ex_texture.cpp
        exception/ex_window.cpp

        physics/init.cpp
        physics/physics_object.cpp

        render/camera.cpp
        render/init.cpp
        render/renderer.cpp
        render/render_object.cpp
        render/uniform.cpp
        render/window.cpp

        render/model/Model.cpp
        render/model/model_loading.cpp
        render/model/raw_model_data.cpp

        render/shader/program.cpp
        render/shader/shader.cpp
        render/shader/type.cpp

        render/texture/texture.cpp
        render/texture/texture_loading.cpp
        render/texture/type.cpp

        cl/init.cpp
        cl/context.cpp
        cl/program.cpp
        cl/buffer.cpp
        cl/cmdqueue.cpp

        util/delta_time_controller.cpp
        util/read_write_buffer.cpp
        util/uid_pool.cpp

        vhe/init.cpp
        vhe/compiler.cpp
        vhe/optimizer.cpp

        vhe/environment/environment.cpp
        vhe/environment/memory.cpp
        vhe/environment/program.cpp
)

set(ENGINE_EXTERNAL_SOURCES
        ../external/glew/src/glew.c
        ../external/lodepng/lodepng.cpp)

include_directories(.)

# BUILD ENGINE
add_library(SwarmEngineCore STATIC
        ${ENGINE_API_HEADERS}
        ${ENGINE_HEADERS_CORE}
        ${ENGINE_SOURCE_CORE}
        ${ENGINE_EXTERNAL_SOURCES}
)
# add_custom_command(TARGET SwarmEngineCore POST_BUILD
#         COMMAND ${CMAKE_SOURCE_DIR}/external/lhg/LibHeaderGen.exe -p ${CMAKE_SOURCE_DIR}/engine -r -o $<TARGET_FILE_DIR:SwarmEngineCore>
#         COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_SOURCE_DIR}/external/engine/SwarmEngine.h
#         COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE_DIR:SwarmEngineCore>/SwarmEngine.h ${CMAKE_SOURCE_DIR}/external/engine/SwarmEngine.h
#         )
target_link_libraries(SwarmEngineCore ${OPENGL_LIBRARIES} glfw ${GLFW_LIBRARIES} OpenCL ${Boost_LIBRARIES})