@echo off
setlocal

echo ============================================
echo  FbxSdkTest - Windows Build
echo ============================================

REM MSVC 환경 설정 (Strawberry Perl GCC 대신 MSVC 사용)
set "VCVARSALL=C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat"
if not exist "%VCVARSALL%" (
    echo [ERROR] vcvarsall.bat not found: %VCVARSALL%
    echo Visual Studio 2022 Community Edition이 설치되어 있는지 확인하세요.
    pause
    exit /b 1
)

call "%VCVARSALL%" x64
if %ERRORLEVEL% NEQ 0 (
    echo [ERROR] MSVC 환경 설정 실패
    pause
    exit /b 1
)

echo.
echo [1/2] CMake Configure (preset: win)
cmake --preset win
if %ERRORLEVEL% NEQ 0 (
    echo [ERROR] CMake 구성 실패
    pause
    exit /b 1
)

echo.
echo [2/2] CMake Build (preset: win)
cmake --build --preset win
if %ERRORLEVEL% NEQ 0 (
    echo [ERROR] 빌드 실패
    pause
    exit /b 1
)

echo.
echo ============================================
echo  빌드 완료!
echo  출력 경로: build\win\
echo ============================================
pause
