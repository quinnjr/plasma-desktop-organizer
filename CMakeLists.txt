cmake_minimum_required(VERSION 3.16)
project(plasma-desktop-organizer)

find_package(ECM REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

include(KDEInstallDirs)

find_package(Plasma REQUIRED)

plasma_install_package(package org.kde.plasma.desktoporganizer)
