/********************************************************************************
** Form generated from reading UI file 'wayfconfigdialog.ui'
**
** Created by: Qt User Interface Compiler version 5.7.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_WAYFCONFIGDIALOG_H
#define UI_WAYFCONFIGDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpinBox>

QT_BEGIN_NAMESPACE

class Ui_WAYFConfigDialog
{
public:
    QPushButton *btnSave;
    QPushButton *btnAbout;
    QPushButton *btnHelp;
    QLabel *label;
    QLabel *label_2;
    QSpinBox *speChannelGroupFriends;
    QSpinBox *speChannelGroupBlocked;
    QCheckBox *chkAutoKick;
    QLabel *label_3;
    QComboBox *cbxServers;
    QLabel *label_4;
    QSpinBox *speChannelGroupAdmin;

    void setupUi(QDialog *WAYFConfigDialog)
    {
        if (WAYFConfigDialog->objectName().isEmpty())
            WAYFConfigDialog->setObjectName(QStringLiteral("WAYFConfigDialog"));
        WAYFConfigDialog->resize(400, 300);
        btnSave = new QPushButton(WAYFConfigDialog);
        btnSave->setObjectName(QStringLiteral("btnSave"));
        btnSave->setEnabled(false);
        btnSave->setGeometry(QRect(320, 270, 75, 23));
        btnAbout = new QPushButton(WAYFConfigDialog);
        btnAbout->setObjectName(QStringLiteral("btnAbout"));
        btnAbout->setGeometry(QRect(10, 270, 75, 23));
        btnHelp = new QPushButton(WAYFConfigDialog);
        btnHelp->setObjectName(QStringLiteral("btnHelp"));
        btnHelp->setGeometry(QRect(10, 240, 75, 23));
        label = new QLabel(WAYFConfigDialog);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(10, 80, 151, 16));
        label_2 = new QLabel(WAYFConfigDialog);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setGeometry(QRect(10, 110, 191, 16));
        speChannelGroupFriends = new QSpinBox(WAYFConfigDialog);
        speChannelGroupFriends->setObjectName(QStringLiteral("speChannelGroupFriends"));
        speChannelGroupFriends->setEnabled(false);
        speChannelGroupFriends->setGeometry(QRect(270, 80, 42, 22));
        speChannelGroupBlocked = new QSpinBox(WAYFConfigDialog);
        speChannelGroupBlocked->setObjectName(QStringLiteral("speChannelGroupBlocked"));
        speChannelGroupBlocked->setEnabled(false);
        speChannelGroupBlocked->setGeometry(QRect(270, 110, 42, 22));
        chkAutoKick = new QCheckBox(WAYFConfigDialog);
        chkAutoKick->setObjectName(QStringLiteral("chkAutoKick"));
        chkAutoKick->setEnabled(false);
        chkAutoKick->setGeometry(QRect(320, 110, 70, 17));
        label_3 = new QLabel(WAYFConfigDialog);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setGeometry(QRect(10, 10, 41, 16));
        cbxServers = new QComboBox(WAYFConfigDialog);
        cbxServers->setObjectName(QStringLiteral("cbxServers"));
        cbxServers->setEnabled(false);
        cbxServers->setGeometry(QRect(60, 10, 321, 22));
        label_4 = new QLabel(WAYFConfigDialog);
        label_4->setObjectName(QStringLiteral("label_4"));
        label_4->setGeometry(QRect(10, 40, 241, 31));
        label_4->setWordWrap(true);
        speChannelGroupAdmin = new QSpinBox(WAYFConfigDialog);
        speChannelGroupAdmin->setObjectName(QStringLiteral("speChannelGroupAdmin"));
        speChannelGroupAdmin->setEnabled(false);
        speChannelGroupAdmin->setGeometry(QRect(270, 50, 42, 22));
        QWidget::setTabOrder(cbxServers, speChannelGroupAdmin);
        QWidget::setTabOrder(speChannelGroupAdmin, speChannelGroupFriends);
        QWidget::setTabOrder(speChannelGroupFriends, speChannelGroupBlocked);
        QWidget::setTabOrder(speChannelGroupBlocked, chkAutoKick);
        QWidget::setTabOrder(chkAutoKick, btnSave);
        QWidget::setTabOrder(btnSave, btnHelp);
        QWidget::setTabOrder(btnHelp, btnAbout);

        retranslateUi(WAYFConfigDialog);

        QMetaObject::connectSlotsByName(WAYFConfigDialog);
    } // setupUi

    void retranslateUi(QDialog *WAYFConfigDialog)
    {
        WAYFConfigDialog->setWindowTitle(QApplication::translate("WAYFConfigDialog", "We are your friends by Leibi - Einstellungen", Q_NULLPTR));
        btnSave->setText(QApplication::translate("WAYFConfigDialog", "Speichern", Q_NULLPTR));
        btnAbout->setText(QApplication::translate("WAYFConfigDialog", "\303\234ber...", Q_NULLPTR));
        btnHelp->setText(QApplication::translate("WAYFConfigDialog", "Hilfe?", Q_NULLPTR));
        label->setText(QApplication::translate("WAYFConfigDialog", "Channel Gruppe f\303\274r Freunde:", Q_NULLPTR));
        label_2->setText(QApplication::translate("WAYFConfigDialog", "Channel Gruppe f\303\274r blockierte Nutzer:", Q_NULLPTR));
        chkAutoKick->setText(QApplication::translate("WAYFConfigDialog", "Auto-Kick", Q_NULLPTR));
        label_3->setText(QApplication::translate("WAYFConfigDialog", "Server:", Q_NULLPTR));
        label_4->setText(QApplication::translate("WAYFConfigDialog", "Welche Channel Gruppe musst du haben, damit das Plugin wirkt (i. d. R. Channel Admin)?", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class WAYFConfigDialog: public Ui_WAYFConfigDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_WAYFCONFIGDIALOG_H
