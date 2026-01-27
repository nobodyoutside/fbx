#include <iostream>
#include <string>
#include <fbxsdk.h>

// FBX 노드를 재귀적으로 탐색하여 모든 오브젝트 정보를 출력하는 함수
void PrintNodeInfo(FbxNode* pNode, int depth = 0)
{
    if (!pNode)
        return;

    // 들여쓰기를 위한 문자열 생성
    std::string indent(depth * 2, ' ');

    // 노드 이름 출력
    std::cout << indent << "Node: " << pNode->GetName() << std::endl;

    // 노드의 속성(Attribute) 정보 출력
    FbxNodeAttribute* pAttribute = pNode->GetNodeAttribute();
    if (pAttribute)
    {
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
        
        // 메시인 경우 추가 정보 출력
        if (pAttribute->GetAttributeType() == FbxNodeAttribute::eMesh)
        {
            FbxMesh* pMesh = (FbxMesh*)pAttribute;
            std::cout << indent << "  Vertices: " << pMesh->GetControlPointsCount() << std::endl;
            std::cout << indent << "  Polygons: " << pMesh->GetPolygonCount() << std::endl;
        }
    }

    // 변환 정보 출력
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

    // 자식 노드들을 재귀적으로 처리
    for (int i = 0; i < pNode->GetChildCount(); i++)
    {
        PrintNodeInfo(pNode->GetChild(i), depth + 1);
    }
}

int main()
{
    // FBX SDK 초기화
    FbxManager* pManager = FbxManager::Create();
    if (!pManager)
    {
        std::cerr << "Error: Unable to create FBX Manager!" << std::endl;
        return -1;
    }

    // IO 설정 객체 생성
    FbxIOSettings* pIOSettings = FbxIOSettings::Create(pManager, IOSROOT);
    pManager->SetIOSettings(pIOSettings);

    // Scene 객체 생성
    FbxScene* pScene = FbxScene::Create(pManager, "MyScene");
    if (!pScene)
    {
        std::cerr << "Error: Unable to create FBX Scene!" << std::endl;
        pManager->Destroy();
        return -1;
    }

    // Importer 생성
    FbxImporter* pImporter = FbxImporter::Create(pManager, "");
    
    // FBX 파일 경로
    const char* fileName = "fbx_files/test.fbx";
    
    std::cout << "Loading FBX file: " << fileName << std::endl;
    
    // FBX 파일 로드
    bool result = pImporter->Initialize(fileName, -1, pManager->GetIOSettings());
    if (!result)
    {
        std::cerr << "Error: Unable to initialize FBX importer!" << std::endl;
        std::cerr << "Error description: " << pImporter->GetStatus().GetErrorString() << std::endl;
        pImporter->Destroy();
        pManager->Destroy();
        return -1;
    }

    // Scene에 FBX 파일 내용 임포트
    result = pImporter->Import(pScene);
    if (!result)
    {
        std::cerr << "Error: Unable to import FBX file!" << std::endl;
        std::cerr << "Error description: " << pImporter->GetStatus().GetErrorString() << std::endl;
        pImporter->Destroy();
        pManager->Destroy();
        return -1;
    }

    // Importer 해제 (더 이상 필요 없음)
    pImporter->Destroy();

    std::cout << "FBX file loaded successfully!" << std::endl;
    std::cout << "================================================" << std::endl;

    // Scene 정보 출력
    std::cout << "Scene Name: " << pScene->GetName() << std::endl;
    
    // 애플리케이션 정보 출력
    FbxDocumentInfo* pDocInfo = pScene->GetDocumentInfo();
    if (pDocInfo)
    {
        std::cout << "Application: " << pDocInfo->Original_ApplicationName.Get().Buffer() << std::endl;
        std::cout << "Application Version: " << pDocInfo->Original_ApplicationVersion.Get().Buffer() << std::endl;
    }

    std::cout << "================================================" << std::endl;
    std::cout << "Scene Objects:" << std::endl;
    std::cout << "================================================" << std::endl;

    // 루트 노드부터 시작하여 모든 오브젝트 출력
    FbxNode* pRootNode = pScene->GetRootNode();
    if (pRootNode)
    {
        PrintNodeInfo(pRootNode);
    }

    // 메모리 정리
    pManager->Destroy();

    std::cout << "Program completed successfully!" << std::endl;
    
    // 콘솔이 바로 닫히지 않도록 대기
    std::cout << "Press Enter to exit...";
    std::cin.get();
    
    return 0;
}
