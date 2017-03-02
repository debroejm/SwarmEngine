# CMake file for the CL Test

project(SwarmEngineTest_CL)

set(SOURCE_FILES
        main.cpp
        )

add_executable(SwarmEngineTest_CL ${SOURCE_FILES})
add_custom_command(TARGET SwarmEngineTest_CL POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:OpenCL> $<TARGET_FILE_DIR:SwarmEngineTest_CL>
        )
target_link_libraries(SwarmEngineTest_CL SwarmEngineCore ${OPENGL_LIBRARIES} glfw ${GLFW_LIBRARIES})
set_target_properties(SwarmEngineTest_CL
        PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tests/CL
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tests/CL
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tests/CL
        )