#ifndef APP_H
#define APP_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "FbxLoader.h"
#include "Renderer.h"

/// @brief GLFW 윈도우 + ImGui + 렌더러를 통합 관리하는 애플리케이션 클래스.
/// 메인 루프, 입력 처리, ImGui UI 패널을 담당한다.
class App {
public:
    App();
    ~App();

    int run();
    void setInitialFile(const char* path);

private:
    bool initWindow();
    void mainLoop();
    void drawImGuiPanel();
    void openFileDialog();

    static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
    static void cursorPosCallback(GLFWwindow* window, double xpos, double ypos);
    static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);

    GLFWwindow* mWindow = nullptr;
    FbxLoader mLoader;
    Renderer mRenderer;

    // UI 상태
    int mViewModeIndex = 0;
    float mAnimTime = 0.0f;
    bool mFileLoaded = false;
    char mFilePath[512] = {};

    // 마우스 상태
    bool mDraggingLeft = false;
    bool mDraggingMiddle = false;
    double mLastMouseX = 0.0;
    double mLastMouseY = 0.0;

    std::string mInitialFile;
};

#endif // APP_H
