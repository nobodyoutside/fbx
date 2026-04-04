#include "App.h"

int main(int argc, char* argv[]) {
    App app;
    if (argc > 1) {
        // 커맨드라인으로 전달된 FBX 파일을 윈도우 초기화 후 자동 로드
        // run() 내부에서 initWindow + mainLoop를 하므로, 별도 처리 필요
        // → run()을 수정하지 않고, 윈도우 표시 전에 로드하기 위해
        //   mainLoop 진입 직전에 로드하는 방식으로 구현
        app.loadFromPath(argv[1]);
    }
    return app.run();
}
