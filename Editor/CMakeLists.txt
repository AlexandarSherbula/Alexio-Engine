project(Editor)

# Base output path
set(OUTPUT_DIR "${CMAKE_BINARY_DIR}/bin/${PROJECT_NAME}/${CMAKE_SYSTEM_NAME}/$<CONFIG>")

# These apply to all targets created afterwards
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${OUTPUT_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${OUTPUT_DIR})

file(
    GLOB_RECURSE EDITOR_SRC
    "Source/*.cpp"
    "Source/*.hpp"
)

add_executable(Editor ${EDITOR_SRC})
target_compile_features(Editor PRIVATE cxx_std_20)

#make folders visible in IDE
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${EDITOR_SRC})

target_link_libraries(Editor PRIVATE Engine)
