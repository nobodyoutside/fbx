#include "Renderer.h"
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <vector>

// ─── 내장 셰이더 (OpenGL 3.3 Core) ─────────────────────────────

static const char* kVertexShader = R"glsl(
#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec4 aColor;
layout(location = 3) in vec2 aUV;

uniform mat4 uMVP;
uniform mat4 uModel;

out vec3 vNormal;
out vec4 vColor;
out vec2 vUV;
out vec3 vWorldPos;

void main() {
    gl_Position = uMVP * vec4(aPos, 1.0);
    vWorldPos = vec3(uModel * vec4(aPos, 1.0));
    vNormal = mat3(transpose(inverse(uModel))) * aNormal;
    vColor = aColor;
    vUV = aUV;
}
)glsl";

static const char* kFragmentShader = R"glsl(
#version 330 core
in vec3 vNormal;
in vec4 vColor;
in vec2 vUV;
in vec3 vWorldPos;

uniform int uViewMode;
uniform sampler2D uTex;
uniform vec3 uLightDir;

out vec4 FragColor;

void main() {
    vec3 N = normalize(vNormal);

    if (uViewMode == 1) {
        // VertexColor
        FragColor = vColor;
    } else if (uViewMode == 2) {
        // VertexNormal 시각화
        FragColor = vec4(N * 0.5 + 0.5, 1.0);
    } else if (uViewMode == 3) {
        // Texture
        FragColor = texture(uTex, vUV);
    } else {
        // Normal: 람베르트 조명
        float diff = max(dot(N, normalize(uLightDir)), 0.0);
        float ambient = 0.15;
        vec3 color = vec3(0.8, 0.8, 0.8) * (ambient + diff);
        FragColor = vec4(color, 1.0);
    }
}
)glsl";

// ─── 라인 셰이더 (그리드 / 축) ──────────────────────────────────

static const char* kLineVertexShader = R"glsl(
#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aColor;

uniform mat4 uMVP;

out vec3 vColor;

void main() {
    gl_Position = uMVP * vec4(aPos, 1.0);
    vColor = aColor;
}
)glsl";

static const char* kLineFragmentShader = R"glsl(
#version 330 core
in vec3 vColor;
out vec4 FragColor;

void main() {
    FragColor = vec4(vColor, 1.0);
}
)glsl";

// ─── 구현부 ─────────────────────────────────────────────────────

Renderer::Renderer() = default;

Renderer::~Renderer() {
    cleanup();
    if (mProgram)     glDeleteProgram(mProgram);
    if (mLineProgram) glDeleteProgram(mLineProgram);
    if (mGridVAO)     glDeleteVertexArrays(1, &mGridVAO);
    if (mGridVBO)     glDeleteBuffers(1, &mGridVBO);
    if (mAxesVAO)     glDeleteVertexArrays(1, &mAxesVAO);
    if (mAxesVBO)     glDeleteBuffers(1, &mAxesVBO);
}

void Renderer::init() {
    GLuint vs = compileShader(GL_VERTEX_SHADER, kVertexShader);
    GLuint fs = compileShader(GL_FRAGMENT_SHADER, kFragmentShader);
    mProgram = linkProgram(vs, fs);
    glDeleteShader(vs);
    glDeleteShader(fs);

    GLuint lvs = compileShader(GL_VERTEX_SHADER, kLineVertexShader);
    GLuint lfs = compileShader(GL_FRAGMENT_SHADER, kLineFragmentShader);
    mLineProgram = linkProgram(lvs, lfs);
    glDeleteShader(lvs);
    glDeleteShader(lfs);

    glEnable(GL_DEPTH_TEST);
    initGridAndAxes();
}

GLuint Renderer::compileShader(GLenum type, const char* src) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char log[512];
        glGetShaderInfoLog(shader, sizeof(log), nullptr, log);
        std::cerr << "Shader compile error: " << log << std::endl;
    }
    return shader;
}

GLuint Renderer::linkProgram(GLuint vert, GLuint frag) {
    GLuint prog = glCreateProgram();
    glAttachShader(prog, vert);
    glAttachShader(prog, frag);
    glLinkProgram(prog);

    GLint success;
    glGetProgramiv(prog, GL_LINK_STATUS, &success);
    if (!success) {
        char log[512];
        glGetProgramInfoLog(prog, sizeof(log), nullptr, log);
        std::cerr << "Program link error: " << log << std::endl;
    }
    return prog;
}

void Renderer::cleanup() {
    for (auto& gl : mMeshGLs) {
        if (gl.vao) glDeleteVertexArrays(1, &gl.vao);
        if (gl.vbo) glDeleteBuffers(1, &gl.vbo);
        if (gl.ibo) glDeleteBuffers(1, &gl.ibo);
        if (gl.textureId) glDeleteTextures(1, &gl.textureId);
    }
    mMeshGLs.clear();
}

void Renderer::setMeshes(const std::vector<MeshData>& meshes) {
    cleanup();
    for (const auto& mesh : meshes) {
        uploadMesh(mesh);
    }
}

