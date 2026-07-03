#include "App.h"

int main(int argc, char* argv[]) {
    App app;
    if (argc > 1) {
        app.setInitialFile(argv[1]);
    }
    return app.run();
}
