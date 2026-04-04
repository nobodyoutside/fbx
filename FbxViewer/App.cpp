#include "App.h"
#include <glad/glad.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <iostream>
#include <cstring>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <commdlg.h>
#endif

App::App() = default;

App::~App() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    if (mWindow) {
        glfwDestroyWindow(mWindow);
    }
    glfwTerminate();
}

int App::run() {
    if (!initWindow()) return -1;
    mRenderer.init();
    mainLoop();
    return 0;
}

bool App::initWindow() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return false;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    mWindow = glfwCreateWindow(1280, 720, "FBX Viewer", nullptr, nullptr);
    if (!mWindow) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(mWindow);
    glfwSwapInterval(1);

    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return false;
    }

    // GLFW 콜백 등록
    glfwSetWindowUserPointer(mWindow, this);
    glfwSetMouseButtonCallback(mWindow, mouseButtonCallback);
    glfwSetCursorPosCallback(mWindow, cursorPosCallback);
    glfwSetScrollCallback(mWindow, scrollCallback);

    // ImGui 초기화
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(mWindow, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    return true;
}

void App::mainLoop() {
    while (!glfwWindowShouldClose(mWindow)) {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        drawImGuiPanel();

        ImGui::Render();

        int w, h;
        glfwGetFramebufferSize(mWindow, &w, &h);
        mRenderer.draw(w, h);

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(mWindow);
    }
}

void App::drawImGuiPanel() {
    ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(320, 220), ImGuiCond_FirstUseEver);
    ImGui::Begin("Controls");

    // 파일 열기
    if (ImGui::Button("Open FBX...")) {
        openFileDialog();
    }
    if (mFileLoaded) {
        ImGui::SameLine();
        ImGui::TextWrapped("%s", mFilePath);
    }

    ImGui::Separator();

    // 뷰 모드 선택
    const char* modes[] = {"Normal", "VertexColor", "VertexNormal", "Texture"};
    if (ImGui::Combo("View Mode", &mViewModeIndex, modes, IM_ARRAYSIZE(modes))) {
        mRenderer.setViewMode(static_cast<ViewMode>(mViewModeIndex));
    }

    // 애니메이션 슬라이더
    float animStart = mLoader.getAnimationStart();
    float animEnd = mLoader.getAnimationEnd();
    if (animEnd > animStart) {
        if (ImGui::SliderFloat("Animation", &mAnimTime, animStart, animEnd, "%.2f s")) {
            mLoader.updateAnimation(mAnimTime);
        }
    }

    ImGui::Separator();

    // 카메라 정보 표시
    ImGui::Text("Camera: yaw=%.1f pitch=%.1f dist=%.1f",
        mRenderer.camera.yaw, mRenderer.camera.pitch, mRenderer.camera.distance);
    if (ImGui::Button("Reset Camera")) {
        mRenderer.camera = Camera{};
    }

    ImGui::Separator();
    ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f),
        "LMB: orbit, MMB: pan, Scroll: zoom");

    ImGui::End();
}

void App::openFileDialog() {
#ifdef _WIN32
    OPENFILENAMEA ofn = {};
    char file[512] = {};
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = nullptr;
    ofn.lpstrFilter = "FBX Files\0*.fbx\0All Files\0*.*\0";
    ofn.lpstrFile = file;
    ofn.nMaxFile = sizeof(file);
    ofn.Flags = OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

    if (GetOpenFileNameA(&ofn)) {
        if (mLoader.loadFile(file)) {
            mRenderer.setMeshes(mLoader.getMeshes());
            mAnimTime = mLoader.getAnimationStart();
            mFileLoaded = true;
            std::strncpy(mFilePath, file, sizeof(mFilePath) - 1);
        } else {
            std::cerr << "Failed to load: " << file << std::endl;
        }
    }
#else
    std::cerr << "File dialog not implemented on this platform" << std::endl;
#endif
}

// ─── GLFW 콜백 ─────────────────────────────────────────────────

void App::mouseButtonCallback(GLFWwindow* window, int button, int action, int /*mods*/) {
    // ImGui가 마우스를 캡처 중이면 3D 뷰포트 조작 무시
    if (ImGui::GetIO().WantCaptureMouse) return;

    auto* app = static_cast<App*>(glfwGetWindowUserPointer(window));
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        app->mDraggingLeft = (action == GLFW_PRESS);
    }
    if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
        app->mDraggingMiddle = (action == GLFW_PRESS);
    }
    if (action == GLFW_PRESS) {
        glfwGetCursorPos(window, &app->mLastMouseX, &app->mLastMouseY);
    }
}

void App::cursorPosCallback(GLFWwindow* window, double xpos, double ypos) {
    if (ImGui::GetIO().WantCaptureMouse) return;

    auto* app = static_cast<App*>(glfwGetWindowUserPointer(window));
    float dx = static_cast<float>(xpos - app->mLastMouseX);
    float dy = static_cast<float>(ypos - app->mLastMouseY);
    app->mLastMouseX = xpos;
    app->mLastMouseY = ypos;

    if (app->mDraggingLeft) {
        app->mRenderer.camera.orbit(dx * 0.3f, dy * 0.3f);
    }
    if (app->mDraggingMiddle) {
        app->mRenderer.camera.pan(-dx, dy);
    }
}

void App::scrollCallback(GLFWwindow* window, double /*xoffset*/, double yoffset) {
    if (ImGui::GetIO().WantCaptureMouse) return;

    auto* app = static_cast<App*>(glfwGetWindowUserPointer(window));
    app->mRenderer.camera.zoom(static_cast<float>(yoffset) * 0.5f);
}
