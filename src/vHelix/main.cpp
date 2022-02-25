/*
 *  Copyright (c) 2020 Henrik Granö
 *  See the license in the root directory for the full notice
 *
 * Starts the program. The user interface and most logic functions are separated as follows:
 *
 *  - VHelix: routing, relaxation, Python API interaction and directory selection
 *      (- processIO, for running child processes/executables in windows systems. This is not used but left here in case it might be needed)
 *  - MainWindow: user interface, sequence- and export functions
 *      - Console for a custom implementation of the text console
 *      - DirView for a custom directory tree view of the workspace
 *      - RpolyParser (should be renamed parser) which communicates with MainWindow for reading models (rpoly or oxdna)
 *  - Model, which contains Helix, Strand and Base model classes
 *  - DNA contains a function for calculating base positions for a given helix
 *
 * The communication between vHelix and MainWindow is done through Qt slots and signals, connected below. This minimizes the interface between them, but is not convenient for all
 * data types.
*/

#include "mainwindow.h"
#include "vhelix.h"

#include <QApplication>
#include <QFile>
#include <QTextStream>
#include <QObject>
#include <QThread>


int main(int argc, char *argv[])
{
    QCoreApplication::addLibraryPath("./");
    QApplication a(argc, argv);

    // set stylesheet
    QFile file(":/dark.qss");
    file.open(QFile::ReadOnly | QFile::Text);
    QTextStream stream(&file);
    a.setStyleSheet(stream.readAll());

    MainWindow w;
    vHelix *vh = new vHelix;
    QThread t1;
    vh->moveToThread(&t1);
    //w.moveToThread(&t1);
    
    // connect signals and slots between the main window and most of the program logic
    /*
    QObject::connect(&w, &MainWindow::sendMesh_,
                     &vh, &vHelix::readMesh_);
    QObject::connect(&vh, &vHelix::sendToConsole_,
                     &w, &MainWindow::vHelixToWindow_);
    QObject::connect(&w, &MainWindow::action_,
                     &vh, &vHelix::action_);
    */
    qRegisterMetaType<QVector<QVariant> >("QVector<QVariant>");
    QObject::connect(vh, &vHelix::sendToConsole_,
                     &w, &MainWindow::vHelixToWindow_);
    std::cout << "Connected console\n";
    QObject::connect(&w, &MainWindow::action_,
                     vh, &vHelix::action_);
    std::cout << "Connected action\n";
    QObject::connect(&w, &MainWindow::sendMesh_,
                     vh, &vHelix::readMesh_);
    std::cout << "Connected mesh\n";

    //
    t1.start();
    //t2.start();
    w.show();
    int ret=  a.exec();
    t1.exit();
    delete vh;
    //t1.quit();
    return ret;
}
