cmake_minimum_required(VERSION 3.16)
project(FbxViewer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# FBX SDK setup (외부에서 -DFBXVIEWER_SDK_ROOT=... 로 override 가능)
if(NOT DEFINED FBXVIEWER_SDK_ROOT)
    set(FBXVIEWER_SDK_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/SDK/fbxsdk"
        CACHE PATH "FBX SDK root for FbxViewer")
endif()
set(FBX_SDK_ROOT "${FBXVIEWER_SDK_ROOT}")
set(FBX_SDK_INCLUDE_DIR "${FBX_SDK_ROOT}/include")
# Detect lib directory: system SDK uses lib/x64/<config>, local SDK uses lib/vs2017/x64/<config>
if(EXISTS "${FBX_SDK_ROOT}/lib/x64")
    set(FBX_SDK_LIB_BASE "${FBX_SDK_ROOT}/lib/x64")
elseif(EXISTS "${FBX_SDK_ROOT}/lib/vs2017/x64")
    set(FBX_SDK_LIB_BASE "${FBX_SDK_ROOT}/lib/vs2017/x64")
else()
    message(FATAL_ERROR "Cannot find FBX SDK lib directory under ${FBX_SDK_ROOT}/lib")
endif()

# Qt5 setup (외부에서 -DFBXVIEWER_QT5_DIR=... 로 override 가능)
if(NOT DEFINED FBXVIEWER_QT5_DIR)
    set(FBXVIEWER_QT5_DIR "${CMAKE_CURRENT_SOURCE_DIR}/SDK/qtsdk/lib/cmake/Qt5"
        CACHE PATH "Qt5 Dir for FbxViewer")
endif()
set(Qt5_DIR "${FBXVIEWER_QT5_DIR}")
set(CMAKE_PREFIX_PATH "${FBXVIEWER_QT5_DIR}/..")
find_package(Qt5 REQUIRED COMPONENTS Core Gui Widgets)

# FbxLoader를 정적 라이브러리로 분리 (테스트에서 재사용)
add_library(FbxLoaderLib STATIC
    FbxLoader.cpp
    FbxLoader.h
)
target_include_directories(FbxLoaderLib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${FBX_SDK_INCLUDE_DIR}
)
target_link_directories(FbxLoaderLib PUBLIC
    "$<IF:$<CONFIG:Debug>,${FBX_SDK_LIB_BASE}/debug,${FBX_SDK_LIB_BASE}/release>")
target_compile_definitions(FbxLoaderLib PUBLIC FBXSDK_SHARED)
target_link_libraries(FbxLoaderLib PUBLIC
    libfbxsdk
    Qt5::Core
    Qt5::Gui
)

# FbxViewer 실행 파일
add_executable(FbxViewer
    main.cpp
    ViewerWidget.cpp ViewerWidget.h
    MainWindow.cpp MainWindow.h
)

target_include_directories(FbxViewer PRIVATE ${FBX_SDK_INCLUDE_DIR})

target_link_libraries(FbxViewer PRIVATE
    FbxLoaderLib
    Qt5::Core Qt5::Gui Qt5::Widgets
    opengl32
)

# Copy DLLs to output directory
add_custom_command(TARGET FbxViewer POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "$<IF:$<CONFIG:Debug>,${FBX_SDK_LIB_BASE}/debug,${FBX_SDK_LIB_BASE}/release>/libfbxsdk.dll"
    $<TARGET_FILE_DIR:FbxViewer>)

# Qt DLL 배포 (windeployqt로 필요한 DLL + 플러그인 자동 복사)
find_program(WINDEPLOYQT_EXECUTABLE windeployqt HINTS "${FBXVIEWER_QT5_DIR}/../../../bin")
if(WINDEPLOYQT_EXECUTABLE)
    add_custom_command(TARGET FbxViewer POST_BUILD
        COMMAND "${WINDEPLOYQT_EXECUTABLE}" --no-translations --no-compiler-runtime
            --no-angle --no-opengl-sw $<TARGET_FILE:FbxViewer>
    )
endif()

# GoogleTest 기반 유닛 테스트
option(BUILD_FBXVIEWER_TESTS "Build FbxViewer tests" ON)

if(BUILD_FBXVIEWER_TESTS)
    include(FetchContent)
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
