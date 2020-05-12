#include <include/mainwindow.h>
#include "ui_mainwindow.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    MainWindow ui;
    ui.show();
    return QApplication::exec();
}