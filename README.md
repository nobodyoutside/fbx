# FBX SDK Workspace

이 저장소는 FBX SDK 기반의 두 서브프로젝트를 포함합니다.

- `FbxFileInfo`: FBX 파일 정보 출력용 콘솔 앱
- `FbxViewer`: Qt 기반 FBX 뷰어

## 공통 요구사항
- CMake 3.16 이상
- C++17 컴파일러
- Autodesk FBX SDK 설치
- `FbxViewer`는 Qt5 필요

## CMake Presets로 빌드 (권장)

### Windows
```bat
cmake --preset win
cmake --build --preset win
```

### macOS
```bash
cmake --preset mac
cmake --build --preset mac
```

### 개인 환경 경로 설정 (CMakeUserPresets.json)
1. `CMakeUserPresets.json`에서 경로를 수정
2. 빌드 실행
```bash
cmake --preset local
cmake --build --preset local
```

## CMake 프리셋 경로 설정
- `CMakePresets.json`은 공통 템플릿입니다.
- 환경별 경로는 `CMakeUserPresets.json`에만 설정하는 것을 권장합니다.

필수 경로 키:
- `FBX_SDK_ROOT`
- `FBXVIEWER_SDK_ROOT`
- `FBXVIEWER_QT5_DIR`

## 개별 서브프로젝트 빌드

### FbxFileInfo
```bash
cmake -S FbxFileInfo -B build/FbxFileInfo
cmake --build build/FbxFileInfo
```

### FbxViewer
```bash
cmake -S FbxViewer -B build/FbxViewer
cmake --build build/FbxViewer
```

## 실행
- `FbxFileInfo`: `FbxFileInfo` 폴더의 `build.bat`/`run.bat` 또는 빌드 출력 폴더의 실행 파일 사용
- `FbxViewer`: 빌드 출력 폴더의 `FbxViewer` 실행
