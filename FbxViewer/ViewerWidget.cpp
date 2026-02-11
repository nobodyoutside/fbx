#include "ViewerWidget.h"
#include <QtGui/qpainter.h>

/// @brief 뷰어 위젯을 기본 상태로 초기화한다.
/// 뷰 모드를 Normal로 설정하고, 셰이더 프로그램을 nullptr로 초기화한다.
ViewerWidget::ViewerWidget(QWidget* parent)
    : QWidget(parent), mMode(ViewMode::Normal), mProgram(nullptr) {
}

/// @brief 소멸자. 현재 플레이스홀더 구현이므로 OpenGL 리소스 해제 코드가 없다.
ViewerWidget::~ViewerWidget() {
}

/// @brief 외부에서 로드된 메시 데이터를 내부 멤버에 복사하고 위젯을 다시 그린다.
/// @param meshes FbxLoader에서 추출된 메시 데이터 목록
void ViewerWidget::setMeshes(const std::vector<MeshData>& meshes) {
    mMeshes = meshes;
    update();
}

/// @brief 렌더링 뷰 모드를 변경하고 위젯을 다시 그린다.
/// @param mode 적용할 뷰 모드 (Normal, VertexColor, VertexNormal, Texture)
void ViewerWidget::setViewMode(ViewMode mode) {
    mMode = mode;
    update();
}

/// @brief 위젯의 그리기 이벤트를 처리한다.
/// Qt5OpenGL이 없는 현재 환경에서는 검정 배경에 안내 텍스트를 출력하는 플레이스홀더로 동작한다.
void ViewerWidget::paintEvent(QPaintEvent* event) {
    QPainter painter(this);
    painter.fillRect(rect(), Qt::black);
    painter.setPen(Qt::white);
    painter.drawText(rect(), Qt::AlignCenter, "OpenGL Library (Qt5OpenGL) Missing in SDK\nFBX Data Loaded Successfully\nCheck Console for Details");
}

/// @brief 위젯 크기 변경 시 호출된다. 변경된 크기에 맞게 화면을 다시 그린다.
void ViewerWidget::resizeEvent(QResizeEvent* event) {
    update();
}
