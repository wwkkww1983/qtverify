/********************************************************************************
** Form generated from reading UI file 'logindialog.ui'
**
** Created by: Qt User Interface Compiler version 4.8.5
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_LOGINDIALOG_H
#define UI_LOGINDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QComboBox>
#include <QtGui/QDialog>
#include <QtGui/QGridLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_LoginDialogClass
{
public:
    QPushButton *loginButton;
    QPushButton *quitButton;
    QWidget *layoutWidget;
    QGridLayout *gridLayout;
    QLabel *label_2;
    QComboBox *userComboBox;
    QLabel *label;
    QLineEdit *passwordEdit;

    void setupUi(QDialog *LoginDialogClass)
    {
        if (LoginDialogClass->objectName().isEmpty())
            LoginDialogClass->setObjectName(QString::fromUtf8("LoginDialogClass"));
        LoginDialogClass->resize(406, 291);
        loginButton = new QPushButton(LoginDialogClass);
        loginButton->setObjectName(QString::fromUtf8("loginButton"));
        loginButton->setGeometry(QRect(160, 250, 93, 28));
        quitButton = new QPushButton(LoginDialogClass);
        quitButton->setObjectName(QString::fromUtf8("quitButton"));
        quitButton->setGeometry(QRect(290, 250, 93, 28));
        layoutWidget = new QWidget(LoginDialogClass);
        layoutWidget->setObjectName(QString::fromUtf8("layoutWidget"));
        layoutWidget->setGeometry(QRect(60, 60, 301, 71));
        gridLayout = new QGridLayout(layoutWidget);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        gridLayout->setContentsMargins(0, 0, 0, 0);
        label_2 = new QLabel(layoutWidget);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        gridLayout->addWidget(label_2, 0, 0, 1, 1);

        userComboBox = new QComboBox(layoutWidget);
        userComboBox->setObjectName(QString::fromUtf8("userComboBox"));

        gridLayout->addWidget(userComboBox, 0, 1, 1, 1);

        label = new QLabel(layoutWidget);
        label->setObjectName(QString::fromUtf8("label"));

        gridLayout->addWidget(label, 1, 0, 1, 1);

        passwordEdit = new QLineEdit(layoutWidget);
        passwordEdit->setObjectName(QString::fromUtf8("passwordEdit"));

        gridLayout->addWidget(passwordEdit, 1, 1, 1, 1);


        retranslateUi(LoginDialogClass);

        QMetaObject::connectSlotsByName(LoginDialogClass);
    } // setupUi

    void retranslateUi(QDialog *LoginDialogClass)
    {
        LoginDialogClass->setWindowTitle(QApplication::translate("LoginDialogClass", "Dialog", 0, QApplication::UnicodeUTF8));
        loginButton->setText(QApplication::translate("LoginDialogClass", "\347\231\273\345\275\225", 0, QApplication::UnicodeUTF8));
        quitButton->setText(QApplication::translate("LoginDialogClass", "\351\200\200\345\207\272", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("LoginDialogClass", "\347\224\250\346\210\267\357\274\232", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("LoginDialogClass", "\345\257\206\347\240\201\357\274\232", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class LoginDialogClass: public Ui_LoginDialogClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_LOGINDIALOG_H