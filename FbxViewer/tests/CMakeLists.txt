# 테스트 FBX 파일 경로를 컴파일 타임에 전달
set(TEST_FBX_FILE "${CMAKE_CURRENT_SOURCE_DIR}/../../FbxFileInfo/fbx_files/test.fbx")

add_executable(fbxviewer_tests
    test_fbxloader.cpp
)

target_link_libraries(fbxviewer_tests PRIVATE
    FbxLoaderLib
    GTest::gtest_main
)

target_compile_definitions(fbxviewer_tests PRIVATE
    TEST_FBX_FILE="${TEST_FBX_FILE}"
)

# libfbxsdk.dll 복사
add_custom_command(TARGET fbxviewer_tests POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "$<IF:$<CONFIG:Debug>,${FBX_SDK_LIB_BASE}/debug,${FBX_SDK_LIB_BASE}/release>/libfbxsdk.dll"
    $<TARGET_FILE_DIR:fbxviewer_tests>
)

# Qt DLL 수동 연결 (기본 콘솔 테스트용)
add_custom_command(TARGET fbxviewer_tests POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${Qt5_DIR}/../../../bin/Qt5Core.dll"
    $<TARGET_FILE_DIR:fbxviewer_tests>
)
add_custom_command(TARGET fbxviewer_tests POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${Qt5_DIR}/../../../bin/Qt5Gui.dll"
    $<TARGET_FILE_DIR:fbxviewer_tests>
)
add_custom_command(TARGET fbxviewer_tests POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${Qt5_DIR}/../../../bin/Qt5Widgets.dll"
    $<TARGET_FILE_DIR:fbxviewer_tests>
)

include(GoogleTest)
gtest_discover_tests(fbxviewer_tests DISCOVERY_MODE PRE_TEST)
