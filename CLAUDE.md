# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

C++ workspace with two independent FBX SDK subprojects:
- **FbxFileInfo**: Console app that reads FBX files and prints scene hierarchy (nodes, meshes, cameras, lights, transforms)
- **FbxViewer**: ImGui + GLFW + OpenGL 3.3 Core based GUI app for interactive 3D FBX model viewing

Both use C++17 and Autodesk FBX SDK 2020.3.7. Windows + Visual Studio + CMake is the primary environment.

## Build Commands

### Full workspace (recommended — uses Ninja generator, requires MSVC environment)
```bat
build_local.bat   # vcvarsall x64 → cmake --preset local → cmake --build
```

### Manual build with presets
```bat
cmake --preset local -DCMAKE_C_COMPILER=cl -DCMAKE_CXX_COMPILER=cl
cmake --build build/local
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
```bat
build\local\FbxViewer\tests\fbxviewer_tests.exe
```

### CMake path variables (override with `-D` or `CMakeUserPresets.json`)
- `FBX_SDK_ROOT` — FBX SDK install (default: `C:/Program Files/Autodesk/FBX/FBX SDK/2020.3.7`)
- `FBXVIEWER_SDK_ROOT` — FbxViewer's FBX SDK (default: `FbxViewer/SDK/fbxsdk`, falls back to system SDK)
- `BUILD_FBX_FILE_INFO` / `BUILD_FBX_VIEWER` — ON/OFF toggles for each subproject

## Architecture

### FbxFileInfo
Single `main.cpp`. Follows the FBX SDK lifecycle: `FbxManager` → `FbxIOSettings` → `FbxImporter` → `FbxScene` → recursive DFS traversal via `PrintNodeInfo()` → `FbxManager::Destroy()`.

### FbxViewer
Modular structure with four key files:
- **FbxLoader** (`FbxLoader.h/cpp`) — FBX SDK wrapper. Loads files, triangulates geometry, extracts per-vertex data (position, normal, color, UV) and textures. Uses GLM vector types. No UI code allowed here.
- **App** (`App.h/cpp`) — Application orchestration. GLFW window creation, ImGui initialization, main loop, input callbacks, file open dialog (Win32 `GetOpenFileNameA`). Owns `FbxLoader` and `Renderer`.
- **Renderer** (`Renderer.h/cpp`) — OpenGL 3.3 Core rendering. VAO/VBO/IBO management, shader compilation, 4 view modes (Normal/VertexColor/VertexNormal/Texture). Defines `ViewMode` enum and `MeshGL` struct.
- **Camera** (`Camera.h`) — Header-only orbit camera using spherical coordinates. Orbit, pan, zoom via GLM math.

Data flow: `App::openFileDialog()` → `FbxLoader::loadFile()` → returns `vector<MeshData>` → `Renderer::setMeshes()`.

### Third-party dependencies (FetchContent)
- **GLM** 1.0.1 — Math library (replaces Qt vector types)
- **GLFW** 3.4 — Windowing and input
- **Dear ImGui** v1.91.8 — Immediate mode GUI (GLFW + OpenGL3 backends)
- **GLAD** — OpenGL 3.3 Core loader (pre-generated, local at `FbxViewer/SDK/glad/`)
- **GoogleTest** v1.14.0 — Unit testing

### FBX SDK linking
- FbxFileInfo: Static linking (`libfbxsdk-md.lib`, `libxml2-md.lib`, `zlib-md.lib`)
- FbxViewer: Dynamic linking (`FBXSDK_SHARED` defined, post-build copies `libfbxsdk.dll`)

## Coding Conventions

- Classes/types: `PascalCase` — Functions/methods/variables: `camelCase` — Constants: `UPPER_SNAKE_CASE` or `kCamelCase`
- Comments only for complex FBX traversal or memory lifecycle logic; no comments on self-evident code
- FBX SDK calls must be validated immediately; file loading failures use early return
- FbxFileInfo logs to stdout/stderr; FbxViewer shows errors in ImGui panel + stderr
- When changing SDK paths or build commands, update both documentation and CMake files together
- Include `glad/glad.h` before `GLFW/glfw3.h` to prevent OpenGL header conflicts
