#include <QApplication>
#include "MainWindow.h"

/// @brief FBX 뷰어 애플리케이션의 진입점.
/// QApplication을 생성하고, 메인 윈도우를 표시한 뒤 Qt 이벤트 루프를 실행한다.
int main(int argc, char* argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
