cmake_minimum_required(VERSION 3.16)
project(FbxAll)

# Global options
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(BUILD_FBX_FILE_INFO "Build FbxFileInfo subproject" ON)
option(BUILD_FBX_VIEWER "Build FbxViewer subproject" ON)

# Optional shared SDK hints (subprojects may override)
# OS-aware defaults (users can override with -D... on the command line)
set(FBX_SDK_ROOT $ENV{FBX_SDK_ROOT}
        CACHE PATH "FBX SDK root for FbxFileInfo")

# FbxViewer SDK roots (override via -DFBXVIEWER_SDK_ROOT=... and -DFBXVIEWER_QT5_DIR=...)
# - FBXVIEWER_SDK_ROOT: root containing include/ and lib/
# - FBXVIEWER_QT5_DIR: Qt5 cmake config dir (ends with /lib/cmake/Qt5)
set(FBXVIEWER_SDK_ROOT $ENV{FBX_SDK_ROOT}
    CACHE PATH "FBX SDK root for FbxViewer")
set(FBXVIEWER_QT5_DIR $ENV{FBXVIEWER_QT5_DIR}
    CACHE PATH "Qt5 Dir for FbxViewer")

enable_testing()

if(BUILD_FBX_FILE_INFO)
  add_subdirectory(FbxFileInfo)
endif()

if(BUILD_FBX_VIEWER)
  add_subdirectory(FbxViewer)
endif()
