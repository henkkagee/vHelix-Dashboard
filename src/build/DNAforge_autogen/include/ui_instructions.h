/********************************************************************************
** Form generated from reading UI file 'instructions.ui'
**
** Created by: Qt User Interface Compiler version 5.9.9
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_INSTRUCTIONS_H
#define UI_INSTRUCTIONS_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_DocWindow
{
public:
    QScrollArea *scrollArea;
    QWidget *scrollAreaWidgetContents;
    QPlainTextEdit *plainTextEdit;

    void setupUi(QWidget *DocWindow)
    {
        if (DocWindow->objectName().isEmpty())
            DocWindow->setObjectName(QStringLiteral("DocWindow"));
        DocWindow->resize(480, 640);
        scrollArea = new QScrollArea(DocWindow);
        scrollArea->setObjectName(QStringLiteral("scrollArea"));
        scrollArea->setGeometry(QRect(-10, 0, 501, 651));
        scrollArea->setWidgetResizable(true);
        scrollAreaWidgetContents = new QWidget();
        scrollAreaWidgetContents->setObjectName(QStringLiteral("scrollAreaWidgetContents"));
        scrollAreaWidgetContents->setGeometry(QRect(0, 0, 499, 649));
        plainTextEdit = new QPlainTextEdit(scrollAreaWidgetContents);
        plainTextEdit->setObjectName(QStringLiteral("plainTextEdit"));
        plainTextEdit->setGeometry(QRect(13, 10, 471, 631));
        scrollArea->setWidget(scrollAreaWidgetContents);

        retranslateUi(DocWindow);

        QMetaObject::connectSlotsByName(DocWindow);
    } // setupUi

    void retranslateUi(QWidget *DocWindow)
    {
        DocWindow->setWindowTitle(QApplication::translate("DocWindow", "Form", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class DocWindow: public Ui_DocWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_INSTRUCTIONS_H
