cmake_minimum_required(VERSION 3.16)
project(FbxAll)

# Global options
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(BUILD_FBX_FILE_INFO "Build FbxFileInfo subproject" ON)
option(BUILD_FBX_VIEWER "Build FbxViewer subproject" ON)

# Optional shared SDK hints (subprojects may override)
# OS-aware defaults (users can override with -D... on the command line)
if(WIN32)
  set(FBX_SDK_ROOT "C:/Program Files/Autodesk/FBX/FBX SDK/2020.3.7"
      CACHE PATH "FBX SDK root for FbxFileInfo")
elseif(APPLE)
  set(FBX_SDK_ROOT "/Applications/Autodesk/FBX SDK/2020.3.7"
      CACHE PATH "FBX SDK root for FbxFileInfo")
else()
  set(FBX_SDK_ROOT "" CACHE PATH "FBX SDK root for FbxFileInfo")
endif()

# FbxViewer SDK roots (override via -DFBXVIEWER_SDK_ROOT=... and -DFBXVIEWER_QT5_DIR=...)
# - FBXVIEWER_SDK_ROOT: root containing include/ and lib/
# - FBXVIEWER_QT5_DIR: Qt5 cmake config dir (ends with /lib/cmake/Qt5)
set(FBXVIEWER_SDK_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/FbxViewer/SDK/fbxsdk"
    CACHE PATH "FBX SDK root for FbxViewer")
set(FBXVIEWER_QT5_DIR "${CMAKE_CURRENT_SOURCE_DIR}/FbxViewer/SDK/qtsdk/lib/cmake/Qt5"
    CACHE PATH "Qt5 Dir for FbxViewer")

if(BUILD_FBX_FILE_INFO)
  add_subdirectory(FbxFileInfo)
endif()

if(BUILD_FBX_VIEWER)
  add_subdirectory(FbxViewer)
endif()
