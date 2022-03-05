/********************************************************************************
** Form generated from reading UI file 'settingsdialog.ui'
**
** Created by: Qt User Interface Compiler version 5.9.9
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SETTINGSDIALOG_H
#define UI_SETTINGSDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_SettingsDialog
{
public:
    QLabel *label;
    QWidget *verticalLayoutWidget;
    QVBoxLayout *verticalLayout;
    QCheckBox *checkBox;
    QLabel *label_2;
    QWidget *formLayoutWidget;
    QFormLayout *formLayout;
    QLabel *discretizeLengthsLabel;
    QCheckBox *discretizeLengthsCheckBox;
    QLabel *label_3;
    QDoubleSpinBox *doubleSpinBox_2;
    QLabel *label_5;
    QDoubleSpinBox *doubleSpinBox_3;
    QLabel *label_6;
    QDoubleSpinBox *doubleSpinBox_4;
    QLabel *label_7;
    QDoubleSpinBox *doubleSpinBox_5;
    QLabel *attachFixedLabel;
    QCheckBox *attachFixedCheckBox;
    QLabel *label_9;
    QDoubleSpinBox *doubleSpinBox_6;
    QLabel *label_10;
    QDoubleSpinBox *doubleSpinBox_7;
    QLabel *label_11;
    QDoubleSpinBox *doubleSpinBox_8;
    QLabel *label_12;
    QDoubleSpinBox *doubleSpinBox_9;
    QLabel *visualDebuggerLabel;
    QCheckBox *visualDebuggerCheckBox;
    QLabel *iterationsLabel;
    QSpinBox *iterationsSpinBox;
    QDialogButtonBox *buttonBox;
    QPushButton *SetDefaults;

    void setupUi(QDialog *SettingsDialog)
    {
        if (SettingsDialog->objectName().isEmpty())
            SettingsDialog->setObjectName(QStringLiteral("SettingsDialog"));
        SettingsDialog->resize(515, 503);
        label = new QLabel(SettingsDialog);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(10, 10, 111, 31));
        verticalLayoutWidget = new QWidget(SettingsDialog);
        verticalLayoutWidget->setObjectName(QStringLiteral("verticalLayoutWidget"));
        verticalLayoutWidget->setGeometry(QRect(10, 40, 211, 41));
        verticalLayout = new QVBoxLayout(verticalLayoutWidget);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        checkBox = new QCheckBox(verticalLayoutWidget);
        checkBox->setObjectName(QStringLiteral("checkBox"));

        verticalLayout->addWidget(checkBox);

        label_2 = new QLabel(SettingsDialog);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setGeometry(QRect(10, 90, 101, 41));
        formLayoutWidget = new QWidget(SettingsDialog);
        formLayoutWidget->setObjectName(QStringLiteral("formLayoutWidget"));
        formLayoutWidget->setGeometry(QRect(10, 140, 501, 352));
        formLayout = new QFormLayout(formLayoutWidget);
        formLayout->setObjectName(QStringLiteral("formLayout"));
        formLayout->setContentsMargins(0, 0, 0, 0);
        discretizeLengthsLabel = new QLabel(formLayoutWidget);
        discretizeLengthsLabel->setObjectName(QStringLiteral("discretizeLengthsLabel"));

        formLayout->setWidget(0, QFormLayout::LabelRole, discretizeLengthsLabel);

        discretizeLengthsCheckBox = new QCheckBox(formLayoutWidget);
        discretizeLengthsCheckBox->setObjectName(QStringLiteral("discretizeLengthsCheckBox"));
        discretizeLengthsCheckBox->setChecked(true);

        formLayout->setWidget(0, QFormLayout::FieldRole, discretizeLengthsCheckBox);

        label_3 = new QLabel(formLayoutWidget);
        label_3->setObjectName(QStringLiteral("label_3"));

        formLayout->setWidget(1, QFormLayout::LabelRole, label_3);

        doubleSpinBox_2 = new QDoubleSpinBox(formLayoutWidget);
        doubleSpinBox_2->setObjectName(QStringLiteral("doubleSpinBox_2"));
        doubleSpinBox_2->setValue(10);

        formLayout->setWidget(1, QFormLayout::FieldRole, doubleSpinBox_2);

        label_5 = new QLabel(formLayoutWidget);
        label_5->setObjectName(QStringLiteral("label_5"));

        formLayout->setWidget(2, QFormLayout::LabelRole, label_5);

        doubleSpinBox_3 = new QDoubleSpinBox(formLayoutWidget);
        doubleSpinBox_3->setObjectName(QStringLiteral("doubleSpinBox_3"));
        doubleSpinBox_3->setDecimals(2);
        doubleSpinBox_3->setMaximum(500);
        doubleSpinBox_3->setValue(100);

        formLayout->setWidget(2, QFormLayout::FieldRole, doubleSpinBox_3);

        label_6 = new QLabel(formLayoutWidget);
        label_6->setObjectName(QStringLiteral("label_6"));

        formLayout->setWidget(3, QFormLayout::LabelRole, label_6);

        doubleSpinBox_4 = new QDoubleSpinBox(formLayoutWidget);
        doubleSpinBox_4->setObjectName(QStringLiteral("doubleSpinBox_4"));
        doubleSpinBox_4->setMaximum(2000);
        doubleSpinBox_4->setValue(1000);

        formLayout->setWidget(3, QFormLayout::FieldRole, doubleSpinBox_4);

        label_7 = new QLabel(formLayoutWidget);
        label_7->setObjectName(QStringLiteral("label_7"));

        formLayout->setWidget(4, QFormLayout::LabelRole, label_7);

        doubleSpinBox_5 = new QDoubleSpinBox(formLayoutWidget);
        doubleSpinBox_5->setObjectName(QStringLiteral("doubleSpinBox_5"));
        doubleSpinBox_5->setMaximum(500);
        doubleSpinBox_5->setValue(100);

        formLayout->setWidget(4, QFormLayout::FieldRole, doubleSpinBox_5);

        attachFixedLabel = new QLabel(formLayoutWidget);
        attachFixedLabel->setObjectName(QStringLiteral("attachFixedLabel"));

        formLayout->setWidget(5, QFormLayout::LabelRole, attachFixedLabel);

        attachFixedCheckBox = new QCheckBox(formLayoutWidget);
        attachFixedCheckBox->setObjectName(QStringLiteral("attachFixedCheckBox"));
        attachFixedCheckBox->setChecked(true);

        formLayout->setWidget(5, QFormLayout::FieldRole, attachFixedCheckBox);

        label_9 = new QLabel(formLayoutWidget);
        label_9->setObjectName(QStringLiteral("label_9"));

        formLayout->setWidget(6, QFormLayout::LabelRole, label_9);

        doubleSpinBox_6 = new QDoubleSpinBox(formLayoutWidget);
        doubleSpinBox_6->setObjectName(QStringLiteral("doubleSpinBox_6"));
        doubleSpinBox_6->setValue(0.5);

        formLayout->setWidget(6, QFormLayout::FieldRole, doubleSpinBox_6);

        label_10 = new QLabel(formLayoutWidget);
        label_10->setObjectName(QStringLiteral("label_10"));

        formLayout->setWidget(7, QFormLayout::LabelRole, label_10);

        doubleSpinBox_7 = new QDoubleSpinBox(formLayoutWidget);
        doubleSpinBox_7->setObjectName(QStringLiteral("doubleSpinBox_7"));
        doubleSpinBox_7->setValue(0.5);

        formLayout->setWidget(7, QFormLayout::FieldRole, doubleSpinBox_7);

        label_11 = new QLabel(formLayoutWidget);
        label_11->setObjectName(QStringLiteral("label_11"));

        formLayout->setWidget(8, QFormLayout::LabelRole, label_11);

        doubleSpinBox_8 = new QDoubleSpinBox(formLayoutWidget);
        doubleSpinBox_8->setObjectName(QStringLiteral("doubleSpinBox_8"));
        doubleSpinBox_8->setValue(1);

        formLayout->setWidget(8, QFormLayout::FieldRole, doubleSpinBox_8);

        label_12 = new QLabel(formLayoutWidget);
        label_12->setObjectName(QStringLiteral("label_12"));

        formLayout->setWidget(9, QFormLayout::LabelRole, label_12);

        doubleSpinBox_9 = new QDoubleSpinBox(formLayoutWidget);
        doubleSpinBox_9->setObjectName(QStringLiteral("doubleSpinBox_9"));
        doubleSpinBox_9->setDecimals(3);
        doubleSpinBox_9->setValue(0.001);

        formLayout->setWidget(9, QFormLayout::FieldRole, doubleSpinBox_9);

        visualDebuggerLabel = new QLabel(formLayoutWidget);
        visualDebuggerLabel->setObjectName(QStringLiteral("visualDebuggerLabel"));

        formLayout->setWidget(10, QFormLayout::LabelRole, visualDebuggerLabel);

        visualDebuggerCheckBox = new QCheckBox(formLayoutWidget);
        visualDebuggerCheckBox->setObjectName(QStringLiteral("visualDebuggerCheckBox"));

        formLayout->setWidget(10, QFormLayout::FieldRole, visualDebuggerCheckBox);

        iterationsLabel = new QLabel(formLayoutWidget);
        iterationsLabel->setObjectName(QStringLiteral("iterationsLabel"));

        formLayout->setWidget(11, QFormLayout::LabelRole, iterationsLabel);

        iterationsSpinBox = new QSpinBox(formLayoutWidget);
        iterationsSpinBox->setObjectName(QStringLiteral("iterationsSpinBox"));
        iterationsSpinBox->setMinimum(1);
        iterationsSpinBox->setValue(1);

        formLayout->setWidget(11, QFormLayout::FieldRole, iterationsSpinBox);

        buttonBox = new QDialogButtonBox(SettingsDialog);
        buttonBox->setObjectName(QStringLiteral("buttonBox"));
        buttonBox->setGeometry(QRect(340, 480, 166, 22));
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        SetDefaults = new QPushButton(SettingsDialog);
        SetDefaults->setObjectName(QStringLiteral("SetDefaults"));
        SetDefaults->setGeometry(QRect(10, 480, 91, 22));

        retranslateUi(SettingsDialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), SettingsDialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), SettingsDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(SettingsDialog);
    } // setupUi

    void retranslateUi(QDialog *SettingsDialog)
    {
        SettingsDialog->setWindowTitle(QApplication::translate("SettingsDialog", "Dialog", Q_NULLPTR));
        label->setText(QApplication::translate("SettingsDialog", "<html><head/><body><p><span style=\" font-size:12pt; font-weight:600;\">Routing</span></p></body></html>", Q_NULLPTR));
        checkBox->setText(QApplication::translate("SettingsDialog", "Write intermediate files", Q_NULLPTR));
        label_2->setText(QApplication::translate("SettingsDialog", "<html><head/><body><p><span style=\" font-size:12pt; font-weight:600;\">PhysX</span></p></body></html>", Q_NULLPTR));
        discretizeLengthsLabel->setText(QApplication::translate("SettingsDialog", "Discretize lengths", Q_NULLPTR));
        label_3->setText(QApplication::translate("SettingsDialog", "Density", Q_NULLPTR));
        label_5->setText(QApplication::translate("SettingsDialog", "Spring stiffness", Q_NULLPTR));
        label_6->setText(QApplication::translate("SettingsDialog", "Fixed spring stiffness", Q_NULLPTR));
        label_7->setText(QApplication::translate("SettingsDialog", "Spring damping", Q_NULLPTR));
        attachFixedLabel->setText(QApplication::translate("SettingsDialog", "Attach fixed", Q_NULLPTR));
        label_9->setText(QApplication::translate("SettingsDialog", "Static friction", Q_NULLPTR));
        label_10->setText(QApplication::translate("SettingsDialog", "Dynamic friction", Q_NULLPTR));
        label_11->setText(QApplication::translate("SettingsDialog", "Restitution", Q_NULLPTR));
        label_12->setText(QApplication::translate("SettingsDialog", "Rigid body sleep threshold", Q_NULLPTR));
        visualDebuggerLabel->setText(QApplication::translate("SettingsDialog", "Visual debugger", Q_NULLPTR));
        iterationsLabel->setText(QApplication::translate("SettingsDialog", "Iterations", Q_NULLPTR));
        SetDefaults->setText(QApplication::translate("SettingsDialog", "Set defaults", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class SettingsDialog: public Ui_SettingsDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SETTINGSDIALOG_H
