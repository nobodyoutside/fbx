# FBX SDK Test Project

이 프로젝트는 Autodesk FBX SDK를 사용하여 FBX 파일의 모든 오브젝트 정보를 출력하는 C++ 프로그램입니다.

## 요구사항

- **FBX SDK 2020.3.7**: `C:\Program Files\Autodesk\FBX\FBX SDK\2020.3.7`에 설치되어 있어야 합니다
- **Visual Studio 2019 이상**: C++ 컴파일러가 필요합니다
- **CMake 3.16 이상**: 프로젝트 빌드를 위해 필요합니다

## 프로젝트 구조

```
FbxSdkTest/
├── main.cpp           # 메인 소스 코드
├── CMakeLists.txt     # CMake 빌드 구성
├── build.bat          # 빌드 스크립트
├── run.bat            # 실행 스크립트
├── fbx_files/         # FBX 파일 디렉토리
│   └── test.fbx       # 테스트용 FBX 파일
└── README.md          # 이 파일
```

## 빌드 방법

1. **자동 빌드 (권장)**:
   ```bash
   build.bat
   ```

2. **수동 빌드**:
   ```bash
   mkdir build
   cd build
   cmake .. -G "Visual Studio 16 2019" -A x64
   cmake --build . --config Release
   ```

## 실행 방법

1. **자동 실행**:
   ```bash
   run.bat
   ```

2. **수동 실행**:
   ```bash
   cd build\Release
   FbxSdkTest.exe
   ```

## 기능

이 프로그램은 `fbx_files/test.fbx` 파일을 읽어서 다음 정보를 출력합니다:

- **Scene 정보**: Scene 이름, 애플리케이션 정보
- **모든 Node 정보**: 
  - Node 이름
  - Node 타입 (Mesh, Camera, Light, Skeleton 등)
  - 메시의 경우: 버텍스 수, 폴리곤 수
  - 변환 정보: 위치(Translation), 회전(Rotation), 크기(Scale)

## 출력 예시

```
Loading FBX file: fbx_files/test.fbx
FBX file loaded successfully!
================================================
Scene Name: MyScene
Application: Blender
Application Version: 3.6.0
================================================
Scene Objects:
================================================
Node: RootNode
  Translation: (0, 0, 0)
  Rotation: (0, 0, 0)
  Scale: (1, 1, 1)

  Node: Cube
    Type: Mesh
    Vertices: 8
    Polygons: 6
    Translation: (0, 0, 0)
    Rotation: (0, 0, 0)
    Scale: (1, 1, 1)
```

## 문제 해결

### 빌드 오류
- FBX SDK가 올바른 경로에 설치되어 있는지 확인하세요
- Visual Studio 2019 이상이 설치되어 있는지 확인하세요
- CMake가 시스템 PATH에 등록되어 있는지 확인하세요

### 실행 오류
- `fbx_files/test.fbx` 파일이 존재하는지 확인하세요
- FBX SDK의 DLL 파일들이 시스템에서 찾을 수 있는 위치에 있는지 확인하세요

## 라이선스

이 프로젝트는 Autodesk FBX SDK의 라이선스 조건을 따릅니다.
