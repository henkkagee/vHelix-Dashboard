/********************************************************************************
** Form generated from reading UI file 'atraildialog.ui'
**
** Created by: Qt User Interface Compiler version 5.9.9
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ATRAILDIALOG_H
#define UI_ATRAILDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_AtrailDialog
{
public:
    QDialogButtonBox *buttonBox;
    QLabel *label_2;
    QLabel *label_3;
    QLabel *label_4;
    QSpinBox *spinBox;
    QSpinBox *spinBox_2;
    QLabel *label;
    QDoubleSpinBox *doubleSpinBox;
    QPushButton *pushButton;
    QWidget *verticalLayoutWidget;
    QVBoxLayout *verticalLayout;
    QCheckBox *autofillStrandGaps;
    QPlainTextEdit *plainTextEdit;
    QLabel *label_6;
    QLabel *label_5;
    QWidget *gridLayoutWidget;
    QGridLayout *gridLayout;
    QRadioButton *radioButton_4;
    QRadioButton *radioButton_6;
    QRadioButton *radioButton_8;
    QRadioButton *radioButton_2;
    QRadioButton *radioButton_5;
    QRadioButton *radioButton;
    QRadioButton *radioButton_3;
    QRadioButton *radioButton_7;
    QLabel *label_7;

    void setupUi(QDialog *AtrailDialog)
    {
        if (AtrailDialog->objectName().isEmpty())
            AtrailDialog->setObjectName(QStringLiteral("AtrailDialog"));
        AtrailDialog->resize(441, 577);
        buttonBox = new QDialogButtonBox(AtrailDialog);
        buttonBox->setObjectName(QStringLiteral("buttonBox"));
        buttonBox->setGeometry(QRect(20, 540, 341, 32));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        label_2 = new QLabel(AtrailDialog);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setGeometry(QRect(10, 190, 131, 31));
        label_3 = new QLabel(AtrailDialog);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setGeometry(QRect(10, 230, 131, 31));
        label_4 = new QLabel(AtrailDialog);
        label_4->setObjectName(QStringLiteral("label_4"));
        label_4->setGeometry(QRect(20, 140, 311, 51));
        label_4->setLayoutDirection(Qt::LeftToRight);
        label_4->setWordWrap(true);
        spinBox = new QSpinBox(AtrailDialog);
        spinBox->setObjectName(QStringLiteral("spinBox"));
        spinBox->setGeometry(QRect(150, 190, 192, 31));
        spinBox->setMinimum(10);
        spinBox->setValue(20);
        spinBox_2 = new QSpinBox(AtrailDialog);
        spinBox_2->setObjectName(QStringLiteral("spinBox_2"));
        spinBox_2->setGeometry(QRect(150, 230, 191, 31));
        spinBox_2->setMinimum(20);
        spinBox_2->setValue(40);
        label = new QLabel(AtrailDialog);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(20, 20, 51, 21));
        doubleSpinBox = new QDoubleSpinBox(AtrailDialog);
        doubleSpinBox->setObjectName(QStringLiteral("doubleSpinBox"));
        doubleSpinBox->setGeometry(QRect(100, 20, 191, 23));
        doubleSpinBox->setValue(1);
        pushButton = new QPushButton(AtrailDialog);
        pushButton->setObjectName(QStringLiteral("pushButton"));
        pushButton->setGeometry(QRect(20, 50, 291, 22));
        verticalLayoutWidget = new QWidget(AtrailDialog);
        verticalLayoutWidget->setObjectName(QStringLiteral("verticalLayoutWidget"));
        verticalLayoutWidget->setGeometry(QRect(20, 80, 231, 41));
        verticalLayout = new QVBoxLayout(verticalLayoutWidget);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        autofillStrandGaps = new QCheckBox(verticalLayoutWidget);
        autofillStrandGaps->setObjectName(QStringLiteral("autofillStrandGaps"));

        verticalLayout->addWidget(autofillStrandGaps);

        plainTextEdit = new QPlainTextEdit(AtrailDialog);
        plainTextEdit->setObjectName(QStringLiteral("plainTextEdit"));
        plainTextEdit->setGeometry(QRect(20, 470, 361, 61));
        label_6 = new QLabel(AtrailDialog);
        label_6->setObjectName(QStringLiteral("label_6"));
        label_6->setGeometry(QRect(20, 440, 141, 16));
        label_5 = new QLabel(AtrailDialog);
        label_5->setObjectName(QStringLiteral("label_5"));
        label_5->setGeometry(QRect(10, 270, 211, 20));
        gridLayoutWidget = new QWidget(AtrailDialog);
        gridLayoutWidget->setObjectName(QStringLiteral("gridLayoutWidget"));
        gridLayoutWidget->setGeometry(QRect(10, 300, 341, 131));
        gridLayout = new QGridLayout(gridLayoutWidget);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        gridLayout->setContentsMargins(0, 0, 0, 0);
        radioButton_4 = new QRadioButton(gridLayoutWidget);
        radioButton_4->setObjectName(QStringLiteral("radioButton_4"));

        gridLayout->addWidget(radioButton_4, 0, 1, 1, 1);

        radioButton_6 = new QRadioButton(gridLayoutWidget);
        radioButton_6->setObjectName(QStringLiteral("radioButton_6"));

        gridLayout->addWidget(radioButton_6, 2, 1, 1, 1);

        radioButton_8 = new QRadioButton(gridLayoutWidget);
        radioButton_8->setObjectName(QStringLiteral("radioButton_8"));

        gridLayout->addWidget(radioButton_8, 3, 1, 1, 1);

        radioButton_2 = new QRadioButton(gridLayoutWidget);
        radioButton_2->setObjectName(QStringLiteral("radioButton_2"));

        gridLayout->addWidget(radioButton_2, 1, 0, 1, 1);

        radioButton_5 = new QRadioButton(gridLayoutWidget);
        radioButton_5->setObjectName(QStringLiteral("radioButton_5"));

        gridLayout->addWidget(radioButton_5, 2, 0, 1, 1);

        radioButton = new QRadioButton(gridLayoutWidget);
        radioButton->setObjectName(QStringLiteral("radioButton"));
        radioButton->setChecked(true);

        gridLayout->addWidget(radioButton, 0, 0, 1, 1);

        radioButton_3 = new QRadioButton(gridLayoutWidget);
        radioButton_3->setObjectName(QStringLiteral("radioButton_3"));

        gridLayout->addWidget(radioButton_3, 1, 1, 1, 1);

        radioButton_7 = new QRadioButton(gridLayoutWidget);
        radioButton_7->setObjectName(QStringLiteral("radioButton_7"));

        gridLayout->addWidget(radioButton_7, 3, 0, 1, 1);

        label_7 = new QLabel(AtrailDialog);
        label_7->setObjectName(QStringLiteral("label_7"));
        label_7->setGeometry(QRect(330, 50, 61, 21));
        label_2->raise();
        label_3->raise();
        label_4->raise();
        spinBox->raise();
        spinBox_2->raise();
        label->raise();
        doubleSpinBox->raise();
        pushButton->raise();
        verticalLayoutWidget->raise();
        plainTextEdit->raise();
        label_6->raise();
        buttonBox->raise();
        label_5->raise();
        gridLayoutWidget->raise();
        label_7->raise();

        retranslateUi(AtrailDialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), AtrailDialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), AtrailDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(AtrailDialog);
    } // setupUi

    void retranslateUi(QDialog *AtrailDialog)
    {
        AtrailDialog->setWindowTitle(QApplication::translate("AtrailDialog", "Dialog", Q_NULLPTR));
        label_2->setText(QApplication::translate("AtrailDialog", "Minimum length", Q_NULLPTR));
        label_3->setText(QApplication::translate("AtrailDialog", "Maximum length", Q_NULLPTR));
        label_4->setText(QApplication::translate("AtrailDialog", "Desired staple strand length in bases\n"
" (use an interval of at least 10-20 bases):", Q_NULLPTR));
        label->setText(QApplication::translate("AtrailDialog", "Scaling:", Q_NULLPTR));
        pushButton->setText(QApplication::translate("AtrailDialog", "Estimate scaffold strand length:", Q_NULLPTR));
        autofillStrandGaps->setText(QApplication::translate("AtrailDialog", "Autofill strand gaps", Q_NULLPTR));
        label_6->setText(QApplication::translate("AtrailDialog", "Custom scaffold sequence:", Q_NULLPTR));
        label_5->setText(QApplication::translate("AtrailDialog", "Select scaffold sequence:", Q_NULLPTR));
        radioButton_4->setText(QApplication::translate("AtrailDialog", "p7308 (length: 7308)", Q_NULLPTR));
        radioButton_6->setText(QApplication::translate("AtrailDialog", "p8100 (length: 8100)", Q_NULLPTR));
        radioButton_8->setText(QApplication::translate("AtrailDialog", "None", Q_NULLPTR));
        radioButton_2->setText(QApplication::translate("AtrailDialog", "p7560 (length: 7560)", Q_NULLPTR));
        radioButton_5->setText(QApplication::translate("AtrailDialog", "p8064 (length: 8064)", Q_NULLPTR));
        radioButton->setText(QApplication::translate("AtrailDialog", "M13mp18 (length: 7249)", Q_NULLPTR));
        radioButton_3->setText(QApplication::translate("AtrailDialog", "p7704 (length: 7704)", Q_NULLPTR));
        radioButton_7->setText(QApplication::translate("AtrailDialog", "p8634 (length: 8634)", Q_NULLPTR));
        label_7->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class AtrailDialog: public Ui_AtrailDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ATRAILDIALOG_H
