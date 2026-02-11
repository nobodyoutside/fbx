#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSlider>
#include <QPushButton>
#include <QComboBox>
#include "ViewerWidget.h"
#include "FbxLoader.h"

/// @brief FBX 뷰어의 메인 윈도우. 파일 열기, 뷰 모드 전환, 애니메이션 슬라이더 등 UI를 관리한다.
class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    /// @brief 메인 윈도우의 레이아웃(툴바, 뷰어, 애니메이션 바)을 구성하고 시그널/슬롯을 연결한다.
    MainWindow(QWidget* parent = nullptr);

    /// @brief 소멸자
    ~MainWindow();

private slots:
    /// @brief 파일 열기 다이얼로그를 표시하고 선택된 FBX 파일을 로드하여 뷰어에 전달한다.
    void openFile();

    /// @brief 뷰 모드 콤보박스 변경 시 호출되어 ViewerWidget의 렌더링 모드를 전환한다.
    /// @param index 콤보박스에서 선택된 항목의 인덱스 (ViewMode 열거형과 1:1 대응)
    void changeMode(int index);

    /// @brief 애니메이션 슬라이더 값 변경 시 호출되어 FbxLoader의 애니메이션 시간을 갱신한다.
    /// @param value 슬라이더 값 (0~1000)
    void updateAnimation(int value);

private:
    ViewerWidget* mViewer;    // 3D 렌더링 위젯
    QSlider* mAnimSlider;     // 애니메이션 타임라인 슬라이더
    FbxLoader mLoader;        // FBX 파일 로더
};

#endif // MAINWINDOW_H
