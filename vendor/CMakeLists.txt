# GLFW
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
add_subdirectory(glfw)

# glad
add_library(glad STATIC glad/src/glad.c)
target_include_directories(glad PUBLIC glad/include)

# ImGui
add_library(imgui STATIC
    imgui/imgui.cpp
    imgui/imgui_draw.cpp
    imgui/imgui_tables.cpp
    imgui/imgui_widgets.cpp
    imgui/backends/imgui_impl_glfw.cpp
    imgui/backends/imgui_impl_opengl3.cpp
    imgui/misc/cpp/imgui_stdlib.cpp
)
target_include_directories(imgui PUBLIC imgui imgui/backends)
target_link_libraries(imgui PUBLIC glfw glad)

# ImGuizmo
add_library(imguizmo STATIC imguizmo/ImGuizmo.cpp)
target_include_directories(imguizmo PUBLIC imguizmo)
target_link_libraries(imguizmo PUBLIC imgui)

# stb
add_library(stb INTERFACE)
target_include_directories(stb INTERFACE stb)

# glm
add_library(glm INTERFACE)
target_include_directories(glm INTERFACE glm)

# Assimp
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_SAMPLES OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_OBJ_IMPORTER  ON CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_FBX_IMPORTER  ON CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_GLTF_IMPORTER ON CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_EXPORTERS OFF CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(ASSIMP_INSTALL OFF CACHE BOOL "" FORCE)

add_subdirectory(assimp)

# EnTT
add_library(entt INTERFACE)
target_include_directories(entt INTERFACE entt/src)

# Cereal
add_library(cereal INTERFACE)
target_include_directories(cereal INTERFACE cereal/include)

# RapidJSON
add_library(rapidjson INTERFACE)
target_include_directories(rapidjson INTERFACE rapidjson/include)