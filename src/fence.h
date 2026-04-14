#pragma once
#include <QString>

struct Fence {
    QString id;
    QString title;
    QString screen;
    int x = 0;
    int y = 0;
    int width = 400;
    int height = 300;
    bool rolledUp = false;
    int iconSize = 64;
};
