# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

C++ workspace with two independent FBX SDK subprojects:
- **FbxFileInfo**: Console app that reads FBX files and prints scene hierarchy (nodes, meshes, cameras, lights, transforms)
- **FbxViewer**: Qt5-based GUI app for interactive 3D FBX model viewing (currently uses placeholder rendering due to missing Qt5OpenGL in SDK)

Both use C++17 and Autodesk FBX SDK 2020.3.7. Windows + Visual Studio + CMake is the primary environment.

## Build Commands

### Full workspace (recommended — uses Ninja generator)
```bat
cmake --preset win
cmake --build --preset win
```

### Individual subprojects
```bash
# FbxFileInfo
cmake -S FbxFileInfo -B build/FbxFileInfo
cmake --build build/FbxFileInfo

# FbxViewer
cmake -S FbxViewer -B build/FbxViewer
cmake --build build/FbxViewer
```

### FbxFileInfo batch scripts
```bat
FbxFileInfo\build.bat   # Configure + build (VS 2022, x64, Release)
FbxFileInfo\run.bat     # Run with fbx_files/test.fbx
```

### Tests (FbxViewer only)
```powershell
ctest --test-dir cmake-build-debug --output-on-failure
```

### CMake path variables (override with `-D` or `CMakeUserPresets.json`)
- `FBX_SDK_ROOT` — FBX SDK install (default: `C:/Program Files/Autodesk/FBX/FBX SDK/2020.3.7`)
- `FBXVIEWER_SDK_ROOT` — FbxViewer's local FBX SDK (default: `FbxViewer/SDK/fbxsdk`)
- `FBXVIEWER_QT5_DIR` — Qt5 cmake config dir (default: `FbxViewer/SDK/qtsdk/lib/cmake/Qt5`)
- `BUILD_FBX_FILE_INFO` / `BUILD_FBX_VIEWER` — ON/OFF toggles for each subproject

## Architecture

### FbxFileInfo
Single `main.cpp`. Follows the FBX SDK lifecycle: `FbxManager` → `FbxIOSettings` → `FbxImporter` → `FbxScene` → recursive DFS traversal via `PrintNodeInfo()` → `FbxManager::Destroy()`.

### FbxViewer
Modular structure with three key files:
- **FbxLoader** (`FbxLoader.h/cpp`) — FBX SDK wrapper. Loads files, triangulates geometry, extracts per-vertex data (position, normal, color, UV) and textures. No UI code allowed here.
- **MainWindow** (`MainWindow.h/cpp`) — Qt UI orchestration. File open dialog, view mode combo box (Normal/VertexColor/VertexNormal/Texture), animation slider. Routes user commands to loader and viewer.
- **ViewerWidget** (`ViewerWidget.h/cpp`) — Rendering widget. Defines `ViewMode` enum and `MeshGL` struct. Currently a placeholder (draws text on black background) because Qt5OpenGL is unavailable in the local SDK.

Data flow: `MainWindow::openFile()` → `FbxLoader::loadFile()` → returns `vector<MeshData>` → `ViewerWidget::setMeshes()`.

### FBX SDK linking
- FbxFileInfo: Static linking (`libfbxsdk-md.lib`, `libxml2-md.lib`, `zlib-md.lib`)
- FbxViewer: Dynamic linking (`FBXSDK_SHARED` defined, post-build copies `libfbxsdk.dll`)

## Coding Conventions

- Classes/types: `PascalCase` — Functions/methods/variables: `camelCase` — Constants: `UPPER_SNAKE_CASE` or `kCamelCase`
- Comments only for complex FBX traversal or memory lifecycle logic; no comments on self-evident code
- FBX SDK calls must be validated immediately; file loading failures use early return
- FbxFileInfo logs to stdout/stderr; FbxViewer shows errors in UI + logs
- When changing SDK paths or build commands, update both documentation and CMake files together
