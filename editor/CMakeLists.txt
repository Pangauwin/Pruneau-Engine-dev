set(EDITOR_DIR ${CMAKE_CURRENT_SOURCE_DIR})

file(GLOB_RECURSE EDITOR_SRC
    "${EDITOR_DIR}/*.cpp"
)

add_executable(PruneauEditor
    ${EDITOR_SRC}
)

target_link_libraries(PruneauEditor
    PRIVATE PruneauEngine imgui imguizmo
)

target_include_directories(PruneauEditor
    PRIVATE ${CMAKE_SOURCE_DIR}/engine/public
    PRIVATE ${CMAKE_SOURCE_DIR}/editor
)