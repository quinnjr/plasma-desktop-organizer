cmake_minimum_required(VERSION 3.22)
project(plasma-desktop-organizer VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

find_package(ECM 6.0 REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Quick QuickControls2 Qml)
find_package(KF6 REQUIRED COMPONENTS
    CoreAddons
    Config
    KIO
    IconThemes
    Svg
    StatusNotifierItem
    WindowSystem
)
find_package(LayerShellQt REQUIRED)

add_library(organizer_core STATIC
    src/application.cpp
    src/fencemanager.cpp
    src/fencemodel.cpp
    src/fencefilemodel.cpp
    src/layershellwindow.cpp
)
target_include_directories(organizer_core PUBLIC src)
target_compile_definitions(organizer_core PRIVATE
    PROJECT_VERSION_STRING="${PROJECT_VERSION}"
)
target_link_libraries(organizer_core PUBLIC
    Qt6::Core
    Qt6::Gui
    Qt6::Quick
    KF6::CoreAddons
)
target_link_libraries(organizer_core PRIVATE
    KF6::KIOCore
    KF6::IconThemes
    KF6::ConfigCore
    LayerShellQt::Interface
)

qt_add_resources(QML_RESOURCES qml/qml.qrc)

add_executable(plasma-organizer
    src/main.cpp
    ${QML_RESOURCES}
)
target_link_libraries(plasma-organizer PRIVATE
    organizer_core
    Qt6::Core
    Qt6::Gui
    Qt6::Quick
    Qt6::QuickControls2
    Qt6::Qml
    KF6::ConfigCore
    KF6::IconThemes
    KF6::Svg
    KF6::StatusNotifierItem
    KF6::WindowSystem
    LayerShellQt::Interface
)

install(TARGETS plasma-organizer
    DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(BUILD_TESTING)
    enable_testing()
    add_subdirectory(tests)
endif()
