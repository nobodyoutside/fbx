#include "FbxLoader.h"
#include <iostream>

/// @brief FBX SDK 매니저를 생성하고 IO 설정을 초기화한다.
/// 매니저는 이 클래스의 수명 동안 유지되며, 소멸자에서 Destroy()로 해제된다.
FbxLoader::FbxLoader() : mSdkManager(nullptr), mScene(nullptr), mAnimStart(0), mAnimEnd(0) {
    mSdkManager = FbxManager::Create();
    FbxIOSettings* ios = FbxIOSettings::Create(mSdkManager, IOSROOT);
    mSdkManager->SetIOSettings(ios);
}

/// @brief FBX SDK 매니저를 파괴하여 매니저가 소유한 모든 FBX 객체를 일괄 해제한다.
FbxLoader::~FbxLoader() {
    if (mSdkManager) mSdkManager->Destroy();
}

/// @brief FBX 파일을 열어 씬으로 임포트한 뒤, 지오메트리를 삼각화하고 메시 데이터를 추출한다.
/// 또한 첫 번째 애니메이션 스택의 시간 범위를 mAnimStart/mAnimEnd에 저장한다.
/// @param filename 로드할 FBX 파일의 경로
/// @return 파일 열기 및 임포트 성공 시 true, 실패 시 false
bool FbxLoader::loadFile(const std::string& filename) {
    mMeshes.clear();
    mAnimStart = 0;
    mAnimEnd = 0;

    // 새 씬 생성 및 Importer로 파일 내용 로드
    mScene = FbxScene::Create(mSdkManager, "myScene");
    FbxImporter* importer = FbxImporter::Create(mSdkManager, "");

    // Importer 초기화 — 파일 열기 시도, 실패하면 false 반환
    if (!importer->Initialize(filename.c_str(), -1, mSdkManager->GetIOSettings())) {
        return false;
    }

    // 파일 내용을 씬 객체로 임포트
    if (!importer->Import(mScene)) {
        importer->Destroy();
        return false;
    }
    // Importer는 씬에 데이터를 복사한 후 더 이상 필요 없으므로 즉시 해제
    importer->Destroy();

    // 모든 지오메트리를 삼각형으로 변환 (쿼드 등 → 트라이앵글)
    FbxGeometryConverter converter(mSdkManager);
    converter.Triangulate(mScene, true);

    // 씬 루트 노드부터 재귀 순회하며 메시 데이터 추출
    processNode(mScene->GetRootNode());

    // 첫 번째 애니메이션 스택에서 재생 시간 범위를 읽어옴
    if (mScene->GetSrcObjectCount<FbxAnimStack>() > 0) {
        FbxAnimStack* animStack = mScene->GetSrcObject<FbxAnimStack>(0);
        FbxTakeInfo* takeInfo = mScene->GetTakeInfo(animStack->GetName());
        if (takeInfo) {
            mAnimStart = (float)takeInfo->mLocalTimeSpan.GetStart().GetSecondDouble();
            mAnimEnd = (float)takeInfo->mLocalTimeSpan.GetStop().GetSecondDouble();
        }
    }

    return true;
}

/// @brief 씬 트리를 DFS로 재귀 순회하며, 메시 속성을 가진 노드를 발견하면 processMesh()를 호출한다.
/// @param node 현재 방문 중인 FBX 노드 (nullptr이면 즉시 반환)
void FbxLoader::processNode(FbxNode* node) {
    if (!node) return;

    // 노드에 메시 속성이 있으면 메시 데이터 추출
    FbxNodeAttribute* attribute = node->GetNodeAttribute();
    if (attribute && attribute->GetAttributeType() == FbxNodeAttribute::eMesh) {
        processMesh(node->GetMesh());
    }

    // 자식 노드를 재귀적으로 순회
    for (int i = 0; i < node->GetChildCount(); ++i) {
        processNode(node->GetChild(i));
    }
}

