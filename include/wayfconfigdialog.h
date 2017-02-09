#ifndef WAYFCONFIGDIALOG_H
#define WAYFCONFIGDIALOG_H

#include <QDialog>

namespace Ui {
	class WAYFConfigDialog;
}

class WAYFConfigDialog : public QDialog
{
	Q_OBJECT

public:
	explicit WAYFConfigDialog(QWidget *parent = 0);
	~WAYFConfigDialog();
	static bool isValid;

	private slots:
	void on_btnAbout_clicked();
	void on_btnSave_clicked();
	void on_btnHelp_clicked();
	void on_cbxServers_currentIndexChanged(int index);

private:
	Ui::WAYFConfigDialog *ui;
	QString WAYFSettingsFile;
	bool LoadSettings();
	bool settingsLoaded;
	//int WAYFGetSettingAsInt(char* name);
	//void WAYFSetSettingFromInt(char* name, int sInt);
};

#endif // WAYFCONFIGDIALOG_H
