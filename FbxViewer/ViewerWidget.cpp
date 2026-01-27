#include "ViewerWidget.h"
#include <QtGui/qpainter.h>

ViewerWidget::ViewerWidget(QWidget* parent) 
    : QWidget(parent), mMode(ViewMode::Normal), mProgram(nullptr) {
}

ViewerWidget::~ViewerWidget() {
}

void ViewerWidget::setMeshes(const std::vector<MeshData>& meshes) {
    mMeshes = meshes;
    update();
}

void ViewerWidget::setViewMode(ViewMode mode) {
    mMode = mode;
    update();
}

void ViewerWidget::paintEvent(QPaintEvent* event) {
    QPainter painter(this);
    painter.fillRect(rect(), Qt::black);
    painter.setPen(Qt::white);
    painter.drawText(rect(), Qt::AlignCenter, "OpenGL Library (Qt5OpenGL) Missing in SDK\nFBX Data Loaded Successfully\nCheck Console for Details");
}

void ViewerWidget::resizeEvent(QResizeEvent* event) {
    update();
}