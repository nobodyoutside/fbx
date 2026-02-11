#include "MainWindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QLabel>
#include <QMessageBox>

/// @brief 메인 윈도우 UI를 구성한다.
/// 상단 툴바(파일 열기 버튼 + 뷰 모드 콤보박스), 중앙 뷰어 위젯, 하단 애니메이션 슬라이더를
/// 수직 레이아웃으로 배치하고, 각 위젯의 시그널을 해당 슬롯에 연결한다.
MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);

    // 상단 툴바 영역: 파일 열기 버튼 + 뷰 모드 선택 콤보박스
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

    // 중앙 3D 뷰어 위젯 (stretch factor 1로 남은 공간을 모두 차지)
    mViewer = new ViewerWidget(this);

    // 하단 애니메이션 타임라인 슬라이더
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

/// @brief 소멸자. Qt의 부모-자식 소유권 체계에 의해 자식 위젯들은 자동 해제된다.
MainWindow::~MainWindow() {}

/// @brief 파일 열기 다이얼로그를 표시하고, 사용자가 선택한 FBX 파일을 FbxLoader로 로드한다.
/// 로드 성공 시 메시 데이터를 ViewerWidget에 전달하고, 애니메이션이 있으면 슬라이더 범위를 갱신한다.
void MainWindow::openFile() {
    QString fileName = QFileDialog::getOpenFileName(this, "Open FBX File", "", "FBX Files (*.fbx)");
    if (!fileName.isEmpty()) {
        if (mLoader.loadFile(fileName.toStdString())) {
            // 로드된 메시 데이터를 뷰어에 전달하여 렌더링 갱신
            mViewer->setMeshes(mLoader.getMeshes());
            // 애니메이션 시간 범위가 유효하면 슬라이더를 1000단계로 세분화
            float start = mLoader.getAnimationStart();
            float end = mLoader.getAnimationEnd();
            if (end > start) {
                mAnimSlider->setRange(0, 1000);
            }
        } else {
            QMessageBox::warning(this, "Load Failed",
                QString("Failed to load FBX file:\n%1").arg(fileName));
        }
    }
}

/// @brief 뷰 모드 콤보박스의 선택이 변경되면, 인덱스를 ViewMode 열거형으로 캐스팅하여 뷰어에 전달한다.
/// @param index 콤보박스 선택 인덱스 (0=Normal, 1=VertexColor, 2=VertexNormal, 3=Texture)
void MainWindow::changeMode(int index) {
    mViewer->setViewMode(static_cast<ViewMode>(index));
}

/// @brief 슬라이더 값(0~1000)을 애니메이션 시간 범위에 선형 보간하여 FbxLoader에 전달하고,
/// 뷰어를 다시 그리도록 요청한다.
/// @param value 슬라이더의 현재 값
void MainWindow::updateAnimation(int value) {
    float t = mLoader.getAnimationStart() + (mLoader.getAnimationEnd() - mLoader.getAnimationStart()) * (value / 1000.0f);
    mLoader.updateAnimation(t);
    mViewer->update();
}
