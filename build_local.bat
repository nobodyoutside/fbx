@echo off
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat" x64
cd /d "%~dp0"
if exist build\local rmdir /s /q build\local
cmake --preset local -DCMAKE_C_COMPILER=cl -DCMAKE_CXX_COMPILER=cl
if errorlevel 1 (
    echo === CONFIGURE FAILED ===
    exit /b 1
)
cmake --build build/local
if errorlevel 1 (
    echo === BUILD FAILED ===
    exit /b 1
)
echo === BUILD SUCCESS ===