/// @brief FBX 메시 객체에서 폴리곤별로 정점 데이터(위치, 법선, 버텍스 컬러, UV)를 추출하고,
/// 첫 번째 머티리얼의 Diffuse 텍스처 경로를 가져와 MeshData에 저장한 뒤 mMeshes에 추가한다.
/// @param mesh 데이터를 추출할 FBX 메시 객체 (삼각화 완료된 상태를 가정)
void FbxLoader::processMesh(FbxMesh* mesh) {
    MeshData data;
    int vertexCount = mesh->GetControlPointsCount();
    FbxVector4* controlPoints = mesh->GetControlPoints();

    // UV set name queried once before the loop
    FbxStringList uvSetNames;
    mesh->GetUVSetNames(uvSetNames);
    const char* uvSetName = (uvSetNames.GetCount() > 0) ? uvSetNames.GetStringAt(0) : nullptr;

    // 폴리곤(삼각형) 단위로 정점과 인덱스를 순회하며 추출
    int polygonCount = mesh->GetPolygonCount();
    for (int i = 0; i < polygonCount; ++i) {
        for (int j = 0; j < 3; ++j) {
            // 현재 폴리곤 버텍스에 해당하는 컨트롤 포인트 인덱스 취득
            int ctrlPointIndex = mesh->GetPolygonVertex(i, j);
            Vertex vertex;

            // 정점 위치: 컨트롤 포인트 배열에서 XYZ 좌표 추출
            vertex.position = QVector3D(
                (float)controlPoints[ctrlPointIndex][0],
                (float)controlPoints[ctrlPointIndex][1],
                (float)controlPoints[ctrlPointIndex][2]
            );

            // 법선 벡터: 폴리곤 버텍스 단위의 법선을 FBX SDK로부터 읽어옴
            FbxVector4 fbxNormal;
            if (mesh->GetPolygonVertexNormal(i, j, fbxNormal)) {
                vertex.normal = QVector3D((float)fbxNormal[0], (float)fbxNormal[1], (float)fbxNormal[2]);
            }

            // 버텍스 컬러: 레이어 0의 eByPolygonVertex 매핑 모드인 경우에만 추출
            if (mesh->GetElementVertexColorCount() > 0) {
                const FbxGeometryElementVertexColor* elementColor = mesh->GetElementVertexColor(0);
                if (elementColor->GetMappingMode() == FbxLayerElement::eByPolygonVertex) {
                    // Direct 모드면 순차 인덱스, IndexToDirect 모드면 인덱스 배열에서 조회
                    int colorIndex = (elementColor->GetReferenceMode() == FbxLayerElement::eDirect) ? i * 3 + j : elementColor->GetIndexArray().GetAt(i * 3 + j);
                    FbxColor color = elementColor->GetDirectArray().GetAt(colorIndex);
                    vertex.color = QVector4D((float)color.mRed, (float)color.mGreen, (float)color.mBlue, (float)color.mAlpha);
                }
            } else {
                // 버텍스 컬러가 없으면 흰색(불투명)으로 기본값 설정
                vertex.color = QVector4D(1.0f, 1.0f, 1.0f, 1.0f);
            }

            // UV 좌표: 첫 번째 UV 세트에서 폴리곤 버텍스 단위로 읽어옴
            if (uvSetName) {
                FbxVector2 uv;
                bool unmapped;
                if (mesh->GetPolygonVertexUV(i, j, uvSetName, uv, unmapped)) {
                    vertex.uv = QVector2D((float)uv[0], (float)uv[1]);
                }
            }

            data.vertices.push_back(vertex);
            // 인덱스는 정점 추가 순서대로 순차 번호를 부여 (현재 정점 공유 없음)
            data.indices.push_back((unsigned int)data.vertices.size() - 1);
        }
    }

    // 텍스처 경로 추출: 첫 번째 머티리얼의 Diffuse 프로퍼티에 연결된 파일 텍스처를 읽어옴
    FbxNode* node = mesh->GetNode();
    if (node && node->GetMaterialCount() > 0) {
        FbxSurfaceMaterial* material = node->GetMaterial(0);
        FbxProperty prop = material->FindProperty(FbxSurfaceMaterial::sDiffuse);
        if (prop.IsValid()) {
            int fileTextureCount = prop.GetSrcObjectCount<FbxFileTexture>();
            if (fileTextureCount > 0) {
                FbxFileTexture* texture = prop.GetSrcObject<FbxFileTexture>(0);
                data.texturePath = texture->GetFileName();
            }
        }
    }

    mMeshes.push_back(data);
}

/// @brief 지정된 시간으로 애니메이션 상태를 갱신한다.
/// 현재는 정적 메시 뷰어 용도이므로 구현이 비어 있다.
/// 스켈레탈 애니메이션 지원 시 노드 트랜스폼 갱신 로직을 여기에 추가해야 한다.
/// @param time 갱신할 애니메이션 시간(초)
void FbxLoader::updateAnimation(float time) {
}
