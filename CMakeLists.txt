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

# FbxViewer SDK root (override via -DFBXVIEWER_SDK_ROOT=...)
set(FBXVIEWER_SDK_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/FbxViewer/SDK/fbxsdk"
    CACHE PATH "FBX SDK root for FbxViewer")

enable_testing()

if(BUILD_FBX_FILE_INFO)
  add_subdirectory(FbxFileInfo)
endif()

if(BUILD_FBX_VIEWER)
  add_subdirectory(FbxViewer)
endif()
