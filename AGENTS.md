# AGENTS

## 저장소 개요
- `FbxFileInfo`와 `FbxViewer`는 독립 서브프로젝트이며 둘 다 FBX SDK 기반 C++ 프로젝트다.
- C++17 사용(`CMAKE_CXX_STANDARD 17`).
- Windows + Visual Studio + CMake 환경을 기본으로 안내한다.

## 의존성/환경
- 공통: Autodesk FBX SDK 필요.
- `FbxFileInfo`:
  - FBX SDK 경로: `C:/Program Files/Autodesk/FBX/FBX SDK/2020.3.7`.
  - 배치 스크립트(`build.bat`, `run.bat`) 기준 작업 흐름 유지.
- `FbxViewer`:
  - FBX SDK 경로: `FbxViewer/SDK/fbxsdk`.
  - Qt5 경로: `FbxViewer/SDK/qtsdk/lib/cmake/Qt5`.
  - 링크: `libfbxsdk`, `Qt5::Core/Gui/Widgets`, `opengl32`.
  - 빌드 후 `libfbxsdk.dll`을 출력 폴더로 복사.

## 빌드/실행 가이드
- `FbxFileInfo`: `FbxFileInfo/CMakeLists.txt` 및 배치 스크립트 기준.
- `FbxViewer`: `FbxViewer/CMakeLists.txt` 및 로컬 SDK 폴더 구조 기준.
- 경로 상수 변경은 요청이 있을 때만 수행한다.

## 코딩 컨벤션
- 네이밍:
  - 클래스/타입: `PascalCase` (예: `FbxLoader`, `ViewerWidget`).
  - 함수/메서드: `camelCase` 권장(기존 코드 스타일 유지 우선).
  - 변수: `camelCase`.
  - 상수: `UPPER_SNAKE_CASE` 또는 `kCamelCase` 중 기존 파일 스타일 우선.
- 파일 배치:
  - `FbxFileInfo`: 단일 `main.cpp` 중심 구조 유지.
  - `FbxViewer`: UI/렌더링/로더를 파일로 분리된 현재 구조 유지(`FbxLoader.*`, `ViewerWidget.*`, `MainWindow.*`).
- 주석:
  - 복잡한 FBX 순회/메모리 라이프사이클 처리에만 간결한 주석 추가.
  - 자명한 코드에 설명 주석 추가 금지.

## 에러 처리 방식
- FBX SDK 호출 결과는 즉시 검증하고 실패 시 명확한 로그를 남긴다.
- 파일 로딩 실패, 씬 초기화 실패는 조기에 반환한다.
- `FbxFileInfo`: 콘솔 로그 기반(표준 출력/에러 출력 구분).
- `FbxViewer`: UI 응답성 유지(긴 작업은 UI 블로킹 최소화), 오류는 사용자 메시지 + 로그로 분리.

## FBX 로딩/씬 순회 규칙
- 로딩 순서:
  - Manager/IOSettings 생성 → Importer 초기화 → Scene 로드 → Importer 종료.
- 리소스 수명:
  - FBX 객체는 생성/파괴 쌍을 명확히 관리(누수 방지).
- 씬 순회:
  - 루트 노드부터 DFS 순회.
  - 노드 타입별 처리 분기(메시/카메라/라이트/스켈레톤 등).
  - 변환 정보(Translation/Rotation/Scale) 출력/활용 시 로컬/글로벌 구분 명확히.
- 메시 처리:
  - 버텍스/폴리곤 수 및 노말/UV/컬러는 존재 여부 확인 후 접근.

## Qt UI 구성 규칙 (`FbxViewer`)
- `MainWindow`:
  - 메뉴/툴바/레이아웃 구성 책임.
  - 사용자 명령(파일 열기, 보기 모드 변경) 라우팅.
- `ViewerWidget`:
  - 렌더링 및 뷰 상호작용(회전/줌/패닝) 담당.
- `FbxLoader`:
  - FBX 로딩 및 데이터 변환 로직 담당. UI 종속 코드 금지.
- UI 변경 시: 기능 영향 범위를 문서화하고 `README.md` 갱신.

## 변경 시 업데이트 규칙
- 빌드/런 명령 변경 시 해당 `README.md`와 배치 스크립트 모두 수정.
- SDK 경로 변경은 문서와 CMake를 함께 갱신.
