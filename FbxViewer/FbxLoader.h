#ifndef FBXLOADER_H
#define FBXLOADER_H

#include <fbxsdk.h>
#include <vector>
#include <string>
#include <QVector3D>
#include <QVector2D>
#include <QVector4D>

struct Vertex {
    QVector3D position;
    QVector3D normal;
    QVector4D color;
    QVector2D uv;
};

struct MeshData {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::string texturePath;
};

class FbxLoader {
public:
    FbxLoader();
    ~FbxLoader();

    bool loadFile(const std::string& filename);
    const std::vector<MeshData>& getMeshes() const { return mMeshes; }
    
    float getAnimationStart() const { return mAnimStart; }
    float getAnimationEnd() const { return mAnimEnd; }
    void updateAnimation(float time);

private:
    void processNode(FbxNode* node);
    void processMesh(FbxMesh* mesh);

    FbxManager* mSdkManager;
    FbxScene* mScene;
    std::vector<MeshData> mMeshes;
    
    float mAnimStart;
    float mAnimEnd;
};

#endif // FBXLOADER_H
