/********************************************************************************
** Form generated from reading UI file 'scaffoldfreedialog.ui'
**
** Created by: Qt User Interface Compiler version 5.9.9
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SCAFFOLDFREEDIALOG_H
#define UI_SCAFFOLDFREEDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ScaffoldfreeDialog
{
public:
    QDialogButtonBox *buttonBox;
    QLabel *label;
    QDoubleSpinBox *doubleSpinBox;
    QPushButton *pushButton;
    QWidget *verticalLayoutWidget;
    QVBoxLayout *verticalLayout;
    QCheckBox *autofillStrandGaps;
    QLabel *label_5;
    QComboBox *comboBox;

    void setupUi(QDialog *ScaffoldfreeDialog)
    {
        if (ScaffoldfreeDialog->objectName().isEmpty())
            ScaffoldfreeDialog->setObjectName(QStringLiteral("ScaffoldfreeDialog"));
        ScaffoldfreeDialog->resize(400, 300);
        buttonBox = new QDialogButtonBox(ScaffoldfreeDialog);
        buttonBox->setObjectName(QStringLiteral("buttonBox"));
        buttonBox->setGeometry(QRect(30, 240, 341, 32));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        label = new QLabel(ScaffoldfreeDialog);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(20, 20, 51, 21));
        doubleSpinBox = new QDoubleSpinBox(ScaffoldfreeDialog);
        doubleSpinBox->setObjectName(QStringLiteral("doubleSpinBox"));
        doubleSpinBox->setGeometry(QRect(100, 20, 191, 23));
        doubleSpinBox->setValue(1);
        pushButton = new QPushButton(ScaffoldfreeDialog);
        pushButton->setObjectName(QStringLiteral("pushButton"));
        pushButton->setGeometry(QRect(20, 60, 291, 22));
        verticalLayoutWidget = new QWidget(ScaffoldfreeDialog);
        verticalLayoutWidget->setObjectName(QStringLiteral("verticalLayoutWidget"));
        verticalLayoutWidget->setGeometry(QRect(20, 100, 231, 41));
        verticalLayout = new QVBoxLayout(verticalLayoutWidget);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        autofillStrandGaps = new QCheckBox(verticalLayoutWidget);
        autofillStrandGaps->setObjectName(QStringLiteral("autofillStrandGaps"));

        verticalLayout->addWidget(autofillStrandGaps);

        label_5 = new QLabel(ScaffoldfreeDialog);
        label_5->setObjectName(QStringLiteral("label_5"));
        label_5->setGeometry(QRect(20, 160, 211, 20));
        comboBox = new QComboBox(ScaffoldfreeDialog);
        comboBox->setObjectName(QStringLiteral("comboBox"));
        comboBox->setGeometry(QRect(20, 190, 221, 22));

        retranslateUi(ScaffoldfreeDialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), ScaffoldfreeDialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), ScaffoldfreeDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(ScaffoldfreeDialog);
    } // setupUi

    void retranslateUi(QDialog *ScaffoldfreeDialog)
    {
        ScaffoldfreeDialog->setWindowTitle(QApplication::translate("ScaffoldfreeDialog", "Dialog", Q_NULLPTR));
        label->setText(QApplication::translate("ScaffoldfreeDialog", "Scaling:", Q_NULLPTR));
        pushButton->setText(QApplication::translate("ScaffoldfreeDialog", "Estimate base usage:", Q_NULLPTR));
        autofillStrandGaps->setText(QApplication::translate("ScaffoldfreeDialog", "Autofill strand gaps", Q_NULLPTR));
        label_5->setText(QApplication::translate("ScaffoldfreeDialog", "Generate strands", Q_NULLPTR));
        comboBox->clear();
        comboBox->insertItems(0, QStringList()
         << QApplication::translate("ScaffoldfreeDialog", "None", Q_NULLPTR)
         << QApplication::translate("ScaffoldfreeDialog", "Random", Q_NULLPTR)
         << QApplication::translate("ScaffoldfreeDialog", "Simulated annealing", Q_NULLPTR)
        );
    } // retranslateUi

};

namespace Ui {
    class ScaffoldfreeDialog: public Ui_ScaffoldfreeDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SCAFFOLDFREEDIALOG_H
