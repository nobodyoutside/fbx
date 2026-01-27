cmake_minimum_required(VERSION 4.1)
project(FbxViewer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# FBX SDK setup
set(FBX_SDK_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/SDK/fbxsdk")
set(FBX_SDK_INCLUDE_DIR "${FBX_SDK_ROOT}/include")
set(FBX_SDK_LIB_DIR "${FBX_SDK_ROOT}/lib/vs2017/x64/debug")

# Qt5 setup
set(Qt5_DIR "${CMAKE_CURRENT_SOURCE_DIR}/SDK/qtsdk/lib/cmake/Qt5")
set(CMAKE_PREFIX_PATH "${CMAKE_CURRENT_SOURCE_DIR}/SDK/qtsdk/lib/cmake")
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

target_link_directories(FbxViewer PRIVATE ${FBX_SDK_LIB_DIR})

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
    "${FBX_SDK_LIB_DIR}/libfbxsdk.dll"
    $<TARGET_FILE_DIR:FbxViewer>)

enable_testing()
add_test(NAME FBXSDK_Test COMMAND FbxViewer)
