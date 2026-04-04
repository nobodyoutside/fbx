#ifndef RENDERER_H
#define RENDERER_H

#include <glad/glad.h>
#include <vector>
#include "FbxLoader.h"
#include "Camera.h"

/// @brief 3D 렌더링 뷰 모드
enum class ViewMode {
    Normal,
    VertexColor,
    VertexNormal,
    Texture
};

/// @brief OpenGL 3.3 Core 기반 FBX 메시 렌더러.
/// VAO/VBO/IBO 관리, 셰이더 컴파일, 카메라 제어를 담당한다.
class Renderer {
public:
    Renderer();
    ~Renderer();

    void init();
    void setMeshes(const std::vector<MeshData>& meshes);
    void setViewMode(ViewMode mode);
    void draw(int width, int height);

    Camera camera;

private:
    struct MeshGL {
        GLuint vao = 0;
        GLuint vbo = 0;
        GLuint ibo = 0;
        int indexCount = 0;
        GLuint textureId = 0;
    };

    std::vector<MeshGL> mMeshGLs;
    GLuint mProgram = 0;
    ViewMode mViewMode = ViewMode::Normal;

    GLuint mLineProgram = 0;
    GLuint mGridVAO = 0, mGridVBO = 0;
    GLuint mAxesVAO = 0, mAxesVBO = 0;
    int    mGridVertexCount = 0;

    GLuint compileShader(GLenum type, const char* src);
    GLuint linkProgram(GLuint vert, GLuint frag);
    void uploadMesh(const MeshData& mesh);
    void cleanup();
    void initGridAndAxes();
    void drawLines(GLuint vao, int vertexCount, const glm::mat4& mvp);

public:
    bool showGrid   = true;
    bool showOrigin = true;
};

#endif // RENDERER_H
