#include <iostream>
#include <string>
#include <fbxsdk.h>

/// @brief FBX 씬 트리를 재귀적으로 순회하며 각 노드의 정보를 콘솔에 출력한다.
/// @param pNode 현재 처리할 FBX 노드 (nullptr이면 즉시 반환)
/// @param depth 현재 트리 깊이 (들여쓰기 수준 결정에 사용)
void PrintNodeInfo(FbxNode* pNode, int depth = 0)
{
    if (!pNode)
        return;

    // 트리 깊이에 따른 들여쓰기 문자열 생성
    std::string indent(depth * 2, ' ');

    // 현재 노드의 이름 출력
    std::cout << indent << "Node: " << pNode->GetName() << std::endl;

    // 노드에 연결된 속성(Attribute)의 타입과 세부 정보를 출력
    FbxNodeAttribute* pAttribute = pNode->GetNodeAttribute();
    if (pAttribute)
    {
        // 속성 타입을 문자열로 변환
        std::string attributeType = "Unknown";

        switch (pAttribute->GetAttributeType())
        {
        case FbxNodeAttribute::eMesh:
            attributeType = "Mesh";
            break;
        case FbxNodeAttribute::eCamera:
            attributeType = "Camera";
            break;
        case FbxNodeAttribute::eLight:
            attributeType = "Light";
            break;
        case FbxNodeAttribute::eSkeleton:
            attributeType = "Skeleton";
            break;
        case FbxNodeAttribute::eNurbs:
            attributeType = "NURBS";
            break;
        case FbxNodeAttribute::ePatch:
            attributeType = "Patch";
            break;
        case FbxNodeAttribute::eNurbsCurve:
            attributeType = "NURBS Curve";
            break;
        case FbxNodeAttribute::eTrimNurbsSurface:
            attributeType = "Trim NURBS Surface";
            break;
        case FbxNodeAttribute::eBoundary:
            attributeType = "Boundary";
            break;
        case FbxNodeAttribute::eNurbsSurface:
            attributeType = "NURBS Surface";
            break;
        case FbxNodeAttribute::eShape:
            attributeType = "Shape";
            break;
        case FbxNodeAttribute::eLODGroup:
            attributeType = "LOD Group";
            break;
        case FbxNodeAttribute::eSubDiv:
            attributeType = "SubDiv";
            break;
        case FbxNodeAttribute::eCachedEffect:
            attributeType = "Cached Effect";
            break;
        case FbxNodeAttribute::eLine:
            attributeType = "Line";
            break;
        default:
            attributeType = "Unknown";
            break;
        }

        std::cout << indent << "  Type: " << attributeType << std::endl;

        // 메시 타입인 경우 정점 수와 폴리곤 수를 추가 출력
        if (pAttribute->GetAttributeType() == FbxNodeAttribute::eMesh)
        {
            FbxMesh* pMesh = static_cast<FbxMesh*>(pAttribute);
            std::cout << indent << "  Vertices: " << pMesh->GetControlPointsCount() << std::endl;
            std::cout << indent << "  Polygons: " << pMesh->GetPolygonCount() << std::endl;
        }
    }

    // 노드의 글로벌 변환 행렬에서 이동/회전/스케일 값을 추출하여 출력
    FbxAMatrix& transform = pNode->EvaluateGlobalTransform();
    FbxVector4 translation = transform.GetT();
    FbxVector4 rotation = transform.GetR();
    FbxVector4 scaling = transform.GetS();

    std::cout << indent << "  Translation: ("
              << translation[0] << ", "
              << translation[1] << ", "
              << translation[2] << ")" << std::endl;
    std::cout << indent << "  Rotation: ("
              << rotation[0] << ", "
              << rotation[1] << ", "
              << rotation[2] << ")" << std::endl;
    std::cout << indent << "  Scale: ("
              << scaling[0] << ", "
              << scaling[1] << ", "
              << scaling[2] << ")" << std::endl;

    std::cout << std::endl;

    // 모든 자식 노드를 재귀적으로 순회
    for (int i = 0; i < pNode->GetChildCount(); i++)
    {
        PrintNodeInfo(pNode->GetChild(i), depth + 1);
    }
}

