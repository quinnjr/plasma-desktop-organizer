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

option(BUILD_COVERAGE "Enable gcov coverage instrumentation" OFF)

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
    KF6::ConfigCore
    KF6::KIOCore
    KF6::IconThemes
    KF6::StatusNotifierItem
    Qt6::Qml
    LayerShellQt::Interface
)

if(BUILD_COVERAGE)
    target_compile_options(organizer_core PRIVATE --coverage -O0 -g)
    target_link_options(organizer_core PRIVATE --coverage)
endif()

qt_add_resources(QML_RESOURCES qml/qml.qrc)

add_executable(plasma-organizer
    src/main.cpp
    ${QML_RESOURCES}
)
target_link_libraries(plasma-organizer PRIVATE
    organizer_core
    Qt6::QuickControls2
    Qt6::Qml
    KF6::Svg
    KF6::StatusNotifierItem
    KF6::WindowSystem
    LayerShellQt::Interface
)

install(TARGETS plasma-organizer
    DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(FILES data/plasma-organizer.desktop
    DESTINATION ${KDE_INSTALL_AUTOSTARTDIR}
)

if(BUILD_TESTING)
    enable_testing()
    add_subdirectory(tests)
endif()

if(BUILD_COVERAGE)
    find_program(LCOV_BIN lcov REQUIRED)
    find_program(GENHTML_BIN genhtml REQUIRED)
    # When building with clang, gcov is the GCC tool which cannot read clang's
    # gcno/gcda files.  Use llvm-cov as a drop-in gcov replacement via a thin
    # wrapper script so that lcov's --gcov-tool flag works (it must be a single
    # executable path, not "llvm-cov gcov").
    set(LLVM_GCOV_WRAPPER ${CMAKE_SOURCE_DIR}/cmake/llvm-gcov.sh)
    add_custom_target(coverage
        COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure -C $<CONFIG>
        # Step 1: capture raw coverage data
        COMMAND ${LCOV_BIN}
                --capture
                --gcov-tool ${LLVM_GCOV_WRAPPER}
                --ignore-errors mismatch,source,path,gcov,inconsistent,unsupported
                --directory ${CMAKE_BINARY_DIR}
                --output-file ${CMAKE_BINARY_DIR}/coverage.raw.info
        # Step 2: remove unwanted paths using --remove (patterns are not
        # shell-glob-expanded; lcov treats them as fnmatch patterns)
        COMMAND ${LCOV_BIN}
                --remove ${CMAKE_BINARY_DIR}/coverage.raw.info
                --ignore-errors mismatch,source,unused
                --output-file ${CMAKE_BINARY_DIR}/coverage.info
                "${CMAKE_BINARY_DIR}/*"
                "*/tests/*"
                "/usr/*"
                "*/application.cpp"
                "*/layershellwindow.cpp"
                "*/main.cpp"
        COMMAND ${GENHTML_BIN}
                ${CMAKE_BINARY_DIR}/coverage.info
                --output-directory ${CMAKE_BINARY_DIR}/coverage_html
                --legend --show-details
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMENT "Running tests and generating HTML coverage report"
        DEPENDS test_fencemanager test_fencemodel test_fencefilemodel
    )
endif()
