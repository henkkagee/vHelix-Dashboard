/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.9.9
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
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
    QMenu *menuMesh;
    QMenu *menuHelp;
    QMenu *menuoxDNA;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->resize(1500, 900);
        QSizePolicy sizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(MainWindow->sizePolicy().hasHeightForWidth());
        MainWindow->setSizePolicy(sizePolicy);
        actionOpen = new QAction(MainWindow);
        actionOpen->setObjectName(QStringLiteral("actionOpen"));
        actionExit = new QAction(MainWindow);
        actionExit->setObjectName(QStringLiteral("actionExit"));
        actionbscor = new QAction(MainWindow);
        actionbscor->setObjectName(QStringLiteral("actionbscor"));
        actionsptree = new QAction(MainWindow);
        actionsptree->setObjectName(QStringLiteral("actionsptree"));
        actionAutofill_strands = new QAction(MainWindow);
        actionAutofill_strands->setObjectName(QStringLiteral("actionAutofill_strands"));
        actionAdd_sequence = new QAction(MainWindow);
        actionAdd_sequence->setObjectName(QStringLiteral("actionAdd_sequence"));
        actionPhysX = new QAction(MainWindow);
        actionPhysX->setObjectName(QStringLiteral("actionPhysX"));
        actionConvert = new QAction(MainWindow);
        actionConvert->setObjectName(QStringLiteral("actionConvert"));
        actionAtrail = new QAction(MainWindow);
        actionAtrail->setObjectName(QStringLiteral("actionAtrail"));
        actionExport_selection = new QAction(MainWindow);
        actionExport_selection->setObjectName(QStringLiteral("actionExport_selection"));
        actionInstructions = new QAction(MainWindow);
        actionInstructions->setObjectName(QStringLiteral("actionInstructions"));
        actionEstimate_nucleotide_use = new QAction(MainWindow);
        actionEstimate_nucleotide_use->setObjectName(QStringLiteral("actionEstimate_nucleotide_use"));
        actionOpen_selection_in_viewer = new QAction(MainWindow);
        actionOpen_selection_in_viewer->setObjectName(QStringLiteral("actionOpen_selection_in_viewer"));
        actionExport_strand_sequences = new QAction(MainWindow);
        actionExport_strand_sequences->setObjectName(QStringLiteral("actionExport_strand_sequences"));
        actionSave_current_model = new QAction(MainWindow);
        actionSave_current_model->setObjectName(QStringLiteral("actionSave_current_model"));
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QStringLiteral("centralwidget"));
        QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(centralwidget->sizePolicy().hasHeightForWidth());
        centralwidget->setSizePolicy(sizePolicy1);
        gridLayout = new QGridLayout(centralwidget);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalLayout->setSizeConstraint(QLayout::SetNoConstraint);
        verticalLayout_3 = new QVBoxLayout();
        verticalLayout_3->setObjectName(QStringLiteral("verticalLayout_3"));
        verticalLayout_3->setSizeConstraint(QLayout::SetNoConstraint);
        plainTextEdit = new Console(centralwidget);
        plainTextEdit->setObjectName(QStringLiteral("plainTextEdit"));
        QSizePolicy sizePolicy2(QSizePolicy::Minimum, QSizePolicy::Expanding);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(plainTextEdit->sizePolicy().hasHeightForWidth());
        plainTextEdit->setSizePolicy(sizePolicy2);

        verticalLayout_3->addWidget(plainTextEdit);

        treeView = new DirView(centralwidget);
        treeView->setObjectName(QStringLiteral("treeView"));
        sizePolicy1.setHeightForWidth(treeView->sizePolicy().hasHeightForWidth());
        treeView->setSizePolicy(sizePolicy1);

        verticalLayout_3->addWidget(treeView);


        horizontalLayout->addLayout(verticalLayout_3);

        widget = new GraphicsView(centralwidget);
        widget->setObjectName(QStringLiteral("widget"));
        sizePolicy1.setHeightForWidth(widget->sizePolicy().hasHeightForWidth());
        widget->setSizePolicy(sizePolicy1);

        horizontalLayout->addWidget(widget);

        horizontalLayout->setStretch(0, 1);
        horizontalLayout->setStretch(1, 2);

        gridLayout->addLayout(horizontalLayout, 0, 0, 1, 1);

        label = new QLabel(centralwidget);
        label->setObjectName(QStringLiteral("label"));

        gridLayout->addWidget(label, 1, 0, 1, 1);

        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName(QStringLiteral("menubar"));
        menubar->setGeometry(QRect(0, 0, 1500, 21));
        menuvHelix = new QMenu(menubar);
        menuvHelix->setObjectName(QStringLiteral("menuvHelix"));
        menuRoute = new QMenu(menubar);
        menuRoute->setObjectName(QStringLiteral("menuRoute"));
        menuRelaxation = new QMenu(menubar);
        menuRelaxation->setObjectName(QStringLiteral("menuRelaxation"));
        menuEdit = new QMenu(menubar);
        menuEdit->setObjectName(QStringLiteral("menuEdit"));
        menuExport = new QMenu(menubar);
        menuExport->setObjectName(QStringLiteral("menuExport"));
        menuMesh = new QMenu(menubar);
        menuMesh->setObjectName(QStringLiteral("menuMesh"));
        menuHelp = new QMenu(menubar);
        menuHelp->setObjectName(QStringLiteral("menuHelp"));
        menuoxDNA = new QMenu(menubar);
        menuoxDNA->setObjectName(QStringLiteral("menuoxDNA"));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName(QStringLiteral("statusbar"));
        MainWindow->setStatusBar(statusbar);

        menubar->addAction(menuvHelix->menuAction());
        menubar->addAction(menuMesh->menuAction());
        menubar->addAction(menuRoute->menuAction());
        menubar->addAction(menuRelaxation->menuAction());
        menubar->addAction(menuEdit->menuAction());
        menubar->addAction(menuExport->menuAction());
        menubar->addAction(menuoxDNA->menuAction());
        menubar->addAction(menuHelp->menuAction());
        menuvHelix->addAction(actionOpen);
        menuvHelix->addAction(actionExit);
        menuvHelix->addAction(actionSave_current_model);
        menuRoute->addAction(actionAtrail);
        menuRoute->addAction(actionsptree);
        menuRelaxation->addAction(actionPhysX);
        menuEdit->addAction(actionAutofill_strands);
        menuEdit->addAction(actionAdd_sequence);
        menuExport->addAction(actionExport_selection);
        menuExport->addAction(actionExport_strand_sequences);
        menuMesh->addAction(actionConvert);
        menuMesh->addAction(actionEstimate_nucleotide_use);
        menuHelp->addAction(actionInstructions);
        menuoxDNA->addAction(actionOpen_selection_in_viewer);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", Q_NULLPTR));
        actionOpen->setText(QApplication::translate("MainWindow", "Import", Q_NULLPTR));
        actionExit->setText(QApplication::translate("MainWindow", "Exit", Q_NULLPTR));
        actionbscor->setText(QApplication::translate("MainWindow", "Bscor", Q_NULLPTR));
        actionsptree->setText(QApplication::translate("MainWindow", "Sptree", Q_NULLPTR));
        actionAutofill_strands->setText(QApplication::translate("MainWindow", "Autofill strands", Q_NULLPTR));
        actionAdd_sequence->setText(QApplication::translate("MainWindow", "Add sequence", Q_NULLPTR));
        actionPhysX->setText(QApplication::translate("MainWindow", "PhysX", Q_NULLPTR));
        actionConvert->setText(QApplication::translate("MainWindow", "Convert...", Q_NULLPTR));
        actionAtrail->setText(QApplication::translate("MainWindow", "Atrail", Q_NULLPTR));
        actionExport_selection->setText(QApplication::translate("MainWindow", "Export selection to oxDNA", Q_NULLPTR));
        actionInstructions->setText(QApplication::translate("MainWindow", "Instructions", Q_NULLPTR));
        actionEstimate_nucleotide_use->setText(QApplication::translate("MainWindow", "Estimate nucleotide use", Q_NULLPTR));
        actionOpen_selection_in_viewer->setText(QApplication::translate("MainWindow", "Open selection in viewer...", Q_NULLPTR));
        actionExport_strand_sequences->setText(QApplication::translate("MainWindow", "Export strand sequences", Q_NULLPTR));
        actionSave_current_model->setText(QApplication::translate("MainWindow", "Save current model", Q_NULLPTR));
        label->setText(QString());
        menuvHelix->setTitle(QApplication::translate("MainWindow", "File", Q_NULLPTR));
        menuRoute->setTitle(QApplication::translate("MainWindow", "Route", Q_NULLPTR));
        menuRelaxation->setTitle(QApplication::translate("MainWindow", "Relaxation", Q_NULLPTR));
        menuEdit->setTitle(QApplication::translate("MainWindow", "Edit", Q_NULLPTR));
        menuExport->setTitle(QApplication::translate("MainWindow", "Export", Q_NULLPTR));
        menuMesh->setTitle(QApplication::translate("MainWindow", "Mesh", Q_NULLPTR));
        menuHelp->setTitle(QApplication::translate("MainWindow", "Help", Q_NULLPTR));
        menuoxDNA->setTitle(QApplication::translate("MainWindow", "oxDNA", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
