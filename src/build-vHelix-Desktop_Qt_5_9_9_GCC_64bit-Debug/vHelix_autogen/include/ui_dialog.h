/********************************************************************************
** Form generated from reading UI file 'dialog.ui'
**
** Created by: Qt User Interface Compiler version 5.9.9
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DIALOG_H
#define UI_DIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
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
            Dialog->setObjectName(QStringLiteral("Dialog"));
        Dialog->resize(519, 443);
        buttonBox = new QDialogButtonBox(Dialog);
        buttonBox->setObjectName(QStringLiteral("buttonBox"));
        buttonBox->setGeometry(QRect(30, 400, 481, 32));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        gridLayoutWidget = new QWidget(Dialog);
        gridLayoutWidget->setObjectName(QStringLiteral("gridLayoutWidget"));
        gridLayoutWidget->setGeometry(QRect(10, 0, 501, 74));
        gridLayout = new QGridLayout(gridLayoutWidget);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        gridLayout->setContentsMargins(0, 0, 0, 0);
        pushButton = new QPushButton(gridLayoutWidget);
        pushButton->setObjectName(QStringLiteral("pushButton"));

        gridLayout->addWidget(pushButton, 1, 1, 1, 1);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        label = new QLabel(gridLayoutWidget);
        label->setObjectName(QStringLiteral("label"));

        verticalLayout_2->addWidget(label);

        doubleSpinBox = new QDoubleSpinBox(gridLayoutWidget);
        doubleSpinBox->setObjectName(QStringLiteral("doubleSpinBox"));
        doubleSpinBox->setValue(1);

        verticalLayout_2->addWidget(doubleSpinBox);


        horizontalLayout->addLayout(verticalLayout_2);

        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        label_13 = new QLabel(gridLayoutWidget);
        label_13->setObjectName(QStringLiteral("label_13"));

        verticalLayout->addWidget(label_13);

        spinBox = new QSpinBox(gridLayoutWidget);
        spinBox->setObjectName(QStringLiteral("spinBox"));
        spinBox->setValue(1);

        verticalLayout->addWidget(spinBox);


        horizontalLayout->addLayout(verticalLayout);


        gridLayout->addLayout(horizontalLayout, 1, 0, 1, 1);

        label_14 = new QLabel(gridLayoutWidget);
        label_14->setObjectName(QStringLiteral("label_14"));

        gridLayout->addWidget(label_14, 1, 2, 1, 1);

        formLayoutWidget = new QWidget(Dialog);
        formLayoutWidget->setObjectName(QStringLiteral("formLayoutWidget"));
        formLayoutWidget->setGeometry(QRect(10, 80, 501, 311));
        formLayout = new QFormLayout(formLayoutWidget);
        formLayout->setObjectName(QStringLiteral("formLayout"));
        formLayout->setContentsMargins(0, 0, 0, 0);
        label_4 = new QLabel(formLayoutWidget);
        label_4->setObjectName(QStringLiteral("label_4"));

        formLayout->setWidget(0, QFormLayout::LabelRole, label_4);

        comboBox = new QComboBox(formLayoutWidget);
        comboBox->setObjectName(QStringLiteral("comboBox"));

        formLayout->setWidget(0, QFormLayout::FieldRole, comboBox);

        label_2 = new QLabel(formLayoutWidget);
        label_2->setObjectName(QStringLiteral("label_2"));

        formLayout->setWidget(1, QFormLayout::LabelRole, label_2);

        doubleSpinBox_2 = new QDoubleSpinBox(formLayoutWidget);
        doubleSpinBox_2->setObjectName(QStringLiteral("doubleSpinBox_2"));
        doubleSpinBox_2->setValue(10);

        formLayout->setWidget(1, QFormLayout::FieldRole, doubleSpinBox_2);

        label_3 = new QLabel(formLayoutWidget);
        label_3->setObjectName(QStringLiteral("label_3"));

        formLayout->setWidget(2, QFormLayout::LabelRole, label_3);

        doubleSpinBox_3 = new QDoubleSpinBox(formLayoutWidget);
        doubleSpinBox_3->setObjectName(QStringLiteral("doubleSpinBox_3"));
        doubleSpinBox_3->setDecimals(2);
        doubleSpinBox_3->setMaximum(500);
        doubleSpinBox_3->setValue(100);

        formLayout->setWidget(2, QFormLayout::FieldRole, doubleSpinBox_3);

        label_5 = new QLabel(formLayoutWidget);
        label_5->setObjectName(QStringLiteral("label_5"));

        formLayout->setWidget(3, QFormLayout::LabelRole, label_5);

        doubleSpinBox_4 = new QDoubleSpinBox(formLayoutWidget);
        doubleSpinBox_4->setObjectName(QStringLiteral("doubleSpinBox_4"));
        doubleSpinBox_4->setMaximum(2000);
        doubleSpinBox_4->setValue(1000);

        formLayout->setWidget(3, QFormLayout::FieldRole, doubleSpinBox_4);

        label_6 = new QLabel(formLayoutWidget);
        label_6->setObjectName(QStringLiteral("label_6"));

        formLayout->setWidget(4, QFormLayout::LabelRole, label_6);

        doubleSpinBox_5 = new QDoubleSpinBox(formLayoutWidget);
        doubleSpinBox_5->setObjectName(QStringLiteral("doubleSpinBox_5"));
        doubleSpinBox_5->setMaximum(500);
        doubleSpinBox_5->setValue(100);

        formLayout->setWidget(4, QFormLayout::FieldRole, doubleSpinBox_5);

        label_7 = new QLabel(formLayoutWidget);
        label_7->setObjectName(QStringLiteral("label_7"));

        formLayout->setWidget(5, QFormLayout::LabelRole, label_7);

        comboBox_2 = new QComboBox(formLayoutWidget);
        comboBox_2->setObjectName(QStringLiteral("comboBox_2"));

        formLayout->setWidget(5, QFormLayout::FieldRole, comboBox_2);

        label_8 = new QLabel(formLayoutWidget);
        label_8->setObjectName(QStringLiteral("label_8"));

        formLayout->setWidget(6, QFormLayout::LabelRole, label_8);

        doubleSpinBox_6 = new QDoubleSpinBox(formLayoutWidget);
        doubleSpinBox_6->setObjectName(QStringLiteral("doubleSpinBox_6"));
        doubleSpinBox_6->setValue(0.5);

        formLayout->setWidget(6, QFormLayout::FieldRole, doubleSpinBox_6);

        label_9 = new QLabel(formLayoutWidget);
        label_9->setObjectName(QStringLiteral("label_9"));

        formLayout->setWidget(7, QFormLayout::LabelRole, label_9);

        doubleSpinBox_7 = new QDoubleSpinBox(formLayoutWidget);
        doubleSpinBox_7->setObjectName(QStringLiteral("doubleSpinBox_7"));
        doubleSpinBox_7->setValue(0.5);

        formLayout->setWidget(7, QFormLayout::FieldRole, doubleSpinBox_7);

        label_10 = new QLabel(formLayoutWidget);
        label_10->setObjectName(QStringLiteral("label_10"));

        formLayout->setWidget(8, QFormLayout::LabelRole, label_10);

        doubleSpinBox_8 = new QDoubleSpinBox(formLayoutWidget);
        doubleSpinBox_8->setObjectName(QStringLiteral("doubleSpinBox_8"));
        doubleSpinBox_8->setValue(1);

        formLayout->setWidget(8, QFormLayout::FieldRole, doubleSpinBox_8);

        label_11 = new QLabel(formLayoutWidget);
        label_11->setObjectName(QStringLiteral("label_11"));

        formLayout->setWidget(9, QFormLayout::LabelRole, label_11);

        doubleSpinBox_9 = new QDoubleSpinBox(formLayoutWidget);
        doubleSpinBox_9->setObjectName(QStringLiteral("doubleSpinBox_9"));
        doubleSpinBox_9->setDecimals(3);
        doubleSpinBox_9->setValue(0.001);

        formLayout->setWidget(9, QFormLayout::FieldRole, doubleSpinBox_9);

        label_12 = new QLabel(formLayoutWidget);
        label_12->setObjectName(QStringLiteral("label_12"));

        formLayout->setWidget(10, QFormLayout::LabelRole, label_12);

        comboBox_3 = new QComboBox(formLayoutWidget);
        comboBox_3->setObjectName(QStringLiteral("comboBox_3"));

        formLayout->setWidget(10, QFormLayout::FieldRole, comboBox_3);


        retranslateUi(Dialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), Dialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), Dialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(Dialog);
    } // setupUi

    void retranslateUi(QDialog *Dialog)
    {
        Dialog->setWindowTitle(QApplication::translate("Dialog", "Dialog", Q_NULLPTR));
        pushButton->setText(QApplication::translate("Dialog", "Estimate scaffold strand length:", Q_NULLPTR));
        label->setText(QApplication::translate("Dialog", "Scaling:", Q_NULLPTR));
        label_13->setText(QApplication::translate("Dialog", "Iterations:", Q_NULLPTR));
        label_14->setText(QString());
        label_4->setText(QApplication::translate("Dialog", "Discretize lengths", Q_NULLPTR));
        comboBox->clear();
        comboBox->insertItems(0, QStringList()
         << QApplication::translate("Dialog", "True", Q_NULLPTR)
         << QApplication::translate("Dialog", "False", Q_NULLPTR)
        );
        label_2->setText(QApplication::translate("Dialog", "Density", Q_NULLPTR));
        label_3->setText(QApplication::translate("Dialog", "Spring stiffness", Q_NULLPTR));
        label_5->setText(QApplication::translate("Dialog", "Fixed spring stiffness", Q_NULLPTR));
        label_6->setText(QApplication::translate("Dialog", "Spring damping", Q_NULLPTR));
        label_7->setText(QApplication::translate("Dialog", "Attach fixed", Q_NULLPTR));
        comboBox_2->clear();
        comboBox_2->insertItems(0, QStringList()
         << QApplication::translate("Dialog", "True", Q_NULLPTR)
         << QApplication::translate("Dialog", "False", Q_NULLPTR)
        );
        label_8->setText(QApplication::translate("Dialog", "Static friction", Q_NULLPTR));
        label_9->setText(QApplication::translate("Dialog", "Dynamic friction", Q_NULLPTR));
        label_10->setText(QApplication::translate("Dialog", "Restitution", Q_NULLPTR));
        label_11->setText(QApplication::translate("Dialog", "Rigid body sleep threshold", Q_NULLPTR));
        label_12->setText(QApplication::translate("Dialog", "Visual debugger", Q_NULLPTR));
        comboBox_3->clear();
        comboBox_3->insertItems(0, QStringList()
         << QApplication::translate("Dialog", "Disabled", Q_NULLPTR)
         << QApplication::translate("Dialog", "Enabled", Q_NULLPTR)
        );
    } // retranslateUi

};

namespace Ui {
    class Dialog: public Ui_Dialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DIALOG_H
