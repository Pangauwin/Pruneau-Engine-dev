set(ENGINE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

file(GLOB_RECURSE ENGINE_PUBLIC
    "${ENGINE_DIR}/public/*.h"
)

file(GLOB_RECURSE ENGINE_INTERNAL
    "${ENGINE_DIR}/internal/*.cpp"
    "${ENGINE_DIR}/internal/*.h"
)

add_library(PruneauEngine STATIC
    ${ENGINE_INTERNAL}
    ${ENGINE_PUBLIC}
)

target_include_directories(PruneauEngine
    PUBLIC  ${ENGINE_DIR}/public
    PRIVATE ${ENGINE_DIR}/internal
)

target_link_libraries(PruneauEngine
    PUBLIC
        glfw
        glad
        glm
        stb
        assimp
        entt
)

if (UNIX)
    find_package(OpenGL REQUIRED)
    find_package(X11 REQUIRED)

    target_link_libraries(PruneauEngine PUBLIC
        OpenGL::GL
        X11::X11
    )
endif()