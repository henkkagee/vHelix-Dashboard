/*
 *  Copyright (c) 2020 Henrik Granö
 *  See the license in the root directory for the full notice
 *  This file contains the mainwindow class responsible for the user interface, dialogs and communication to different parts of the program. Most logic functions are implemented
 *  in vHelix.cpp but sequence setting and export implementations are placed here. See main.cpp for the communication between this class and vhelix.cpp.
 *
 *  This class also communicates with the file parser and the custom ui elements for 3D graphics, console and directory tree window.
*/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <string>
#include <sstream>
#include <iostream>
#include <random>
#include <cmath>
#include <numeric>
#include <chrono>
#include <ctime>
#include <fstream>
#include <iomanip>

#include <QMainWindow>
#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QFileDialog>
#include <QString>
#include <QInputDialog>
#include <QVector>
#include <QVariant>
#include <QString>
#include <QDir>

#include <console.h>
#include <dirview.h>
#include "graphicsview.h"
#include "modelhandler.h"
#include "model.h"

// the .ui designer forms are used (and automatically generated in Qtcreator)
// as these ui_.h headers
#include "./ui_dialog.h"
#include "./ui_rpolydialog.h"
#include "./ui_sequencedialog.h"
#include "./ui_instructions.h"
#include "./ui_settingsdialog.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class RelaxDialog;

// Main UI class
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui_;
    Console *console_;
    DirView *dirview_;
    RelaxDialog *dialog_;
    GraphicsView *graphicsview_;
    Controller::Handler *handler_;
    std::vector<std::string> selection_;

    void printToConsole_(std::string str);

// Qt slots and signals shouldn't pass arguments as references
public slots:
    void vHelixToWindow_(std::string msg);

    // UI slots
    void on_actionExit_triggered();
    void on_actionOpen_triggered();
    void on_actionAtrail_triggered();
    void on_actionSpanning_tree_triggered();
    void on_actionScaffold_free_triggered();
    void on_actionPhysX_triggered();

signals:
    void sendMesh_(std::string str);
    // action_ sends UI input to the main program logic in class vHelix.
    // argument str determines the command name to be interpreted by action_
    // member function in the vHelix-class, args contains the arguments
    // in a QList<QVariant> which can store several different types in a single
    // list.
    void action_(std::string str, QVector<QVariant> args);

private slots:
    void on_actionExport_selection_triggered();
    void on_actionOpen_selection_in_viewer_triggered();
    void on_treeView_clicked(const QModelIndex &index);
    void on_treeView_doubleClicked(const QModelIndex &index);
    void on_actionAdd_sequence_triggered();
    void on_actionExport_strand_sequences_triggered();
    void on_actionSave_current_model_triggered();
    void on_actionAutofill_strands_triggered();
    void on_actionInstructions_triggered();
    void on_actionLicense_triggered();
    void on_actionSettings_triggered();
};



#endif // MAINWINDOW_H

class RelaxDialog : public QDialog
{
    Q_OBJECT
public:
    explicit RelaxDialog(MainWindow *parent);
    void getOpts(double &scaling, int &iterations);
    /*void getOpts(double &scaling, int &iterations, bool &discretize_lengths, double &density,
                double &spring_stiffness, double &fixed_spring_stiffness,
                double &spring_damping, bool &attach_fixed,
                double &static_friction, double &dynamic_friction,
                double &restitution, double &rigid_body_sleep_threshold,
                std::string &visual_debugger);*/
    void setBaseEstimate(const std::string &estimate);

private slots:
    void on_pushButton_clicked();

private:
    Ui::Dialog ui;
    MainWindow *parent_;
};

class RpolyDialog : public QDialog
{
    Q_OBJECT
public:
    explicit RpolyDialog(MainWindow *parent);
    void getOpts(int &min, int &max);

private:
    Ui::RpolyDialog ui_;
    MainWindow *parent_;
};

class SequenceDialog : public QDialog
{
    Q_OBJECT
public:
    explicit SequenceDialog(MainWindow *parent, const std::vector<Model::Strand>& strands);
    std::string getSequence();
    std::string getTargetStrand();
private:
    Ui::SequenceDialog ui_;
    MainWindow *parent_;
};

class SettingsDialog : public QDialog {
    Q_OBJECT
    public:
        explicit SettingsDialog(MainWindow *parent);
        void getSettings(bool &write_intermediate_files,
                         bool &discretize_lengths, double &density, double &spring_stiffness,
                         double &fixed_spring_stiffness, double &spring_damping, bool &attach_fixed,
                         double &static_friction,double &dynamic_friction,double &restitution,
                         double &rigid_body_sleep_threshold, bool &visual_debugger);
    private slots:
            void on_SetDefaults_clicked();

private:
        Ui::SettingsDialog ui_;
        MainWindow *parent_;
};

class DocWindow : public QWidget
{
    Q_OBJECT

public:
    explicit DocWindow(MainWindow *parent, const int &type);

private:
    Ui::DocWindow ui_;
    MainWindow *parent_;
};
