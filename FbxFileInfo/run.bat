@echo off
echo Running FBX SDK Test...

REM 실행 파일이 있는지 확인
if not exist "build\Release\FbxSdkTest.exe" (
    echo Error: FbxSdkTest.exe not found!
    echo Please run build.bat first to build the project.
    pause
    exit /b 1
)

REM FBX 파일이 있는지 확인
if not exist "fbx_files\test.fbx" (
    echo Error: test.fbx not found in fbx_files directory!
    pause
    exit /b 1
)

REM 프로그램 실행
cd build\Release
FbxSdkTest.exe

pause
