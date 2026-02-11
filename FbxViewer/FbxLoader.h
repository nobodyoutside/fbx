#ifndef FBXLOADER_H
#define FBXLOADER_H

#include <fbxsdk.h>
#include <vector>
#include <string>
#include <QVector3D>
#include <QVector2D>
#include <QVector4D>

/// @brief 하나의 정점이 가지는 속성 데이터 (위치, 법선, 색상, UV 좌표)
struct Vertex {
    QVector3D position;  // 정점의 3D 월드 좌표
    QVector3D normal;    // 정점의 법선 벡터
    QVector4D color;     // 정점 색상 (RGBA)
    QVector2D uv;        // 텍스처 UV 좌표
};

/// @brief 하나의 메시를 구성하는 정점 배열, 인덱스 배열, 텍스처 경로
struct MeshData {
    std::vector<Vertex> vertices;       // 정점 목록
    std::vector<unsigned int> indices;  // 삼각형 인덱스 목록
    std::string texturePath;            // Diffuse 텍스처 파일 경로 (없으면 빈 문자열)
};

/// @brief FBX SDK를 래핑하여 FBX 파일을 로드하고 메시/애니메이션 데이터를 추출하는 클래스.
/// FbxManager 수명주기를 내부적으로 관리한다.
class FbxLoader {
public:
    /// @brief FBX SDK 매니저와 IO 설정을 초기화하는 생성자
    FbxLoader();

    /// @brief FBX SDK 매니저를 파괴하여 모든 SDK 리소스를 해제하는 소멸자
    ~FbxLoader();

    /// @brief 지정된 FBX 파일을 로드하고, 삼각화 후 메시 데이터를 추출한다.
    /// @param filename 로드할 FBX 파일 경로
    /// @return 로드 성공 시 true, 실패 시 false
    bool loadFile(const std::string& filename);

    /// @brief 로드된 모든 메시 데이터의 읽기 전용 참조를 반환한다.
    const std::vector<MeshData>& getMeshes() const { return mMeshes; }

    /// @brief 애니메이션 시작 시간(초)을 반환한다.
    float getAnimationStart() const { return mAnimStart; }

    /// @brief 애니메이션 종료 시간(초)을 반환한다.
    float getAnimationEnd() const { return mAnimEnd; }

    /// @brief 지정된 시간으로 애니메이션 상태를 갱신한다. (현재 미구현)
    /// @param time 갱신할 애니메이션 시간(초)
    void updateAnimation(float time);

private:
    /// @brief 씬 트리를 재귀적으로 순회하며 메시 노드를 찾아 processMesh()를 호출한다.
    /// @param node 현재 처리할 FBX 노드
    void processNode(FbxNode* node);

    /// @brief FBX 메시에서 정점(위치, 법선, 색상, UV), 인덱스, 텍스처 경로를 추출한다.
    /// @param mesh 처리할 FBX 메시 객체
    void processMesh(FbxMesh* mesh);

    FbxManager* mSdkManager;        // FBX SDK 메모리 관리 매니저
    FbxScene* mScene;               // 현재 로드된 FBX 씬
    std::vector<MeshData> mMeshes;  // 추출된 메시 데이터 목록

    float mAnimStart;  // 애니메이션 시작 시간(초)
    float mAnimEnd;    // 애니메이션 종료 시간(초)
};

#endif // FBXLOADER_H
