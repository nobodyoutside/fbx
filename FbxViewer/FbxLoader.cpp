#include "FbxLoader.h"
#include <iostream>

FbxLoader::FbxLoader() : mSdkManager(nullptr), mScene(nullptr), mAnimStart(0), mAnimEnd(0) {
    mSdkManager = FbxManager::Create();
    FbxIOSettings* ios = FbxIOSettings::Create(mSdkManager, IOSROOT);
    mSdkManager->SetIOSettings(ios);
}

FbxLoader::~FbxLoader() {
    if (mSdkManager) mSdkManager->Destroy();
}

bool FbxLoader::loadFile(const std::string& filename) {
    mScene = FbxScene::Create(mSdkManager, "myScene");
    FbxImporter* importer = FbxImporter::Create(mSdkManager, "");

    if (!importer->Initialize(filename.c_str(), -1, mSdkManager->GetIOSettings())) {
        return false;
    }

    importer->Import(mScene);
    importer->Destroy();

    FbxGeometryConverter converter(mSdkManager);
    converter.Triangulate(mScene, true);

    processNode(mScene->GetRootNode());

    // Basic animation range
    FbxTimeSpan timeSpan;
    if (mScene->GetSrcObjectCount<FbxAnimStack>() > 0) {
        FbxAnimStack* animStack = mScene->GetSrcObject<FbxAnimStack>(0);
        FbxTimeSpan timeSpan;
        FbxTakeInfo* takeInfo = mScene->GetTakeInfo(animStack->GetName());
        if (takeInfo) {
            mAnimStart = (float)takeInfo->mLocalTimeSpan.GetStart().GetSecondDouble();
            mAnimEnd = (float)takeInfo->mLocalTimeSpan.GetStop().GetSecondDouble();
        }
    }

    return true;
}

void FbxLoader::processNode(FbxNode* node) {
    if (!node) return;

    FbxNodeAttribute* attribute = node->GetNodeAttribute();
    if (attribute && attribute->GetAttributeType() == FbxNodeAttribute::eMesh) {
        processMesh(node->GetMesh());
    }

    for (int i = 0; i < node->GetChildCount(); ++i) {
        processNode(node->GetChild(i));
    }
}

void FbxLoader::processMesh(FbxMesh* mesh) {
    MeshData data;
    int vertexCount = mesh->GetControlPointsCount();
    FbxVector4* controlPoints = mesh->GetControlPoints();

    // Vertices and Indices
    int polygonCount = mesh->GetPolygonCount();
    for (int i = 0; i < polygonCount; ++i) {
        for (int j = 0; j < 3; ++j) {
            int ctrlPointIndex = mesh->GetPolygonVertex(i, j);
            Vertex vertex;
            
            // Position
            vertex.position = QVector3D(
                (float)controlPoints[ctrlPointIndex][0],
                (float)controlPoints[ctrlPointIndex][1],
                (float)controlPoints[ctrlPointIndex][2]
            );

            // Normal
            FbxVector4 fbxNormal;
            if (mesh->GetPolygonVertexNormal(i, j, fbxNormal)) {
                vertex.normal = QVector3D((float)fbxNormal[0], (float)fbxNormal[1], (float)fbxNormal[2]);
            }

            // Color
            if (mesh->GetElementVertexColorCount() > 0) {
                const FbxGeometryElementVertexColor* elementColor = mesh->GetElementVertexColor(0);
                if (elementColor->GetMappingMode() == FbxLayerElement::eByPolygonVertex) {
                    int colorIndex = (elementColor->GetReferenceMode() == FbxLayerElement::eDirect) ? i * 3 + j : elementColor->GetIndexArray().GetAt(i * 3 + j);
                    FbxColor color = elementColor->GetDirectArray().GetAt(colorIndex);
                    vertex.color = QVector4D((float)color.mRed, (float)color.mGreen, (float)color.mBlue, (float)color.mAlpha);
                }
            } else {
                vertex.color = QVector4D(1.0f, 1.0f, 1.0f, 1.0f);
            }

            // UV
            FbxStringList uvSetNames;
            mesh->GetUVSetNames(uvSetNames);
            if (uvSetNames.GetCount() > 0) {
                FbxVector2 uv;
                bool unmapped;
                if (mesh->GetPolygonVertexUV(i, j, uvSetNames.GetStringAt(0), uv, unmapped)) {
                    vertex.uv = QVector2D((float)uv[0], (float)uv[1]);
                }
            }

            data.vertices.push_back(vertex);
            data.indices.push_back((unsigned int)data.vertices.size() - 1);
        }
    }

    // Texture Path (Rough implementation)
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

void FbxLoader::updateAnimation(float time) {
    // Animation update logic (complex for full skeletal, but basic node transform can be done)
    // For this scope, we focus on static mesh viewing and basic play bar UI.
}
