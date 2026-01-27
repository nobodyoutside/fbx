#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSlider>
#include <QPushButton>
#include <QComboBox>
#include "ViewerWidget.h"
#include "FbxLoader.h"

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private slots:
    void openFile();
    void changeMode(int index);
    void updateAnimation(int value);

private:
    ViewerWidget* mViewer;
    QSlider* mAnimSlider;
    FbxLoader mLoader;
};

#endif // MAINWINDOW_H