/// @brief FBX 파일을 읽어 씬 계층 구조를 콘솔에 출력하는 프로그램의 진입점.
/// FBX SDK 수명주기: FbxManager 생성 → IOSettings → Importer → Scene → 탐색 → Destroy
int main()
{
    // FBX SDK의 메모리 관리 매니저 생성 (모든 FBX 객체의 소유자)
    FbxManager* pManager = FbxManager::Create();
    if (!pManager)
    {
        std::cerr << "Error: Unable to create FBX Manager!" << std::endl;
        return -1;
    }

    // 파일 입출력에 관한 설정 객체 생성 및 매니저에 등록
    FbxIOSettings* pIOSettings = FbxIOSettings::Create(pManager, IOSROOT);
    pManager->SetIOSettings(pIOSettings);

    // FBX 데이터를 담을 씬(Scene) 컨테이너 생성
    FbxScene* pScene = FbxScene::Create(pManager, "MyScene");
    if (!pScene)
    {
        std::cerr << "Error: Unable to create FBX Scene!" << std::endl;
        pManager->Destroy();
        return -1;
    }

    // 파일 읽기를 담당하는 Importer 생성
    FbxImporter* pImporter = FbxImporter::Create(pManager, "");

    // 읽을 FBX 파일 경로 (하드코딩)
    const char* fileName = "fbx_files/test.fbx";

    std::cout << "Loading FBX file: " << fileName << std::endl;

    // Importer 초기화 — 파일 형식 자동 감지(-1), 실패 시 에러 출력 후 종료
    bool result = pImporter->Initialize(fileName, -1, pManager->GetIOSettings());
    if (!result)
    {
        std::cerr << "Error: Unable to initialize FBX importer!" << std::endl;
        std::cerr << "Error description: " << pImporter->GetStatus().GetErrorString() << std::endl;
        pImporter->Destroy();
        pManager->Destroy();
        return -1;
    }

    // 초기화된 Importer로 FBX 파일 내용을 씬에 임포트
    result = pImporter->Import(pScene);
    if (!result)
    {
        std::cerr << "Error: Unable to import FBX file!" << std::endl;
        std::cerr << "Error description: " << pImporter->GetStatus().GetErrorString() << std::endl;
        pImporter->Destroy();
        pManager->Destroy();
        return -1;
    }

    // 임포트 완료 후 Importer 즉시 해제 (씬에 데이터가 복사되었으므로 불필요)
    pImporter->Destroy();

    std::cout << "FBX file loaded successfully!" << std::endl;
    std::cout << "================================================" << std::endl;

    // 씬 메타데이터 출력 (씬 이름)
    std::cout << "Scene Name: " << pScene->GetName() << std::endl;

    // FBX 파일을 생성한 애플리케이션 정보 출력
    FbxDocumentInfo* pDocInfo = pScene->GetDocumentInfo();
    if (pDocInfo)
    {
        std::cout << "Application: " << pDocInfo->Original_ApplicationName.Get().Buffer() << std::endl;
        std::cout << "Application Version: " << pDocInfo->Original_ApplicationVersion.Get().Buffer() << std::endl;
    }

    std::cout << "================================================" << std::endl;
    std::cout << "Scene Objects:" << std::endl;
    std::cout << "================================================" << std::endl;

    // 루트 노드에서 시작하여 전체 씬 트리를 재귀 출력
    FbxNode* pRootNode = pScene->GetRootNode();
    if (pRootNode)
    {
        PrintNodeInfo(pRootNode);
    }

    // FbxManager::Destroy()로 매니저가 소유한 모든 FBX 객체 일괄 해제
    pManager->Destroy();

    std::cout << "Program completed successfully!" << std::endl;

    // 콘솔 창이 바로 닫히지 않도록 사용자 입력 대기
    std::cout << "Press Enter to exit...";
    std::cin.get();

    return 0;
}
