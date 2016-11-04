# CMake file for the Generic Test


set(SOURCE_FILES
        main.cpp
        ${CMAKE_SOURCE_DIR}/engine/Core.h
        ${CMAKE_SOURCE_DIR}/engine/Render.h
)

include_directories(${CMAKE_SOURCE_DIR}/engine/core)

add_executable(SwarmEngineTest_Generic ${SOURCE_FILES})
add_custom_command(TARGET SwarmEngineTest_Generic POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E remove_directory $<TARGET_FILE_DIR:SwarmEngineTest_Generic>/resources
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/tests/generic/resources $<TARGET_FILE_DIR:SwarmEngineTest_Generic>/resources
)
target_link_libraries(SwarmEngineTest_Generic SwarmEngineCore ${OPENGL_LIBRARIES} glfw ${GLFW_LIBRARIES})
