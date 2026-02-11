cmake_minimum_required(VERSION 4.1)
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

include_directories(
    ${FBX_SDK_INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/SDK/qtsdk/include
    ${CMAKE_CURRENT_SOURCE_DIR}/SDK/qtsdk/include/QtCore
    ${CMAKE_CURRENT_SOURCE_DIR}/SDK/qtsdk/include/QtGui
    ${CMAKE_CURRENT_SOURCE_DIR}/SDK/qtsdk/include/QtWidgets
)

add_executable(FbxViewer 
    main.cpp
    FbxLoader.cpp
    FbxLoader.h
    ViewerWidget.cpp
    ViewerWidget.h
    MainWindow.cpp
    MainWindow.h
)

target_include_directories(FbxViewer PRIVATE 
    ${FBX_SDK_INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/SDK/qtsdk/include
    ${CMAKE_CURRENT_SOURCE_DIR}/SDK/qtsdk/include/QtCore
    ${CMAKE_CURRENT_SOURCE_DIR}/SDK/qtsdk/include/QtGui
    ${CMAKE_CURRENT_SOURCE_DIR}/SDK/qtsdk/include/QtWidgets
)

target_link_directories(FbxViewer PRIVATE
    "$<IF:$<CONFIG:Debug>,${FBX_SDK_LIB_BASE}/debug,${FBX_SDK_LIB_BASE}/release>")

# Link against FBX SDK and Qt5
target_compile_definitions(FbxViewer PRIVATE FBXSDK_SHARED)
target_link_libraries(FbxViewer PRIVATE 
    libfbxsdk 
    Qt5::Core 
    Qt5::Gui 
    Qt5::Widgets 
    opengl32
)

# Copy DLLs to output directory
add_custom_command(TARGET FbxViewer POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "$<IF:$<CONFIG:Debug>,${FBX_SDK_LIB_BASE}/debug,${FBX_SDK_LIB_BASE}/release>/libfbxsdk.dll"
    $<TARGET_FILE_DIR:FbxViewer>)

enable_testing()
add_test(NAME FBXSDK_Test COMMAND FbxViewer)
