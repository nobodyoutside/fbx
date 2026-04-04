cmake_minimum_required(VERSION 3.16)
project(FbxViewer)

set(CMAKE_CXX_STANDARD 17)

# ─── FBX SDK setup ───────────────────────────────────────────────
if(NOT DEFINED FBXVIEWER_SDK_ROOT)
    set(LOCAL_SDK "${CMAKE_CURRENT_SOURCE_DIR}/SDK/fbxsdk")
    set(SYSTEM_SDK "C:/Program Files/Autodesk/FBX/FBX SDK/2020.3.7")

    if(EXISTS "${LOCAL_SDK}/include/fbxsdk.h")
        set(FBXVIEWER_SDK_ROOT "${LOCAL_SDK}" CACHE PATH "FBX SDK root for FbxViewer" FORCE)
    elseif(DEFINED ENV{FBX_SDK_ROOT} AND EXISTS "$ENV{FBX_SDK_ROOT}/include/fbxsdk.h")
        set(FBXVIEWER_SDK_ROOT "$ENV{FBX_SDK_ROOT}" CACHE PATH "FBX SDK root for FbxViewer" FORCE)
    elseif(EXISTS "${SYSTEM_SDK}/include/fbxsdk.h")
        set(FBXVIEWER_SDK_ROOT "${SYSTEM_SDK}" CACHE PATH "FBX SDK root for FbxViewer" FORCE)
    else()
        set(FBXVIEWER_SDK_ROOT "${LOCAL_SDK}" CACHE PATH "FBX SDK root for FbxViewer")
    endif()
endif()
set(FBX_SDK_ROOT "${FBXVIEWER_SDK_ROOT}")
set(FBX_SDK_INCLUDE_DIR "${FBX_SDK_ROOT}/include")

if(EXISTS "${FBX_SDK_ROOT}/lib/x64")
    set(FBX_SDK_LIB_BASE "${FBX_SDK_ROOT}/lib/x64")
elseif(EXISTS "${FBX_SDK_ROOT}/lib/vs2019/x64")
    set(FBX_SDK_LIB_BASE "${FBX_SDK_ROOT}/lib/vs2019/x64")
elseif(EXISTS "${FBX_SDK_ROOT}/lib/vs2017/x64")
    set(FBX_SDK_LIB_BASE "${FBX_SDK_ROOT}/lib/vs2017/x64")
else()
    message(WARNING "Cannot find FBX SDK lib directory under ${FBX_SDK_ROOT}/lib")
    set(FBX_SDK_LIB_BASE "${FBX_SDK_ROOT}/lib/x64")
endif()

# ─── Third-party dependencies via FetchContent ───────────────────
include(FetchContent)

# GLM (header-only math library)
FetchContent_Declare(glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG        1.0.1
)
FetchContent_MakeAvailable(glm)

# GLFW (windowing + input)
FetchContent_Declare(glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG        3.4
)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(glfw)

# Dear ImGui (no native CMake — register sources manually)
FetchContent_Declare(imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG        v1.91.8
)
FetchContent_MakeAvailable(imgui)

add_library(imgui_lib STATIC
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
)
target_include_directories(imgui_lib PUBLIC
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
)
target_link_libraries(imgui_lib PUBLIC glfw)

# GLAD (local — generated for OpenGL 3.3 Core)
add_library(glad STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/SDK/glad/src/glad.c
)
target_include_directories(glad PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/SDK/glad/include
)

# ─── FbxLoaderLib (static library, no UI dependency) ─────────────
add_library(FbxLoaderLib STATIC
    FbxLoader.cpp
    FbxLoader.h
)
target_include_directories(FbxLoaderLib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${FBX_SDK_INCLUDE_DIR}
)
target_link_libraries(FbxLoaderLib PUBLIC glm::glm libfbxsdk)
target_link_directories(FbxLoaderLib PUBLIC
    $<IF:$<CONFIG:Debug>,${FBX_SDK_LIB_BASE}/debug,${FBX_SDK_LIB_BASE}/release>
)
target_compile_definitions(FbxLoaderLib PUBLIC FBXSDK_SHARED)

# ─── FbxViewer executable ────────────────────────────────────────
add_executable(FbxViewer
    main.cpp
    App.cpp
    App.h
    Renderer.cpp
    Renderer.h
    Camera.h
)
target_link_libraries(FbxViewer PRIVATE
    FbxLoaderLib
    imgui_lib
    glad
    glfw
    opengl32
)

# Copy libfbxsdk.dll to output directory
add_custom_command(TARGET FbxViewer POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "$<IF:$<CONFIG:Debug>,${FBX_SDK_LIB_BASE}/debug,${FBX_SDK_LIB_BASE}/release>/libfbxsdk.dll"
    $<TARGET_FILE_DIR:FbxViewer>
)

# ─── GoogleTest ──────────────────────────────────────────────────
option(BUILD_FBXVIEWER_TESTS "Build FbxViewer tests" ON)

if(BUILD_FBXVIEWER_TESTS)
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.14.0
    )
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)

    enable_testing()
    add_subdirectory(tests)
endif()
