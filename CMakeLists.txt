cmake_minimum_required(VERSION 3.15)
project(GEditor)
set(CMAKE_CXX_STANDARD 17)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
INCLUDE(config.local OPTIONAL)
find_package(Qt5 REQUIRED COMPONENTS Core Widgets Gui)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

set(project_ui src/mainwindow.ui)
set(project_headers include/mainwindow.h include/processhandler.h)
set(project_sources src/main.cpp src/mainwindow.cpp src/processhandler.cpp)
set(project_resources resources/logg.qrc resources/geditor.rc resources/ProximaNova_rg.ttf)

add_executable(${PROJECT_NAME} ${project_sources} ${project_headers} ${project_ui} ${project_resources})

target_link_libraries(${PROJECT_NAME} PUBLIC Qt5::Core Qt5::Core Qt5::Widgets)