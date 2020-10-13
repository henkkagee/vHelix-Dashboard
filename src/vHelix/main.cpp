/*
 *  Copyright (c) 2020 Henrik Granö
 *  See the license in the root directory for the full notice
*/

#include "mainwindow.h"
#include "vhelix.h"

#include <QApplication>
#include <QObject>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    vHelix vh;
    // connect signals and slots between the main window and most of the program logic
    QObject::connect(&w, &MainWindow::sendMesh_,
                     &vh, &vHelix::readMesh_);
    QObject::connect(&vh, &vHelix::sendToConsole_,
                     &w, &MainWindow::vHelixToWindow_);
    QObject::connect(&w, &MainWindow::action_,
                     &vh, &vHelix::action_);
    w.show();
    return a.exec();
}
