cmake_minimum_required(VERSION 3.16)
project(QTFirst LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 先找 Qt（Core/Gui/Widgets/Charts）
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Gui Widgets Charts)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Gui Widgets Charts)

# 为 Qt Quick 单独再找 Qml / Quick / QuickControls2
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Quick Qml QuickControls2)

# ★ 你的源码实际在子目录 QTFirst/ 下
set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/QTFirst)

# =========================
# 1) 传统 Widgets 程序（QMainWindow 那个）
# =========================
add_executable(${PROJECT_NAME})

target_sources(${PROJECT_NAME} PRIVATE
    ${SRC_DIR}/main.cpp
    ${SRC_DIR}/Imp/QtApp.cpp
    ${SRC_DIR}/Imp/ChartView.cpp
    ${SRC_DIR}/Imp/CallOut.cpp
    ${SRC_DIR}/Imp/DrawLayer.cpp
    ${SRC_DIR}/Imp/CsvDataProvider.cpp
    ${SRC_DIR}/Imp/QtApp.ui

    ${SRC_DIR}/Headers/QtApp.h
    ${SRC_DIR}/Headers/ChartView.h
    ${SRC_DIR}/Headers/CallOut.h
    ${SRC_DIR}/Headers/DrawLayer.h
    ${SRC_DIR}/Headers/CsvDataProvider.h
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    WIN32_EXECUTABLE TRUE
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${SRC_DIR}/Headers
)

set(CMAKE_AUTOUIC_SEARCH_PATHS
    ${SRC_DIR}/Imp
    ${SRC_DIR}
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Gui
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Charts
)
# 注意：这里不需要 Quick / Qml / QuickControls2


# =========================
# 2) Qt Quick 程序（QML 入口）
# =========================
add_executable(QTFirst_Qml
    ${SRC_DIR}/main_qml.cpp
    ${SRC_DIR}/qml.qrc
)

set_target_properties(QTFirst_Qml PROPERTIES
    WIN32_EXECUTABLE TRUE
)

target_link_libraries(QTFirst_Qml PRIVATE
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Gui
    Qt${QT_VERSION_MAJOR}::Qml
    Qt${QT_VERSION_MAJOR}::Quick
    Qt${QT_VERSION_MAJOR}::QuickControls2
    # 如果以后要在 QML 里用 ChartView（QtCharts QML 版），再加：
    # Qt${QT_VERSION_MAJOR}::Charts
)
