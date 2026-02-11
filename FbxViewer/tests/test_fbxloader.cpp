#include <gtest/gtest.h>
#include "FbxLoader.h"
#include <fstream>
#include <cstdio>

#ifndef TEST_FBX_FILE
#define TEST_FBX_FILE ""
#endif

/// @brief FbxLoader 단위 테스트 픽스처. 각 테스트마다 새로운 FbxLoader 인스턴스를 생성한다.
class FbxLoaderTest : public ::testing::Test {
protected:
    FbxLoader loader;
};

/// @brief FbxLoader 객체의 생성과 소멸이 크래시 없이 완료되는지 검증한다.
TEST_F(FbxLoaderTest, InitAndDestroy) {
    SUCCEED();
}

/// @brief 존재하지 않는 파일 경로를 전달하면 loadFile()이 false를 반환하는지 검증한다.
TEST_F(FbxLoaderTest, LoadNonExistentFile) {
    EXPECT_FALSE(loader.loadFile("nonexistent_file_that_does_not_exist.fbx"));
}

/// @brief 유효하지 않은 FBX 형식의 파일을 전달하면 loadFile()이 false를 반환하는지 검증한다.
TEST_F(FbxLoaderTest, LoadInvalidFile) {
    // 임시 파일을 생성하여 잘못된 내용을 기록
    const char* tempFile = "test_invalid_temp.fbx";
    {
        std::ofstream ofs(tempFile);
        ofs << "this is not a valid fbx file";
    }
    EXPECT_FALSE(loader.loadFile(tempFile));
    // 테스트 후 임시 파일 정리
    std::remove(tempFile);
}

/// @brief 유효한 FBX 파일을 로드하면 성공하고, 메시 데이터가 존재하는지 검증한다.
/// TEST_FBX_FILE이 미설정이면 테스트를 건너뛴다.
TEST_F(FbxLoaderTest, LoadValidFbxFile) {
    std::string testFile = TEST_FBX_FILE;
    if (testFile.empty()) {
        GTEST_SKIP() << "TEST_FBX_FILE not set";
    }
    EXPECT_TRUE(loader.loadFile(testFile));
    EXPECT_FALSE(loader.getMeshes().empty());
}

/// @brief 로드된 각 메시에 정점과 인덱스가 1개 이상 존재하는지 검증한다.
TEST_F(FbxLoaderTest, MeshDataHasVertices) {
    std::string testFile = TEST_FBX_FILE;
    if (testFile.empty()) {
        GTEST_SKIP() << "TEST_FBX_FILE not set";
    }
    ASSERT_TRUE(loader.loadFile(testFile));
    for (const auto& mesh : loader.getMeshes()) {
        EXPECT_GT(mesh.vertices.size(), 0u);
        EXPECT_GT(mesh.indices.size(), 0u);
    }
}

/// @brief 모든 메시의 인덱스가 해당 메시의 정점 범위 내에 있는지 검증한다.
TEST_F(FbxLoaderTest, IndicesInRange) {
    std::string testFile = TEST_FBX_FILE;
    if (testFile.empty()) {
        GTEST_SKIP() << "TEST_FBX_FILE not set";
    }
    ASSERT_TRUE(loader.loadFile(testFile));
    for (const auto& mesh : loader.getMeshes()) {
        for (unsigned int idx : mesh.indices) {
            EXPECT_LT(idx, mesh.vertices.size());
        }
    }
}

/// @brief 애니메이션 시작 시간이 종료 시간 이하인지 검증한다. (시간 범위 유효성)
TEST_F(FbxLoaderTest, AnimationTimeValid) {
    std::string testFile = TEST_FBX_FILE;
    if (testFile.empty()) {
        GTEST_SKIP() << "TEST_FBX_FILE not set";
    }
    ASSERT_TRUE(loader.loadFile(testFile));
    EXPECT_LE(loader.getAnimationStart(), loader.getAnimationEnd());
}

/// @brief 동일 파일을 두 번 연속 로드해도 크래시 없이 성공하는지 검증한다.
/// 주의: 현재 구현에서는 mMeshes가 초기화되지 않아 메시 데이터가 누적됨.
TEST_F(FbxLoaderTest, MultipleLoadCalls) {
    std::string testFile = TEST_FBX_FILE;
    if (testFile.empty()) {
        GTEST_SKIP() << "TEST_FBX_FILE not set";
    }
    // 첫 번째 로드
    ASSERT_TRUE(loader.loadFile(testFile));
    size_t firstMeshCount = loader.getMeshes().size();
    EXPECT_GT(firstMeshCount, 0u);

    // 두 번째 로드 — 이전 데이터와 누적됨 (현재 구현의 알려진 동작)
    ASSERT_TRUE(loader.loadFile(testFile));
    EXPECT_FALSE(loader.getMeshes().empty());
}
