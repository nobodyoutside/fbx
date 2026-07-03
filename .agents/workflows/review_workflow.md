---
description: 코드 작업 완료 후 무결성 점검(FBX 메모리 및 Qt 블로킹) 리뷰 가이드
---
# 무결성 검토 전담 에이전트 (Review Workflow)

이 워크플로우는 개발(Dev) 에이전트가 코드를 다 작성했다고 판단한 후, 코드가 주 브랜치에 병합되거나 QA로 넘어가기 전에 수행하는 치밀한 정적 검토 과정입니다. 이 단계의 가장 큰 목적은 C++ 메모리 누수와 메인 스레드 멈춤 현상을 사전에 박멸하는 것입니다.

## 1. 코드 변경(Diff) 스캔 (Scope Definition)
*   리뷰를 시작하면, 기획(PM) 단계의 `task.md`를 먼저 가져와 달성 목표를 확인합니다.
*   Dev 측이 최근 수정한 `git diff` 혹은 파일을 스캔하여 이번 작업이 어느 범위인지 특정합니다.

## 2. FBX 리소스 해제 검증 (Zero-Leak Policy)
FBX SDK는 C++ 수동 메모리 관리가 필수적이므로 이 과정은 절대 생략할 수 없습니다. 수정된 코드 내에서 다음 패턴을 꼼꼼히 찾으십시오.
*   **할당 추적**: `FbxManager::Create()`, `FbxIOSettings::Create()`, `FbxImporter::Create()`, `FbxScene::Create()` 등이 호출되었는지 살핍니다.
*   **소멸 보장**: 객체가 할당되었다면, 어떤 에러 분기(Early Return/Exception)를 타더라도 `destroy()` 함수나 리소스 릴리즈 코드가 호출되는지 확인합니다.
*   **객체 복사 주의**: 노드나 메시의 포인터(`FbxNode*`, `FbxMesh*`)를 어딘가에 보관(Vector 등)할 때 생명주기가 UI 수명주기보다 긴지 판단합니다.

## 3. Qt Threading 및 UI 응답성 검증 (Responsive UI Policy)
`FbxViewer` 모듈 변경이 포함되어 있다면 반드시 다음을 점검하십시오.
*   파일 열기, 텍스처 로딩 같이 무거운 I/O나 연산 작업이 `MainWindow`나 `ViewerWidget` 같은 메인(GUI) 스레드에서 직접 동기적으로(Synchronously) 실행되고 있지 않은지 검사합니다.
*   긴 작업이 도입되었다면 `비동기 처리(QtConcurrent::run 등)` 또는 시그널/슬롯 방식으로 우회되고 있는지 확인합니다.

## 4. 인계 및 피드백 (Handoff & Feedback)
*   **반려 (Dev 반환)**: 위 2, 3번에서 결함이 강력하게 예상되면, 문제를 수정할 개발(Dev) 에이전트에게 지적 사항과 함께 코드 수정을 지시합니다. (`"dev_workflow로 돌아가서 이 메모리 누수를 고쳐줘"`)
*   **통과 (QA 인계)**: 리뷰를 통과했다면, 이제 앱을 실제로 켜서 화면을 캡처하고 검증할 품질 보증(QA) 에이전트를 호출합니다. (`"코드 무결성을 확인했어. qa_workflow 프로세스를 구동해 화면을 테스트해 줘"`)
