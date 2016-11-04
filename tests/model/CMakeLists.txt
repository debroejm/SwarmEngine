# CMake file for the Generic Test

set(SOURCE_FILES
        main.cpp
        )

include_directories(${CMAKE_SOURCE_DIR}/engine/core)

add_executable(SwarmEngineTest_Model ${SOURCE_FILES})
add_custom_command(TARGET SwarmEngineTest_Model POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E remove_directory $<TARGET_FILE_DIR:SwarmEngineTest_Model>/resources
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/tests/model/resources $<TARGET_FILE_DIR:SwarmEngineTest_Model>/resources
        )
target_link_libraries(SwarmEngineTest_Model SwarmEngineCore ${OPENGL_LIBRARIES} glfw ${GLFW_LIBRARIES})
