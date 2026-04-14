#include "application.h"
#include <QDebug>

int main(int argc, char *argv[])
{
    Application app(argc, argv);
    if (!app.init()) {
        return 1;
    }
    return 0; // will become app.exec() in Task 6
}
