/********************************************************************************
** Form generated from reading UI file 'dialog.ui'
**
** Created by: Qt User Interface Compiler version 5.12.8
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DIALOG_H
#define UI_DIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Dialog
{
public:
    QDialogButtonBox *buttonBox;
    QWidget *gridLayoutWidget;
    QGridLayout *gridLayout;
    QPushButton *pushButton;
    QHBoxLayout *horizontalLayout;
    QVBoxLayout *verticalLayout_2;
    QLabel *label;
    QDoubleSpinBox *doubleSpinBox;
    QVBoxLayout *verticalLayout;
    QLabel *label_13;
    QSpinBox *spinBox;
    QLabel *label_14;
    QWidget *formLayoutWidget;
    QFormLayout *formLayout;
    QLabel *label_4;
    QComboBox *comboBox;
    QLabel *label_2;
    QDoubleSpinBox *doubleSpinBox_2;
    QLabel *label_3;
    QDoubleSpinBox *doubleSpinBox_3;
    QLabel *label_5;
    QDoubleSpinBox *doubleSpinBox_4;
    QLabel *label_6;
    QDoubleSpinBox *doubleSpinBox_5;
    QLabel *label_7;
    QComboBox *comboBox_2;
    QLabel *label_8;
    QDoubleSpinBox *doubleSpinBox_6;
    QLabel *label_9;
    QDoubleSpinBox *doubleSpinBox_7;
    QLabel *label_10;
    QDoubleSpinBox *doubleSpinBox_8;
    QLabel *label_11;
    QDoubleSpinBox *doubleSpinBox_9;
    QLabel *label_12;
    QComboBox *comboBox_3;

    void setupUi(QDialog *Dialog)
    {
        if (Dialog->objectName().isEmpty())
            Dialog->setObjectName(QString::fromUtf8("Dialog"));
        Dialog->resize(519, 443);
        buttonBox = new QDialogButtonBox(Dialog);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setGeometry(QRect(30, 400, 481, 32));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        gridLayoutWidget = new QWidget(Dialog);
        gridLayoutWidget->setObjectName(QString::fromUtf8("gridLayoutWidget"));
        gridLayoutWidget->setGeometry(QRect(10, 0, 501, 74));
        gridLayout = new QGridLayout(gridLayoutWidget);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        gridLayout->setContentsMargins(0, 0, 0, 0);
        pushButton = new QPushButton(gridLayoutWidget);
        pushButton->setObjectName(QString::fromUtf8("pushButton"));

        gridLayout->addWidget(pushButton, 1, 1, 1, 1);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        label = new QLabel(gridLayoutWidget);
        label->setObjectName(QString::fromUtf8("label"));

        verticalLayout_2->addWidget(label);

        doubleSpinBox = new QDoubleSpinBox(gridLayoutWidget);
        doubleSpinBox->setObjectName(QString::fromUtf8("doubleSpinBox"));
        doubleSpinBox->setValue(1.000000000000000);

        verticalLayout_2->addWidget(doubleSpinBox);


        horizontalLayout->addLayout(verticalLayout_2);

        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        label_13 = new QLabel(gridLayoutWidget);
        label_13->setObjectName(QString::fromUtf8("label_13"));

        verticalLayout->addWidget(label_13);

        spinBox = new QSpinBox(gridLayoutWidget);
        spinBox->setObjectName(QString::fromUtf8("spinBox"));
        spinBox->setValue(1);

        verticalLayout->addWidget(spinBox);


        horizontalLayout->addLayout(verticalLayout);


        gridLayout->addLayout(horizontalLayout, 1, 0, 1, 1);

        label_14 = new QLabel(gridLayoutWidget);
        label_14->setObjectName(QString::fromUtf8("label_14"));

        gridLayout->addWidget(label_14, 1, 2, 1, 1);

        formLayoutWidget = new QWidget(Dialog);
        formLayoutWidget->setObjectName(QString::fromUtf8("formLayoutWidget"));
        formLayoutWidget->setGeometry(QRect(10, 80, 501, 311));
        formLayout = new QFormLayout(formLayoutWidget);
        formLayout->setObjectName(QString::fromUtf8("formLayout"));
        formLayout->setContentsMargins(0, 0, 0, 0);
        label_4 = new QLabel(formLayoutWidget);
        label_4->setObjectName(QString::fromUtf8("label_4"));

        formLayout->setWidget(0, QFormLayout::LabelRole, label_4);

        comboBox = new QComboBox(formLayoutWidget);
        comboBox->addItem(QString());
        comboBox->addItem(QString());
        comboBox->setObjectName(QString::fromUtf8("comboBox"));

        formLayout->setWidget(0, QFormLayout::FieldRole, comboBox);

        label_2 = new QLabel(formLayoutWidget);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        formLayout->setWidget(1, QFormLayout::LabelRole, label_2);

        doubleSpinBox_2 = new QDoubleSpinBox(formLayoutWidget);
        doubleSpinBox_2->setObjectName(QString::fromUtf8("doubleSpinBox_2"));
        doubleSpinBox_2->setValue(10.000000000000000);

        formLayout->setWidget(1, QFormLayout::FieldRole, doubleSpinBox_2);

        label_3 = new QLabel(formLayoutWidget);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        formLayout->setWidget(2, QFormLayout::LabelRole, label_3);

        doubleSpinBox_3 = new QDoubleSpinBox(formLayoutWidget);
        doubleSpinBox_3->setObjectName(QString::fromUtf8("doubleSpinBox_3"));
        doubleSpinBox_3->setDecimals(2);
        doubleSpinBox_3->setMaximum(500.000000000000000);
        doubleSpinBox_3->setValue(100.000000000000000);

        formLayout->setWidget(2, QFormLayout::FieldRole, doubleSpinBox_3);

        label_5 = new QLabel(formLayoutWidget);
        label_5->setObjectName(QString::fromUtf8("label_5"));

        formLayout->setWidget(3, QFormLayout::LabelRole, label_5);

        doubleSpinBox_4 = new QDoubleSpinBox(formLayoutWidget);
        doubleSpinBox_4->setObjectName(QString::fromUtf8("doubleSpinBox_4"));
        doubleSpinBox_4->setMaximum(2000.000000000000000);
        doubleSpinBox_4->setValue(1000.000000000000000);

        formLayout->setWidget(3, QFormLayout::FieldRole, doubleSpinBox_4);

        label_6 = new QLabel(formLayoutWidget);
        label_6->setObjectName(QString::fromUtf8("label_6"));

        formLayout->setWidget(4, QFormLayout::LabelRole, label_6);

        doubleSpinBox_5 = new QDoubleSpinBox(formLayoutWidget);
        doubleSpinBox_5->setObjectName(QString::fromUtf8("doubleSpinBox_5"));
        doubleSpinBox_5->setMaximum(500.000000000000000);
        doubleSpinBox_5->setValue(100.000000000000000);

        formLayout->setWidget(4, QFormLayout::FieldRole, doubleSpinBox_5);

        label_7 = new QLabel(formLayoutWidget);
        label_7->setObjectName(QString::fromUtf8("label_7"));

        formLayout->setWidget(5, QFormLayout::LabelRole, label_7);

        comboBox_2 = new QComboBox(formLayoutWidget);
        comboBox_2->addItem(QString());
        comboBox_2->addItem(QString());
        comboBox_2->setObjectName(QString::fromUtf8("comboBox_2"));

        formLayout->setWidget(5, QFormLayout::FieldRole, comboBox_2);

        label_8 = new QLabel(formLayoutWidget);
        label_8->setObjectName(QString::fromUtf8("label_8"));

        formLayout->setWidget(6, QFormLayout::LabelRole, label_8);

        doubleSpinBox_6 = new QDoubleSpinBox(formLayoutWidget);
        doubleSpinBox_6->setObjectName(QString::fromUtf8("doubleSpinBox_6"));
        doubleSpinBox_6->setValue(0.500000000000000);

        formLayout->setWidget(6, QFormLayout::FieldRole, doubleSpinBox_6);

        label_9 = new QLabel(formLayoutWidget);
        label_9->setObjectName(QString::fromUtf8("label_9"));

        formLayout->setWidget(7, QFormLayout::LabelRole, label_9);

        doubleSpinBox_7 = new QDoubleSpinBox(formLayoutWidget);
        doubleSpinBox_7->setObjectName(QString::fromUtf8("doubleSpinBox_7"));
        doubleSpinBox_7->setValue(0.500000000000000);

        formLayout->setWidget(7, QFormLayout::FieldRole, doubleSpinBox_7);

        label_10 = new QLabel(formLayoutWidget);
        label_10->setObjectName(QString::fromUtf8("label_10"));

        formLayout->setWidget(8, QFormLayout::LabelRole, label_10);

        doubleSpinBox_8 = new QDoubleSpinBox(formLayoutWidget);
        doubleSpinBox_8->setObjectName(QString::fromUtf8("doubleSpinBox_8"));
        doubleSpinBox_8->setValue(1.000000000000000);

        formLayout->setWidget(8, QFormLayout::FieldRole, doubleSpinBox_8);

        label_11 = new QLabel(formLayoutWidget);
        label_11->setObjectName(QString::fromUtf8("label_11"));

        formLayout->setWidget(9, QFormLayout::LabelRole, label_11);

        doubleSpinBox_9 = new QDoubleSpinBox(formLayoutWidget);
        doubleSpinBox_9->setObjectName(QString::fromUtf8("doubleSpinBox_9"));
        doubleSpinBox_9->setDecimals(3);
        doubleSpinBox_9->setValue(0.001000000000000);

        formLayout->setWidget(9, QFormLayout::FieldRole, doubleSpinBox_9);

        label_12 = new QLabel(formLayoutWidget);
        label_12->setObjectName(QString::fromUtf8("label_12"));

        formLayout->setWidget(10, QFormLayout::LabelRole, label_12);

        comboBox_3 = new QComboBox(formLayoutWidget);
        comboBox_3->addItem(QString());
        comboBox_3->addItem(QString());
        comboBox_3->setObjectName(QString::fromUtf8("comboBox_3"));

        formLayout->setWidget(10, QFormLayout::FieldRole, comboBox_3);


        retranslateUi(Dialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), Dialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), Dialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(Dialog);
    } // setupUi

    void retranslateUi(QDialog *Dialog)
    {
        Dialog->setWindowTitle(QApplication::translate("Dialog", "Dialog", nullptr));
        pushButton->setText(QApplication::translate("Dialog", "Estimate scaffold strand length:", nullptr));
        label->setText(QApplication::translate("Dialog", "Scaling:", nullptr));
        label_13->setText(QApplication::translate("Dialog", "Iterations:", nullptr));
        label_14->setText(QString());
        label_4->setText(QApplication::translate("Dialog", "Discretize lengths", nullptr));
        comboBox->setItemText(0, QApplication::translate("Dialog", "True", nullptr));
        comboBox->setItemText(1, QApplication::translate("Dialog", "False", nullptr));

        label_2->setText(QApplication::translate("Dialog", "Density", nullptr));
        label_3->setText(QApplication::translate("Dialog", "Spring stiffness", nullptr));
        label_5->setText(QApplication::translate("Dialog", "Fixed spring stiffness", nullptr));
        label_6->setText(QApplication::translate("Dialog", "Spring damping", nullptr));
        label_7->setText(QApplication::translate("Dialog", "Attach fixed", nullptr));
        comboBox_2->setItemText(0, QApplication::translate("Dialog", "True", nullptr));
        comboBox_2->setItemText(1, QApplication::translate("Dialog", "False", nullptr));

        label_8->setText(QApplication::translate("Dialog", "Static friction", nullptr));
        label_9->setText(QApplication::translate("Dialog", "Dynamic friction", nullptr));
        label_10->setText(QApplication::translate("Dialog", "Restitution", nullptr));
        label_11->setText(QApplication::translate("Dialog", "Rigid body sleep threshold", nullptr));
        label_12->setText(QApplication::translate("Dialog", "Visual debugger", nullptr));
        comboBox_3->setItemText(0, QApplication::translate("Dialog", "Disabled", nullptr));
        comboBox_3->setItemText(1, QApplication::translate("Dialog", "Enabled", nullptr));

    } // retranslateUi

};

namespace Ui {
    class Dialog: public Ui_Dialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DIALOG_H
