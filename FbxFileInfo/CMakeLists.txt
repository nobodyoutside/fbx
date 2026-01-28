cmake_minimum_required(VERSION 3.16)
project(FbxSdkTest)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# FBX SDK 경로 설정 (외부에서 -DFBX_SDK_ROOT=... 로 override 가능)
if(NOT DEFINED FBX_SDK_ROOT)
    if(WIN32)
        set(FBX_SDK_ROOT "C:/Program Files/Autodesk/FBX/FBX SDK/2020.3.7"
            CACHE PATH "FBX SDK root path")
    elseif(APPLE)
        set(FBX_SDK_ROOT "/Applications/Autodesk/FBX SDK/2020.3.7"
            CACHE PATH "FBX SDK root path")
    else()
        set(FBX_SDK_ROOT "" CACHE PATH "FBX SDK root path")
    endif()
endif()

# FBX SDK 헤더 및 라이브러리 경로 설정
include_directories("${FBX_SDK_ROOT}/include")

# 플랫폼별 라이브러리 경로 설정
if(WIN32)
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        # 64비트
        link_directories("${FBX_SDK_ROOT}/lib/x64/release")
        link_directories("${FBX_SDK_ROOT}/lib/x64/debug")
        set(FBX_LIBRARIES_RELEASE libfbxsdk-md.lib)
        set(FBX_LIBRARIES_DEBUG libfbxsdk-md.lib)
    else()
        # 32비트
        link_directories("${FBX_SDK_ROOT}/lib/x86/release")
        link_directories("${FBX_SDK_ROOT}/lib/x86/debug")
        set(FBX_LIBRARIES_RELEASE libfbxsdk-md.lib)
        set(FBX_LIBRARIES_DEBUG libfbxsdk-md.lib)
    endif()
endif()

# 실행 파일 생성
add_executable(FbxSdkTest main.cpp)

# 라이브러리 링크
target_link_libraries(FbxSdkTest 
    optimized ${FBX_LIBRARIES_RELEASE}
    debug ${FBX_LIBRARIES_DEBUG}
    optimized libxml2-md.lib
    debug libxml2-md.lib
    optimized zlib-md.lib
    debug zlib-md.lib
)

# Windows에서 필요한 추가 라이브러리
if(WIN32)
    target_link_libraries(FbxSdkTest 
        kernel32.lib 
        user32.lib 
        gdi32.lib 
        winspool.lib 
        comdlg32.lib 
        advapi32.lib 
        shell32.lib 
        ole32.lib 
        oleaut32.lib 
        uuid.lib 
        odbc32.lib 
        odbccp32.lib
    )
endif()
