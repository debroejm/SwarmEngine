# CMake file for the VHE Test

project(SwarmEngineTest_VHE)

set(SOURCE_FILES
        main.cpp
        )

add_executable(SwarmEngineTest_VHE ${SOURCE_FILES})
add_custom_command(TARGET SwarmEngineTest_VHE POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:OpenCL> $<TARGET_FILE_DIR:SwarmEngineTest_VHE>
        )
target_link_libraries(SwarmEngineTest_VHE SwarmEngineCore)
set_target_properties(SwarmEngineTest_VHE
        PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tests/VHE
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tests/VHE
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tests/VHE
        )