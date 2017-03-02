# CMake file for the Generic Test

project(SwarmEngineTest_Generic)

set(SOURCE_FILES
        main.cpp
)

add_executable(SwarmEngineTest_Generic ${SOURCE_FILES})
add_custom_command(TARGET SwarmEngineTest_Generic POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E remove_directory $<TARGET_FILE_DIR:SwarmEngineTest_Generic>/resources
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/tests/generic/resources $<TARGET_FILE_DIR:SwarmEngineTest_Generic>/resources
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:OpenCL> $<TARGET_FILE_DIR:SwarmEngineTest_Generic>
)
target_link_libraries(SwarmEngineTest_Generic SwarmEngineCore)
set_target_properties(SwarmEngineTest_Generic
        PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tests/Generic
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tests/Generic
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tests/Generic
)