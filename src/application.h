#pragma once
#include <QGuiApplication>

class Application : public QGuiApplication {
    Q_OBJECT
public:
    explicit Application(int &argc, char **argv);
    ~Application() override;

    bool init();
};
