#include "wayfconfigdialog.h"
#include "ui_wayfconfigdialog.h"
#include <qmessagebox.h>
#include <qstandardpaths.h>
#include <qsettings.h>
#include <qdesktopwidget.h>

WAYFConfigDialog::WAYFConfigDialog(QWidget *parent) :
QDialog(parent),
ui(new Ui::WAYFConfigDialog)
{
	ui->setupUi(this);
	WAYFSettingsFile = QStandardPaths::locate(QStandardPaths::AppLocalDataLocation, QString(), QStandardPaths::LocateDirectory) + "wayf.ini";

	settingsLoaded = false;
	isValid = true;

	if (!LoadSettings())
	{
		QMessageBox m(QMessageBox::Critical, "We are your friends by Leibi", "Fehler!\n\nBitte verbinde dich zuerst mit einem Server, bevor du das Plugin konfigurierst.", QMessageBox::NoButton, this);
		QSize mSize = m.sizeHint();
		QRect screenRect = QDesktopWidget().screen()->rect();
		m.move(QPoint(screenRect.width() / 2 - mSize.width() / 2,
			screenRect.height() / 2 - mSize.height() / 2));

		m.exec();
		isValid = false;
	}
}

bool WAYFConfigDialog::isValid = true;

WAYFConfigDialog::~WAYFConfigDialog()
{
	delete ui;
}

void WAYFConfigDialog::on_btnAbout_clicked()
{
	QMessageBox* msg = new QMessageBox(this);
	msg->setIcon(QMessageBox::Information);
	msg->setWindowTitle("We are your friends by Leibi");
	msg->setTextFormat(Qt::RichText);
	msg->setText("Autor: Leibi<br />Webseite: <a href='https://teamspeak-plugins.org'>Teamspeak Plugins</a>");
	msg->exec();
}

void WAYFConfigDialog::on_btnHelp_clicked()
{
	QMessageBox* msg = new QMessageBox(this);
	msg->setIcon(QMessageBox::Information);
	msg->setWindowTitle("We are your friends by Leibi");
	msg->setTextFormat(Qt::RichText);
	msg->setText("Das &#34;We are your friends&#34;-Plugin (kurz: WAYF) weist Freunden oder blockierten Nutzern, die deinen Channel betreten, eine festgelegte Channel Gruppe zu.<br /><br />Schritt-f&#252;r-Schritt-Anleitung:<br />1. Verbinde dich mit einem Server.<br />2. Dr&#252;cke STRG+F4 um die Channel Gruppen zu sehen (alternativ: Men&#252; unter &#34;Rechte&#34; &gt; &#34;Channel Gruppen&#34;)<br />3. Notiere dir die Channel Gruppen, die du Freunden und blockierten Nutzern automatisch zuweisen willst.<br />4. Speichere diese Channel Gruppen in den WAYF-Plugin-Einstellungen ab.<br /><br />Wenn das Plugin richtig konfiguriert ist, werden Freunden und blockierten Nutzern die angegebenen Channel Gruppen beim Joinen in deinen Channel vergeben.");
	msg->exec();
}

void WAYFConfigDialog::on_btnSave_clicked()
{
	QSettings settings(WAYFSettingsFile, QSettings::IniFormat);
	QString selectedGroupKey = this->findChild<QComboBox*>("cbxServers")->itemData(this->findChild<QComboBox*>("cbxServers")->currentIndex()).toString();
	settings.setValue(selectedGroupKey + "/ChannelGroupAdmin", this->findChild<QSpinBox*>("speChannelGroupAdmin")->value());
	settings.setValue(selectedGroupKey + "/ChannelGroupFriends", this->findChild<QSpinBox*>("speChannelGroupFriends")->value());
	settings.setValue(selectedGroupKey + "/ChannelGroupBlocked", this->findChild<QSpinBox*>("speChannelGroupBlocked")->value());
	settings.setValue(selectedGroupKey + "/AutoKick", this->findChild<QCheckBox*>("chkAutoKick")->isChecked());
	settings.sync();

	QMessageBox::information(this, "We are your friends by Leibi", "Die Einstellungen wurden gespeichert.", QMessageBox::Ok);
	this->close();
}

bool WAYFConfigDialog::LoadSettings()
{
	QSettings settings(WAYFSettingsFile, QSettings::IniFormat);
	QStringList lstGroups = settings.childGroups();
	if (lstGroups.size() > 0)
	{
		for (int i = 0; i < lstGroups.size(); ++i)
		{
			this->findChild<QComboBox*>("cbxServers")->addItem(settings.value(lstGroups.at(i) + "/ServerName", QString()).toString(), lstGroups.at(i));
		}
		this->findChild<QComboBox*>("cbxServers")->setEnabled(true);

		QString selectedGroupKey = this->findChild<QComboBox*>("cbxServers")->itemData(this->findChild<QComboBox*>("cbxServers")->currentIndex()).toString();

		this->findChild<QSpinBox*>("speChannelGroupAdmin")->setValue(settings.value(selectedGroupKey + "/ChannelGroupAdmin", 0).toInt());
		this->findChild<QSpinBox*>("speChannelGroupFriends")->setValue(settings.value(selectedGroupKey + "/ChannelGroupFriends", 0).toInt());
		this->findChild<QSpinBox*>("speChannelGroupBlocked")->setValue(settings.value(selectedGroupKey + "/ChannelGroupBlocked", 0).toInt());
		this->findChild<QCheckBox*>("chkAutoKick")->setChecked(settings.value(selectedGroupKey + "/AutoKick", false).toBool());

		this->findChild<QSpinBox*>("speChannelGroupAdmin")->setEnabled(true);
		this->findChild<QSpinBox*>("speChannelGroupFriends")->setEnabled(true);
		this->findChild<QSpinBox*>("speChannelGroupBlocked")->setEnabled(true);
		this->findChild<QCheckBox*>("chkAutoKick")->setEnabled(true);
		this->findChild<QPushButton*>("btnSave")->setEnabled(true);

		settingsLoaded = true;
		return settingsLoaded;
	}
	return false;
}

void WAYFConfigDialog::on_cbxServers_currentIndexChanged(int index)
{
	if (settingsLoaded)
	{
		printf("WAYF: on_cbxServers_currentIndexChanged\n");
		QSettings settings(WAYFSettingsFile, QSettings::IniFormat);
		QString selectedGroupKey = this->findChild<QComboBox*>("cbxServers")->itemData(index).toString();
		this->findChild<QSpinBox*>("speChannelGroupAdmin")->setValue(settings.value(selectedGroupKey + "/ChannelGroupAdmin", 0).toInt());
		this->findChild<QSpinBox*>("speChannelGroupFriends")->setValue(settings.value(selectedGroupKey + "/ChannelGroupFriends", 0).toInt());
		this->findChild<QSpinBox*>("speChannelGroupBlocked")->setValue(settings.value(selectedGroupKey + "/ChannelGroupBlocked", 0).toInt());
		this->findChild<QCheckBox*>("chkAutoKick")->setChecked(settings.value(selectedGroupKey + "/AutoKick", false).toBool());
	}
}