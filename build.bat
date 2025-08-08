@echo off
echo Building FBX SDK Test Project...

REM Build 디렉토리 생성
if not exist build mkdir build
cd build

REM CMake 구성
cmake .. -G "Visual Studio 17 2022" -A x64

if %ERRORLEVEL% NEQ 0 (
    echo CMake configuration failed!
    pause
    exit /b 1
)

REM Visual Studio로 빌드
cmake --build . --config Release

if %ERRORLEVEL% NEQ 0 (
    echo Build failed!
    pause
    exit /b 1
)

echo Build completed successfully!
echo Executable location: build\Release\FbxSdkTest.exe

pause
