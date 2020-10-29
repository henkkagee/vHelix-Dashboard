/********************************************************************************
** Form generated from reading UI file 'sequencedialog.ui'
**
** Created by: Qt User Interface Compiler version 5.12.8
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SEQUENCEDIALOG_H
#define UI_SEQUENCEDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_SequenceDialog
{
public:
    QDialogButtonBox *buttonBox;
    QPlainTextEdit *plainTextEdit;
    QFrame *frame;
    QWidget *gridLayoutWidget;
    QGridLayout *gridLayout;
    QRadioButton *radioButton_4;
    QRadioButton *radioButton_2;
    QRadioButton *radioButton_5;
    QRadioButton *radioButton;
    QRadioButton *radioButton_6;
    QRadioButton *radioButton_3;
    QRadioButton *radioButton_7;
    QLabel *label;

    void setupUi(QDialog *Dialog)
    {
        if (Dialog->objectName().isEmpty())
            Dialog->setObjectName(QString::fromUtf8("Dialog"));
        Dialog->resize(400, 300);
        buttonBox = new QDialogButtonBox(Dialog);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setGeometry(QRect(30, 240, 341, 32));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        plainTextEdit = new QPlainTextEdit(Dialog);
        plainTextEdit->setObjectName(QString::fromUtf8("plainTextEdit"));
        plainTextEdit->setGeometry(QRect(20, 170, 361, 61));
        frame = new QFrame(Dialog);
        frame->setObjectName(QString::fromUtf8("frame"));
        frame->setGeometry(QRect(20, 10, 361, 131));
        frame->setFrameShape(QFrame::StyledPanel);
        frame->setFrameShadow(QFrame::Raised);
        gridLayoutWidget = new QWidget(frame);
        gridLayoutWidget->setObjectName(QString::fromUtf8("gridLayoutWidget"));
        gridLayoutWidget->setGeometry(QRect(9, -1, 341, 131));
        gridLayout = new QGridLayout(gridLayoutWidget);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        gridLayout->setContentsMargins(0, 0, 0, 0);
        radioButton_4 = new QRadioButton(gridLayoutWidget);
        radioButton_4->setObjectName(QString::fromUtf8("radioButton_4"));

        gridLayout->addWidget(radioButton_4, 0, 1, 1, 1);

        radioButton_2 = new QRadioButton(gridLayoutWidget);
        radioButton_2->setObjectName(QString::fromUtf8("radioButton_2"));

        gridLayout->addWidget(radioButton_2, 1, 0, 1, 1);

        radioButton_5 = new QRadioButton(gridLayoutWidget);
        radioButton_5->setObjectName(QString::fromUtf8("radioButton_5"));

        gridLayout->addWidget(radioButton_5, 2, 0, 1, 1);

        radioButton = new QRadioButton(gridLayoutWidget);
        radioButton->setObjectName(QString::fromUtf8("radioButton"));

        gridLayout->addWidget(radioButton, 0, 0, 1, 1);

        radioButton_6 = new QRadioButton(gridLayoutWidget);
        radioButton_6->setObjectName(QString::fromUtf8("radioButton_6"));

        gridLayout->addWidget(radioButton_6, 2, 1, 1, 1);

        radioButton_3 = new QRadioButton(gridLayoutWidget);
        radioButton_3->setObjectName(QString::fromUtf8("radioButton_3"));

        gridLayout->addWidget(radioButton_3, 1, 1, 1, 1);

        radioButton_7 = new QRadioButton(gridLayoutWidget);
        radioButton_7->setObjectName(QString::fromUtf8("radioButton_7"));

        gridLayout->addWidget(radioButton_7, 3, 0, 1, 1);

        label = new QLabel(Dialog);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(20, 150, 141, 16));

        retranslateUi(Dialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), Dialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), Dialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(Dialog);
    } // setupUi

    void retranslateUi(QDialog *Dialog)
    {
        Dialog->setWindowTitle(QApplication::translate("Dialog", "Set scaffold base sequence", nullptr));
        radioButton_4->setText(QApplication::translate("Dialog", "p7308 (length: 7308)", nullptr));
        radioButton_2->setText(QApplication::translate("Dialog", "p7560 (length: 7560)", nullptr));
        radioButton_5->setText(QApplication::translate("Dialog", "p8064 (length: 8064)", nullptr));
        radioButton->setText(QApplication::translate("Dialog", "M13mp18 (length: 7249)", nullptr));
        radioButton_6->setText(QApplication::translate("Dialog", "p8100 (length: 8100)", nullptr));
        radioButton_3->setText(QApplication::translate("Dialog", "p7704 (length: 7704)", nullptr));
        radioButton_7->setText(QApplication::translate("Dialog", "p8634 (length: 8634)", nullptr));
        label->setText(QApplication::translate("Dialog", "Custom scaffold sequence:", nullptr));
    } // retranslateUi

};

namespace Ui {
    class SequenceDialog: public Ui_SequenceDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SEQUENCEDIALOG_H
