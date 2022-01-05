/********************************************************************************
** Form generated from reading UI file 'rpolydialog.ui'
**
** Created by: Qt User Interface Compiler version 5.9.9
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_RPOLYDIALOG_H
#define UI_RPOLYDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_RpolyDialog
{
public:
    QWidget *formLayoutWidget;
    QFormLayout *formLayout;
    QLabel *label_3;
    QLabel *label;
    QSpinBox *spinBox;
    QLabel *label_2;
    QSpinBox *spinBox_2;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *RpolyDialog)
    {
        if (RpolyDialog->objectName().isEmpty())
            RpolyDialog->setObjectName(QStringLiteral("RpolyDialog"));
        RpolyDialog->resize(300, 136);
        formLayoutWidget = new QWidget(RpolyDialog);
        formLayoutWidget->setObjectName(QStringLiteral("formLayoutWidget"));
        formLayoutWidget->setGeometry(QRect(10, 0, 281, 131));
        formLayout = new QFormLayout(formLayoutWidget);
        formLayout->setObjectName(QStringLiteral("formLayout"));
        formLayout->setContentsMargins(0, 0, 0, 0);
        label_3 = new QLabel(formLayoutWidget);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setLayoutDirection(Qt::LeftToRight);
        label_3->setWordWrap(true);

        formLayout->setWidget(0, QFormLayout::SpanningRole, label_3);

        label = new QLabel(formLayoutWidget);
        label->setObjectName(QStringLiteral("label"));

        formLayout->setWidget(1, QFormLayout::LabelRole, label);

        spinBox = new QSpinBox(formLayoutWidget);
        spinBox->setObjectName(QStringLiteral("spinBox"));
        spinBox->setMinimum(10);
        spinBox->setValue(20);

        formLayout->setWidget(1, QFormLayout::FieldRole, spinBox);

        label_2 = new QLabel(formLayoutWidget);
        label_2->setObjectName(QStringLiteral("label_2"));

        formLayout->setWidget(2, QFormLayout::LabelRole, label_2);

        spinBox_2 = new QSpinBox(formLayoutWidget);
        spinBox_2->setObjectName(QStringLiteral("spinBox_2"));
        spinBox_2->setMinimum(20);
        spinBox_2->setValue(40);

        formLayout->setWidget(2, QFormLayout::FieldRole, spinBox_2);

        buttonBox = new QDialogButtonBox(formLayoutWidget);
        buttonBox->setObjectName(QStringLiteral("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        formLayout->setWidget(3, QFormLayout::FieldRole, buttonBox);


        retranslateUi(RpolyDialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), RpolyDialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), RpolyDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(RpolyDialog);
    } // setupUi

    void retranslateUi(QDialog *RpolyDialog)
    {
        RpolyDialog->setWindowTitle(QApplication::translate("RpolyDialog", "Routed mesh import options", Q_NULLPTR));
        label_3->setText(QApplication::translate("RpolyDialog", "Desired staple strand length in bases\n"
" (use an interval of at least 10-20 bases):", Q_NULLPTR));
        label->setText(QApplication::translate("RpolyDialog", "Minimum length", Q_NULLPTR));
        label_2->setText(QApplication::translate("RpolyDialog", "Maximum length", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class RpolyDialog: public Ui_RpolyDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_RPOLYDIALOG_H
