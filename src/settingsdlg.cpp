#include "settingsdlg.h"
#include "main.h"
#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QRegExpValidator>
#include <QFileDialog>
#include <QMessageBox>

/*!
 * \brief Konstruktor von SettingsDialog
 * \param parent Eltern-Widget
 * \see m_createComponents(), m_alignComponents, m_setInitialValues(), m_connectComponents()
 */
SettingsDialog::SettingsDialog(QWidget *parent) : QDialog(parent) {
	m_createComponents();
	m_alignComponents();
	m_setInitialValues();
	m_connectComponents();
}

/*!
 * \brief Erstellt die Komponenten
 */
void SettingsDialog::m_createComponents() {
	a_buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
					   Qt::Vertical);
	a_general = new QGroupBox(tr("Allgemeine Einstellungen"));
	a_database = new QGroupBox(tr("Datenbankeinstellungen"));
	a_pdf = new QLineEdit;
	a_choosePDF = new QPushButton(tr("Suchen..."));
	a_doc = new QLineEdit;
	a_chooseDoc = new QPushButton(tr("Suchen..."));
	a_dbName = new QLineEdit;
	a_dbPassword = new QLineEdit;
	a_dbPort = new QLineEdit;
	a_dbServer = new QLineEdit;
	a_dbUser = new QLineEdit;
}

/*!
 * \brief Ordnet die Komponenten in Layouts an
 */
void SettingsDialog::m_alignComponents() {
	QGridLayout *a = new QGridLayout;
	a->addWidget(new QLabel("PDF-Viewer"), 0, 0);
	a->addWidget(a_pdf, 0, 1);
	a->addWidget(a_choosePDF, 0, 2);
	a->addWidget(new QLabel("Dokumentation"), 1, 0);
	a->addWidget(a_doc, 1, 1);
	a->addWidget(a_chooseDoc, 1, 2);
	a_general->setLayout(a);

	QGridLayout *b = new QGridLayout;
	b->addWidget(new QLabel(tr("IP des Servers")), 0, 0);
	b->addWidget(a_dbServer, 0, 1);
	b->addWidget(new QLabel(tr("Port des Servers")), 1, 0);
	b->addWidget(a_dbPort, 1, 1);
	b->addWidget(new QLabel(tr("Benutzer")), 2, 0);
	b->addWidget(a_dbUser, 2, 1);
	b->addWidget(new QLabel(tr("Passwort")), 3, 0);
	b->addWidget(a_dbPassword, 3, 1);
	b->addWidget(new QLabel(tr("Name der Datenbank")), 4, 0);
	b->addWidget(a_dbName, 4, 1);
	a_database->setLayout(b);

	QVBoxLayout *c = new QVBoxLayout;
	c->addWidget(a_general);
	c->addWidget(a_database);

	QHBoxLayout *layout = new QHBoxLayout;
	layout->addLayout(c);
	layout->addWidget(a_buttonBox);
	setLayout(layout);
}

/*!
 * \brief Setzt die Anfangswerte
 *
 * Erst werden die Einstellungen aus der Datei gelesen und dann die
 * RegEx-Validatoren für a_dbServer und a_dbPort gesetzt.
 */
void SettingsDialog::m_setInitialValues() {
	setWindowTitle(tr("Einstellungen"));
	a_pdf->setText(PDF_VIEWER);
	a_doc->setText(DOC_FILE);
	a_dbName->setText(DB_NAME);
	a_dbPort->setText(tr("%1").arg(DB_PORT));
	a_dbServer->setText(DB_IP);
	a_dbPassword->setText(DB_PASSWORD);
	a_dbUser->setText(DB_USER);

	QRegExpValidator *ip = new QRegExpValidator(QRegExp("([0-2]?[0-9]?[0-9].){3}[0-2]?[0-9]?[0-9]"));
	QRegExpValidator *port = new QRegExpValidator(QRegExp("[0-9]{0, 5}"));

	a_dbServer->setValidator(ip);
	a_dbPort->setValidator(port);
}

/*!
 * \brief Verbindet die Komponenten mit den Slots
 */
void SettingsDialog::m_connectComponents() {
	connect(a_buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
	connect(a_buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
	connect(a_choosePDF, SIGNAL(clicked()), this, SLOT(choosePdfViewer()));
	connect(a_chooseDoc, SIGNAL(clicked()), this, SLOT(chooseDocFile()));
}

/*!
 * \brief Wird bei Klick auf a_choose ausgelöst
 *
 * Startet einen QFileDialog zur Auswahl des PDF-Viewers.
 */
void SettingsDialog::choosePdfViewer() {
	a_pdf->setText(QFileDialog::getOpenFileName(this, tr("PDF-Viewer suchen"), "/usr/bin"));
}

void SettingsDialog::chooseDocFile() {
	a_doc->setText(QFileDialog::getOpenFileName(this, tr("Dokumentation suchen"), "/usr/share/doc",
						    "*.pdf"));
}

/*!
 * \brief Wird bei Klick auf Ok ausgelöst
 *
 * Zuerst wird überprüft, ob die Portnummer unter 65536 ist und dann werden alle Einstellungen
 * gespeichert.
 */
void SettingsDialog::accept() {
	if (a_dbPort->text().toInt() > 65536 || a_dbPort->text().toInt() < 0) {
		QMessageBox::critical(this, "Fehler", tr("Ungültige Portnummer! (1 - 65535)"));
		return;
	}
	PDF_VIEWER = a_pdf->text();
	DOC_FILE = a_doc->text();
	DB_IP = a_dbServer->text();
	DB_PORT = a_dbPort->text().toShort();
	DB_USER = a_dbUser->text();
	DB_PASSWORD = a_dbPassword->text();
	DB_NAME = a_dbName->text();
	QSettings settings;
	settings.setValue("allg/pdf-viewer", PDF_VIEWER);
	settings.setValue("allg/doc", DOC_FILE);
	settings.setValue("db/ip", DB_IP);
	settings.setValue("db/port", DB_PORT);
	settings.setValue("db/usr", DB_USER);
	settings.setValue("db/pwd", DB_PASSWORD);
	settings.setValue("db/name", DB_NAME);
	QDialog::accept();
}
