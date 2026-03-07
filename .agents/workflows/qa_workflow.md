---
description: 개발 완료 후 검증(QA), 화면 캡처 및 결과 보고 가이드
---
# QA 및 검증 전담 에이전트 (QA Workflow)

이 워크플로우는 개발 에이전트가 부여받은 `task.md` 상의 작업을 모두 완료했을 때 구동되어야 합니다. QA 에이전트는 누락된 요구사항이 없는지 확인하고, 빌드된 결과물(FbxViewer 등)을 실행하여 스크린샷 캡처본과 함께 프로젝트 매니저(PM) 에이전트 또는 사용자에게 이메일로 최종 보고합니다.

## 1. 미완료 작업 점검 (Verification)
1. 기획 단계에서 작성된 `implementation_plan.md`와 `task.md`를 읽고 모든 체크리스트가 달성되었는지 대조합니다.
2. 각 변경 사항이 `AGENTS.md`에 명시된 규칙(특히 FBX 메모리 해제, Qt UI 차단 방지)을 준수했는지 소스 코드를 스캔하여 마지막으로 확인합니다.
3. 문제가 발견되면, 즉시 PM이나 개발 에이전트에게 수정 사항을 피드백합니다.

## 2. 빌드 및 테스트 실행
1. `build.bat` 과 `run.bat`(혹은 FbxViewer 직접 실행)를 통해 프로그램이 정상적으로 빌드 및 런칭되는지 확인합니다.
2. 필요한 경우 특정 FBX 파일을 인자로 넘기어 뷰어에 올바르게 로드되는지 확인합니다.

## 3. 실행 결과 이미지 캡처
테스트 프로그램(FbxViewer)이 띄워지면, 사전에 제공된 스크립트를 통해 화면을 캡처하여 시각적으로 검증합니다.

* **캡처 스크립트 사용법**:
  ```powershell
  # PowerShell에서 아래 스크립트 실행
  powershell -ExecutionPolicy Bypass -File .\.agents\scripts\qa_screenshot.ps1 -Output "QA_Result.png"
  ```
* 캡처된 `QA_Result.png`를 확인하여 렌더링이나 UI 상의 결함(예: 텍스트 겹침, 모델 렌더링 실패)이 없는지 검토합니다.

## 4. 최종 보고 (Reporting & Hand-off)
1. 테스트 로그, 누락 없이 통과된 내역, 발견된 경고, 그리고 `QA_Result.png` 파일 경로를 포함한 최종 QA 리포트(예: `qa_report.md`)를 작성합니다.
2. 이 결과를 PM 에이전트에게 Handoff 하거나, 지정된 메일 스크립트를 이용해 결과를 발송합니다.
  ```powershell
  # 이메일 전송 스크립트 가이드 (수신자/발신자 정보는 적절히 수정하여 사용)
  powershell -ExecutionPolicy Bypass -File .\.agents\scripts\qa_send_email.ps1 -To "pm@example.com" -Subject "QA Report: FbxViewer" -BodyFile "qa_report.md" -Attachment "QA_Result.png"
  ```
