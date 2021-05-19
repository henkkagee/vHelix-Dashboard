/********************************************************************************
** Form generated from reading UI file 'sequencedialog.ui'
**
** Created by: Qt User Interface Compiler version 5.9.9
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SEQUENCEDIALOG_H
#define UI_SEQUENCEDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHeaderView>
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
    QComboBox *comboBox;
    QLabel *label_2;
    QLabel *label;
    QWidget *gridLayoutWidget;
    QGridLayout *gridLayout;
    QRadioButton *radioButton_4;
    QRadioButton *radioButton_2;
    QRadioButton *radioButton_5;
    QRadioButton *radioButton;
    QRadioButton *radioButton_6;
    QRadioButton *radioButton_3;
    QRadioButton *radioButton_7;

    void setupUi(QDialog *SequenceDialog)
    {
        if (SequenceDialog->objectName().isEmpty())
            SequenceDialog->setObjectName(QStringLiteral("SequenceDialog"));
        SequenceDialog->resize(400, 376);
        buttonBox = new QDialogButtonBox(SequenceDialog);
        buttonBox->setObjectName(QStringLiteral("buttonBox"));
        buttonBox->setGeometry(QRect(40, 340, 341, 32));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        plainTextEdit = new QPlainTextEdit(SequenceDialog);
        plainTextEdit->setObjectName(QStringLiteral("plainTextEdit"));
        plainTextEdit->setGeometry(QRect(20, 270, 361, 61));
        frame = new QFrame(SequenceDialog);
        frame->setObjectName(QStringLiteral("frame"));
        frame->setGeometry(QRect(20, 10, 361, 131));
        frame->setFrameShape(QFrame::StyledPanel);
        frame->setFrameShadow(QFrame::Raised);
        comboBox = new QComboBox(frame);
        comboBox->setObjectName(QStringLiteral("comboBox"));
        comboBox->setGeometry(QRect(10, 30, 331, 22));
        label_2 = new QLabel(frame);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setGeometry(QRect(10, 10, 101, 16));
        label = new QLabel(SequenceDialog);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(20, 240, 141, 16));
        gridLayoutWidget = new QWidget(SequenceDialog);
        gridLayoutWidget->setObjectName(QStringLiteral("gridLayoutWidget"));
        gridLayoutWidget->setGeometry(QRect(30, 90, 341, 131));
        gridLayout = new QGridLayout(gridLayoutWidget);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        gridLayout->setContentsMargins(0, 0, 0, 0);
        radioButton_4 = new QRadioButton(gridLayoutWidget);
        radioButton_4->setObjectName(QStringLiteral("radioButton_4"));

        gridLayout->addWidget(radioButton_4, 0, 1, 1, 1);

        radioButton_2 = new QRadioButton(gridLayoutWidget);
        radioButton_2->setObjectName(QStringLiteral("radioButton_2"));

        gridLayout->addWidget(radioButton_2, 1, 0, 1, 1);

        radioButton_5 = new QRadioButton(gridLayoutWidget);
        radioButton_5->setObjectName(QStringLiteral("radioButton_5"));

        gridLayout->addWidget(radioButton_5, 2, 0, 1, 1);

        radioButton = new QRadioButton(gridLayoutWidget);
        radioButton->setObjectName(QStringLiteral("radioButton"));

        gridLayout->addWidget(radioButton, 0, 0, 1, 1);

        radioButton_6 = new QRadioButton(gridLayoutWidget);
        radioButton_6->setObjectName(QStringLiteral("radioButton_6"));

        gridLayout->addWidget(radioButton_6, 2, 1, 1, 1);

        radioButton_3 = new QRadioButton(gridLayoutWidget);
        radioButton_3->setObjectName(QStringLiteral("radioButton_3"));

        gridLayout->addWidget(radioButton_3, 1, 1, 1, 1);

        radioButton_7 = new QRadioButton(gridLayoutWidget);
        radioButton_7->setObjectName(QStringLiteral("radioButton_7"));

        gridLayout->addWidget(radioButton_7, 3, 0, 1, 1);


        retranslateUi(SequenceDialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), SequenceDialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), SequenceDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(SequenceDialog);
    } // setupUi

    void retranslateUi(QDialog *SequenceDialog)
    {
        SequenceDialog->setWindowTitle(QApplication::translate("SequenceDialog", "Set scaffold base sequence", Q_NULLPTR));
        label_2->setText(QApplication::translate("SequenceDialog", "Assign to strand:", Q_NULLPTR));
        label->setText(QApplication::translate("SequenceDialog", "Custom scaffold sequence::", Q_NULLPTR));
        radioButton_4->setText(QApplication::translate("SequenceDialog", "p7308 (length: 7308)", Q_NULLPTR));
        radioButton_2->setText(QApplication::translate("SequenceDialog", "p7560 (length: 7560)", Q_NULLPTR));
        radioButton_5->setText(QApplication::translate("SequenceDialog", "p8064 (length: 8064)", Q_NULLPTR));
        radioButton->setText(QApplication::translate("SequenceDialog", "M13mp18 (length: 7249)", Q_NULLPTR));
        radioButton_6->setText(QApplication::translate("SequenceDialog", "p8100 (length: 8100)", Q_NULLPTR));
        radioButton_3->setText(QApplication::translate("SequenceDialog", "p7704 (length: 7704)", Q_NULLPTR));
        radioButton_7->setText(QApplication::translate("SequenceDialog", "p8634 (length: 8634)", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class SequenceDialog: public Ui_SequenceDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SEQUENCEDIALOG_H
