---
name: Verify FBX SDK Setup
description: 프로젝트 내 FBX SDK 파일 위치와 설정이 규칙에 부합하는지 검증합니다.
---

# FBX SDK 검증 스킬

이 스킬은 빌드 또는 코딩을 시작하기 전, `FbxFileInfo`와 `FbxViewer` 두 프로젝트의 FBX SDK 참조 경로가 유효한지 확인하는 데 사용됩니다.

## 수행 지침
1. **FbxFileInfo 검사**:
   - `CMakeLists.txt` 파일 내에서 FBX SDK의 경로가 `C:/Program Files/Autodesk/FBX/FBX SDK/2020.3.7`로 명시되어 있는지 확인합니다.
2. **FbxViewer 검사**:
   - `FbxViewer/SDK/fbxsdk` 디렉토리에 대상 라이브러리와 헤더가 존재하는지 점검하고, 만약 비어 있다면 `CMakeLists.txt` 내에 시스템 환경 변수나 공통 설치 경로(`C:/Program Files/Autodesk/FBX/FBX SDK/2020.3.7` 등)로 대체(Fallback)하는 검증 로직이 동작하는지 평가합니다.
   - `CMakeLists.txt`에서 링크 대상이 `libfbxsdk`로 잘 설정되어 있는지 검토합니다.
3. 빌드를 마친 후, 실행 단계에서 `libfbxsdk.dll` 모듈을 찾을 수 없다는 오류가 없도록, DLL 복사 스크립트나 명령어가 포함되어 있는지 확인합니다.

## 조치
만약 규칙에 맞지 않는 설정이 발견되면, 즉시 사용자에게 알려 수정 방향을 제안하십시오.