void Renderer::uploadMesh(const MeshData& mesh) {
    MeshGL gl;
    gl.indexCount = static_cast<int>(mesh.indices.size());

    glGenVertexArrays(1, &gl.vao);
    glGenBuffers(1, &gl.vbo);
    glGenBuffers(1, &gl.ibo);

    glBindVertexArray(gl.vao);

    glBindBuffer(GL_ARRAY_BUFFER, gl.vbo);
    glBufferData(GL_ARRAY_BUFFER,
        mesh.vertices.size() * sizeof(Vertex),
        mesh.vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gl.ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
        mesh.indices.size() * sizeof(unsigned int),
        mesh.indices.data(), GL_STATIC_DRAW);

    // position (location 0)
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
        reinterpret_cast<void*>(offsetof(Vertex, position)));

    // normal (location 1)
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
        reinterpret_cast<void*>(offsetof(Vertex, normal)));

    // color (location 2)
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex),
        reinterpret_cast<void*>(offsetof(Vertex, color)));

    // uv (location 3)
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
        reinterpret_cast<void*>(offsetof(Vertex, uv)));

    glBindVertexArray(0);

    mMeshGLs.push_back(gl);
}

void Renderer::setViewMode(ViewMode mode) {
    mViewMode = mode;
}

void Renderer::initGridAndAxes() {
    struct LineVertex { float x, y, z, r, g, b; };

    // ── 그리드 (XZ 평면, ±10 범위, 1단위 간격) ─────────────────
    std::vector<LineVertex> gridVerts;
    const int N = 10;
    const float dim = static_cast<float>(N);
    for (int i = -N; i <= N; ++i) {
        float t = static_cast<float>(i);
        // Z축 방향 라인 (X 고정)
        gridVerts.push_back({t, 0.0f, -dim, 0.4f, 0.4f, 0.4f});
        gridVerts.push_back({t, 0.0f,  dim, 0.4f, 0.4f, 0.4f});
        // X축 방향 라인 (Z 고정)
        gridVerts.push_back({-dim, 0.0f, t, 0.4f, 0.4f, 0.4f});
        gridVerts.push_back({ dim, 0.0f, t, 0.4f, 0.4f, 0.4f});
    }
    mGridVertexCount = static_cast<int>(gridVerts.size());

    glGenVertexArrays(1, &mGridVAO);
    glGenBuffers(1, &mGridVBO);
    glBindVertexArray(mGridVAO);
    glBindBuffer(GL_ARRAY_BUFFER, mGridVBO);
    glBufferData(GL_ARRAY_BUFFER, gridVerts.size() * sizeof(LineVertex),
                 gridVerts.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(LineVertex),
                          reinterpret_cast<void*>(0));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(LineVertex),
                          reinterpret_cast<void*>(3 * sizeof(float)));
    glBindVertexArray(0);

    // ── 원점 축 (X=빨강, Y=초록, Z=파랑, ±5) ───────────────────
    const float A = 5.0f;
    const LineVertex axesVerts[] = {
        {-A, 0, 0,  0.8f, 0.2f, 0.2f}, { A, 0, 0,  0.8f, 0.2f, 0.2f},  // X
        { 0,-A, 0,  0.2f, 0.8f, 0.2f}, { 0, A, 0,  0.2f, 0.8f, 0.2f},  // Y
        { 0, 0,-A,  0.2f, 0.4f, 0.9f}, { 0, 0, A,  0.2f, 0.4f, 0.9f},  // Z
    };

    glGenVertexArrays(1, &mAxesVAO);
    glGenBuffers(1, &mAxesVBO);
    glBindVertexArray(mAxesVAO);
    glBindBuffer(GL_ARRAY_BUFFER, mAxesVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(axesVerts), axesVerts, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(LineVertex),
                          reinterpret_cast<void*>(0));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(LineVertex),
                          reinterpret_cast<void*>(3 * sizeof(float)));
    glBindVertexArray(0);
}

void Renderer::drawLines(GLuint vao, int vertexCount, const glm::mat4& mvp) {
    glUniformMatrix4fv(glGetUniformLocation(mLineProgram, "uMVP"), 1, GL_FALSE,
                       glm::value_ptr(mvp));
    glBindVertexArray(vao);
    glDrawArrays(GL_LINES, 0, vertexCount);
    glBindVertexArray(0);
}

void Renderer::draw(int width, int height) {
    glViewport(0, 0, width, height);
    glClearColor(0.15f, 0.15f, 0.18f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    float aspect = (height > 0) ? static_cast<float>(width) / height : 1.0f;
    glm::mat4 view = camera.viewMatrix();
    glm::mat4 proj = camera.projMatrix(aspect);
    glm::mat4 mvp  = proj * view * glm::mat4(1.0f);

    // ── 그리드 / 축 ─────────────────────────────────────────────
    if (mLineProgram && (showGrid || showOrigin)) {
        glUseProgram(mLineProgram);
        if (showGrid)   drawLines(mGridVAO, mGridVertexCount, mvp);
        if (showOrigin) drawLines(mAxesVAO, 6, mvp);
        glUseProgram(0);
    }

    // ── 메시 ────────────────────────────────────────────────────
    if (!mProgram || mMeshGLs.empty()) return;

    glUseProgram(mProgram);

    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 meshMvp = proj * view * model;

    glUniformMatrix4fv(glGetUniformLocation(mProgram, "uMVP"), 1, GL_FALSE, glm::value_ptr(meshMvp));
    glUniformMatrix4fv(glGetUniformLocation(mProgram, "uModel"), 1, GL_FALSE, glm::value_ptr(model));
    glUniform1i(glGetUniformLocation(mProgram, "uViewMode"), static_cast<int>(mViewMode));
    glUniform3f(glGetUniformLocation(mProgram, "uLightDir"), 0.5f, 1.0f, 0.3f);

    for (const auto& gl : mMeshGLs) {
        glBindVertexArray(gl.vao);
        glDrawElements(GL_TRIANGLES, gl.indexCount, GL_UNSIGNED_INT, nullptr);
    }

    glBindVertexArray(0);
    glUseProgram(0);
}
