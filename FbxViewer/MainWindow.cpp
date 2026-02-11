#include "MainWindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QLabel>
#include <QMessageBox>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);

    // Toolbar area
    QHBoxLayout* toolLayout = new QHBoxLayout();
    QPushButton* openBtn = new QPushButton("Open FBX", this);
    connect(openBtn, &QPushButton::clicked, this, &MainWindow::openFile);
    
    QComboBox* modeCombo = new QComboBox(this);
    modeCombo->addItem("Normal");
    modeCombo->addItem("Vertex Color");
    modeCombo->addItem("Vertex Normal");
    modeCombo->addItem("Texture");
    connect(modeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::changeMode);

    toolLayout->addWidget(openBtn);
    toolLayout->addWidget(new QLabel("View Mode:"));
    toolLayout->addWidget(modeCombo);
    toolLayout->addStretch();

    // Viewer
    mViewer = new ViewerWidget(this);

    // Animation bar
    QHBoxLayout* animLayout = new QHBoxLayout();
    mAnimSlider = new QSlider(Qt::Horizontal, this);
    mAnimSlider->setRange(0, 100);
    connect(mAnimSlider, &QSlider::valueChanged, this, &MainWindow::updateAnimation);
    
    animLayout->addWidget(new QLabel("Animation:"));
    animLayout->addWidget(mAnimSlider);

    mainLayout->addLayout(toolLayout);
    mainLayout->addWidget(mViewer, 1);
    mainLayout->addLayout(animLayout);

    resize(1024, 768);
    setWindowTitle("FBX Viewer");
}

MainWindow::~MainWindow() {}

void MainWindow::openFile() {
    QString fileName = QFileDialog::getOpenFileName(this, "Open FBX File", "", "FBX Files (*.fbx)");
    if (!fileName.isEmpty()) {
        if (mLoader.loadFile(fileName.toStdString())) {
            mViewer->setMeshes(mLoader.getMeshes());
            // Update slider range if animation exists
            float start = mLoader.getAnimationStart();
            float end = mLoader.getAnimationEnd();
            if (end > start) {
                mAnimSlider->setRange(0, 1000); // 1000 steps
            }
        } else {
            QMessageBox::warning(this, "Load Failed",
                QString("Failed to load FBX file:\n%1").arg(fileName));
        }
    }
}

void MainWindow::changeMode(int index) {
    mViewer->setViewMode(static_cast<ViewMode>(index));
}

void MainWindow::updateAnimation(int value) {
    float t = mLoader.getAnimationStart() + (mLoader.getAnimationEnd() - mLoader.getAnimationStart()) * (value / 1000.0f);
    mLoader.updateAnimation(t);
    mViewer->update();
}
