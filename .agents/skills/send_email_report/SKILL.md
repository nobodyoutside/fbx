---
name: Send Email Report
description: QA/검토 결과 보고서를 plaied45@gmail.com으로 Gmail SMTP를 통해 이메일 전송합니다.
---

# 이메일 결과 보고 스킬

QA/검토 에이전트가 작업 완료 후 결과를 `plaied45@gmail.com`에게 이메일로 전송할 때 사용합니다.  
**OAuth2 Gmail API**(권장)와 **SMTP 폴백** 두 가지 방법을 지원합니다.

---

## 방법 A: Gmail REST API + OAuth2 (권장)

### 최초 1회 설정

1. [Google Cloud Console](https://console.cloud.google.com/) 에서 프로젝트 생성
2. Gmail API 활성화
3. OAuth 동의 화면 설정 → 테스트 사용자에 본인 계정 추가
4. 사용자 인증 정보 → **데스크탑 앱** 유형으로 OAuth 클라이언트 ID 생성
5. JSON 다운로드 → `.agents/secrets/client_secret.json` 으로 저장
6. 인증 스크립트 실행:
   ```powershell
   powershell -ExecutionPolicy Bypass -File .\.agents\scripts\gmail_oauth_setup.ps1
   ```
   → 브라우저에서 Google 로그인 → 권한 허용 → `.agents/secrets/gmail_token.json` 자동 저장

### 이메일 전송 (인증 후)

1. **발신자 주소** 설정 (권장):
   ```powershell
   $env:GMAIL_FROM_ADDRESS = "yourname@gmail.com"
   ```
2. **스크립트 실행**:
   ```powershell
   # 환경변수 설정 후에는 -From 없이 실행 가능합니다.
   .\.agents\scripts\qa_send_email.ps1 `
     -To "plaied45@gmail.com" `
     -Subject "[QA] FbxViewer 검증 결과" `
     -BodyFile "qa_report.md"
   ```

---

## 방법 B: SMTP 앱 비밀번호 (폴백)

### 사전 설정

1. [앱 비밀번호 생성](https://myaccount.google.com/apppasswords) (2단계 인증 필요)
2. **앱 비밀번호** 환경변수 설정:
   ```powershell
   $env:GMAIL_APP_PASSWORD = "xxxx xxxx xxxx xxxx"
   ```
3. **발신자 주소** 환경변수 설정:
   ```powershell
   $env:GMAIL_FROM_ADDRESS = "yourname@gmail.com"
   ```

### 이메일 전송
```powershell
.\.agents\scripts\qa_send_email.ps1 `
  -To "plaied45@gmail.com" `
  -Subject "[QA] FbxViewer 검증 결과" `
  -BodyFile "qa_report.md"
```

---

## 보안 주의사항

> [!CAUTION]
> `.agents/secrets/` 디렉토리는 반드시 `.gitignore`에 추가해야 합니다.
> `client_secret.json`과 `gmail_token.json`이 Git에 커밋되면 보안 사고로 직결됩니다.

- 토큰 파일(`gmail_token.json`)에는 refresh_token이 포함되어 있어 무기한 접근 가능
- 앱 비밀번호는 스크립트에 **하드코딩 금지** — 환경변수 또는 파라미터로만 전달
- OAuth2는 `gmail.send` 스코프만 요청 (메일 읽기 권한 없음)

## 결과 확인

- `[SUCCESS]` 메시지 → 전송 성공
- `[ERROR]` 메시지 → 에러 내용 확인 후 재시도 (인코딩 문제로 깨져 보일 경우 -From 파라미터 누락 여부 확인)
- OAuth2 실패 시 SMTP 폴백 자동 시도 (둘 다 설정된 경우)
