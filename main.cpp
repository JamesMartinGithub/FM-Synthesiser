#include "Controller.h"

int main(int argc, char *argv[]) {
    Controller* controller = new Controller();
    return controller->Start(argc, argv);
}