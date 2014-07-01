#ifndef SETTINGSDLG_H
#define SETTINGSDLG_H

#include <QDialog>
#include <QDialogButtonBox>
#include <QLabel>
#include <QLineEdit>
#include <QFileDialog>
#include <QGroupBox>
#include <QPushButton>
#include <QSettings>

/*!
 * \brief Der Einstellungs-Dialog
 */
class SettingsDialog : public QDialog {
	Q_OBJECT
private:
	QDialogButtonBox *a_buttonBox;           ///< Ok/Abbrechen
	QGroupBox *a_general;                    ///< Enthält allgemeine Einstellungen
	QGroupBox *a_database;                   ///< Enthält Einstellungen für die Datenbank
	QLineEdit *a_pdf;                        ///< Enthält das Anzeigeprogramm für die Doku
	QPushButton *a_choose;                   ///< Dient der Suche nach einem PDF-Viewer
	QLineEdit *a_dbName;                     ///< Enthält den Namen der Datenbank
	QLineEdit *a_dbPort;                     ///< Enthält den Port des Datenbankservers
	QLineEdit *a_dbServer;                   ///< Enthält die IP des Datenbankservers
	QLineEdit *a_dbPassword;                 ///< Enthält das Passwort für den Datenbankserver
	QLineEdit *a_dbUser;                     ///< Enthält den Benutzername für den Datenbankserver

	void m_createComponents();
	void m_alignComponents();
	void m_setInitialValues();
	void m_connectComponents();

public:
	SettingsDialog(QWidget *parent = 0);

private slots:
	void choosePdfViewer();

public slots:
	void accept();

};

#endif
