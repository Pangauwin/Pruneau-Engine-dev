set(SANDBOX_DIR ${CMAKE_CURRENT_SOURCE_DIR})

file(GLOB_RECURSE SANDBOX_SRC
    "${SANDBOX_DIR}/*.cpp"
)

add_executable(Sandbox
    ${SANDBOX_SRC}
)

target_link_libraries(Sandbox
    PRIVATE PruneauEngine
)

target_include_directories(Sandbox
    PRIVATE ${CMAKE_SOURCE_DIR}/engine/public
    PRIVATE ${CMAKE_SOURCE_DIR}/sandbox
)

target_link_libraries(PruneauEngine
    PUBLIC
        glm
        imgui
        imguizmo
)