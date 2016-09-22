# CMake file for the Test


set(SOURCE_FILES
        main.cpp
        ../engine/Core.h
        ../engine/Render.h
)

include_directories(../engine/core)

add_executable(SwarmEngineTest ${SOURCE_FILES})
add_custom_command(TARGET SwarmEngineTest POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E remove_directory $<TARGET_FILE_DIR:SwarmEngineTest>/resources
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/test/resources $<TARGET_FILE_DIR:SwarmEngineTest>/resources
)
target_link_libraries(SwarmEngineTest SwarmEngineCore ${OPENGL_LIBRARIES} glfw ${GLFW_LIBRARIES})
