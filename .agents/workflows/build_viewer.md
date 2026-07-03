---
description: FbxViewer 프로젝트 빌드 및 실행 워크플로우
---

이 워크플로우는 CMake를 사용하여 FbxViewer 프로젝트를 빌드하고 실행하는 단계를 나타냅니다.

1. `FbxViewer` 디렉토리로 이동하여 `빌드` 구성을 시작합니다.
`cd d:\TestWork\FbxSdkTest\FbxViewer`

// turbo
2. CMake 구성(Configure)을 실행합니다.
`cmake -B build -S .`

// turbo
3. 프로젝트를 빌드(Compile)합니다.
`cmake --build build --config Release`
