/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.12.8
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include <dirview.h>
#include "console.h"
#include "graphicsview.h"

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *actionOpen;
    QAction *actionExit;
    QAction *actionbscor;
    QAction *actionsptree;
    QAction *actionAutofill_strands;
    QAction *actionAdd_sequence;
    QAction *actionPhysX;
    QAction *actionConvert;
    QAction *actionAtrail;
    QAction *actionExport_selection;
    QAction *actionInstructions;
    QAction *actionEstimate_nucleotide_use;
    QAction *actionOpen_selection_in_viewer;
    QAction *actionExport_strand_sequences;
    QAction *actionSave_current_model;
    QWidget *centralwidget;
    QGridLayout *gridLayout;
    QHBoxLayout *horizontalLayout;
    QVBoxLayout *verticalLayout_3;
    Console *plainTextEdit;
    DirView *treeView;
    GraphicsView *widget;
    QLabel *label;
    QMenuBar *menubar;
    QMenu *menuvHelix;
    QMenu *menuRoute;
    QMenu *menuRelaxation;
    QMenu *menuEdit;
    QMenu *menuExport;
    QMenu *menuHelp;
    QMenu *menuoxDNA;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(1500, 900);
        QSizePolicy sizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(MainWindow->sizePolicy().hasHeightForWidth());
        MainWindow->setSizePolicy(sizePolicy);
        QIcon icon;
        icon.addFile(QString::fromUtf8("resources/icon.png"), QSize(), QIcon::Normal, QIcon::Off);
        MainWindow->setWindowIcon(icon);
        actionOpen = new QAction(MainWindow);
        actionOpen->setObjectName(QString::fromUtf8("actionOpen"));
        actionExit = new QAction(MainWindow);
        actionExit->setObjectName(QString::fromUtf8("actionExit"));
        actionbscor = new QAction(MainWindow);
        actionbscor->setObjectName(QString::fromUtf8("actionbscor"));
        actionsptree = new QAction(MainWindow);
        actionsptree->setObjectName(QString::fromUtf8("actionsptree"));
        actionAutofill_strands = new QAction(MainWindow);
        actionAutofill_strands->setObjectName(QString::fromUtf8("actionAutofill_strands"));
        actionAdd_sequence = new QAction(MainWindow);
        actionAdd_sequence->setObjectName(QString::fromUtf8("actionAdd_sequence"));
        actionPhysX = new QAction(MainWindow);
        actionPhysX->setObjectName(QString::fromUtf8("actionPhysX"));
        actionConvert = new QAction(MainWindow);
        actionConvert->setObjectName(QString::fromUtf8("actionConvert"));
        actionAtrail = new QAction(MainWindow);
        actionAtrail->setObjectName(QString::fromUtf8("actionAtrail"));
        actionExport_selection = new QAction(MainWindow);
        actionExport_selection->setObjectName(QString::fromUtf8("actionExport_selection"));
        actionInstructions = new QAction(MainWindow);
        actionInstructions->setObjectName(QString::fromUtf8("actionInstructions"));
        actionEstimate_nucleotide_use = new QAction(MainWindow);
        actionEstimate_nucleotide_use->setObjectName(QString::fromUtf8("actionEstimate_nucleotide_use"));
        actionOpen_selection_in_viewer = new QAction(MainWindow);
        actionOpen_selection_in_viewer->setObjectName(QString::fromUtf8("actionOpen_selection_in_viewer"));
        actionExport_strand_sequences = new QAction(MainWindow);
        actionExport_strand_sequences->setObjectName(QString::fromUtf8("actionExport_strand_sequences"));
        actionSave_current_model = new QAction(MainWindow);
        actionSave_current_model->setObjectName(QString::fromUtf8("actionSave_current_model"));
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(centralwidget->sizePolicy().hasHeightForWidth());
        centralwidget->setSizePolicy(sizePolicy1);
        gridLayout = new QGridLayout(centralwidget);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalLayout->setSizeConstraint(QLayout::SetNoConstraint);
        verticalLayout_3 = new QVBoxLayout();
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        verticalLayout_3->setSizeConstraint(QLayout::SetNoConstraint);
        plainTextEdit = new Console(centralwidget);
        plainTextEdit->setObjectName(QString::fromUtf8("plainTextEdit"));
        QSizePolicy sizePolicy2(QSizePolicy::Minimum, QSizePolicy::Expanding);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(plainTextEdit->sizePolicy().hasHeightForWidth());
        plainTextEdit->setSizePolicy(sizePolicy2);

        verticalLayout_3->addWidget(plainTextEdit);

        treeView = new DirView(centralwidget);
        treeView->setObjectName(QString::fromUtf8("treeView"));
        sizePolicy1.setHeightForWidth(treeView->sizePolicy().hasHeightForWidth());
        treeView->setSizePolicy(sizePolicy1);

        verticalLayout_3->addWidget(treeView);


        horizontalLayout->addLayout(verticalLayout_3);

        widget = new GraphicsView(centralwidget);
        widget->setObjectName(QString::fromUtf8("widget"));
        sizePolicy1.setHeightForWidth(widget->sizePolicy().hasHeightForWidth());
        widget->setSizePolicy(sizePolicy1);

        horizontalLayout->addWidget(widget);

        horizontalLayout->setStretch(0, 1);
        horizontalLayout->setStretch(1, 2);

        gridLayout->addLayout(horizontalLayout, 0, 0, 1, 1);

        label = new QLabel(centralwidget);
        label->setObjectName(QString::fromUtf8("label"));

        gridLayout->addWidget(label, 1, 0, 1, 1);

        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 1500, 21));
        menuvHelix = new QMenu(menubar);
        menuvHelix->setObjectName(QString::fromUtf8("menuvHelix"));
        menuRoute = new QMenu(menubar);
        menuRoute->setObjectName(QString::fromUtf8("menuRoute"));
        menuRelaxation = new QMenu(menubar);
        menuRelaxation->setObjectName(QString::fromUtf8("menuRelaxation"));
        menuEdit = new QMenu(menubar);
        menuEdit->setObjectName(QString::fromUtf8("menuEdit"));
        menuExport = new QMenu(menubar);
        menuExport->setObjectName(QString::fromUtf8("menuExport"));
        menuHelp = new QMenu(menubar);
        menuHelp->setObjectName(QString::fromUtf8("menuHelp"));
        menuoxDNA = new QMenu(menubar);
        menuoxDNA->setObjectName(QString::fromUtf8("menuoxDNA"));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        MainWindow->setStatusBar(statusbar);

        menubar->addAction(menuvHelix->menuAction());
        menubar->addAction(menuRoute->menuAction());
        menubar->addAction(menuRelaxation->menuAction());
        menubar->addAction(menuEdit->menuAction());
        menubar->addAction(menuExport->menuAction());
        menubar->addAction(menuoxDNA->menuAction());
        menubar->addAction(menuHelp->menuAction());
        menuvHelix->addAction(actionOpen);
        menuvHelix->addAction(actionSave_current_model);
        menuvHelix->addAction(actionExit);
        menuRoute->addAction(actionAtrail);
        menuRelaxation->addAction(actionPhysX);
        menuEdit->addAction(actionAutofill_strands);
        menuEdit->addAction(actionAdd_sequence);
        menuExport->addAction(actionExport_selection);
        menuExport->addAction(actionExport_strand_sequences);
        menuHelp->addAction(actionInstructions);
        menuoxDNA->addAction(actionOpen_selection_in_viewer);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", nullptr));
        actionOpen->setText(QApplication::translate("MainWindow", "Import", nullptr));
        actionExit->setText(QApplication::translate("MainWindow", "Exit", nullptr));
        actionbscor->setText(QApplication::translate("MainWindow", "Bscor", nullptr));
        actionsptree->setText(QApplication::translate("MainWindow", "Sptree", nullptr));
        actionAutofill_strands->setText(QApplication::translate("MainWindow", "Autofill staple strand gaps", nullptr));
        actionAdd_sequence->setText(QApplication::translate("MainWindow", "Add sequence", nullptr));
        actionPhysX->setText(QApplication::translate("MainWindow", "PhysX", nullptr));
        actionConvert->setText(QApplication::translate("MainWindow", "Convert...", nullptr));
        actionAtrail->setText(QApplication::translate("MainWindow", "Atrail", nullptr));
        actionExport_selection->setText(QApplication::translate("MainWindow", "Export selection to oxDNA", nullptr));
        actionInstructions->setText(QApplication::translate("MainWindow", "Instructions", nullptr));
        actionEstimate_nucleotide_use->setText(QApplication::translate("MainWindow", "Estimate nucleotide use", nullptr));
        actionOpen_selection_in_viewer->setText(QApplication::translate("MainWindow", "Open OxDNA viewer", nullptr));
        actionExport_strand_sequences->setText(QApplication::translate("MainWindow", "Export strand sequences", nullptr));
        actionSave_current_model->setText(QApplication::translate("MainWindow", "Save current model", nullptr));
        label->setText(QString());
        menuvHelix->setTitle(QApplication::translate("MainWindow", "File", nullptr));
        menuRoute->setTitle(QApplication::translate("MainWindow", "Route", nullptr));
        menuRelaxation->setTitle(QApplication::translate("MainWindow", "Relaxation", nullptr));
        menuEdit->setTitle(QApplication::translate("MainWindow", "Edit", nullptr));
        menuExport->setTitle(QApplication::translate("MainWindow", "Export", nullptr));
        menuHelp->setTitle(QApplication::translate("MainWindow", "Help", nullptr));
        menuoxDNA->setTitle(QApplication::translate("MainWindow", "oxDNA", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
