#include "view.h"
#include <QApplication>
#include <QMessageBox>
#include <QtSql>
#include <QDebug>

QString APP_NAME = "Obsidian";
QString ORG_NAME = "Philip Schlösser";
QString APP_VERS = "3.3.1";
QString ORG_DOMAIN = "https://github.com/philip98/";

short DB_PORT = 3306;
QString DB_IP = "127.0.0.1";
QString DB_USER = "bverwaltung";
QString DB_PASSWORD = "Buecherverwaltungsprogramm";
QString DB_NAME = "biblio";
QString PDF_VIEWER = "/usr/bin/evince";
QString DOC_FILE = "/usr/share/doc/Obsidian/doc.pdf";

QBrush STUDENT_FREE = QBrush(QColor(127, 255, 127));

/*!
 * \brief Gibt eine Fehlermeldung aus
 * \param query QSqlQuery-Objekt, mit dem der Fehler geschah
 *
 * Erst wird der letzte Fehler ermittelt und einige gängige Fehlermeldungen übersetzt (1451, 1452),
 * schließlich wird der Fehlertext inkl. Fehlernummer in Form einer QMessageBox ausgegeben.
 */
void sqlError(QSqlQuery query) {
	QSqlError e = query.lastError();
	switch(e.number()) {
	case 1451:
		QMessageBox::critical(0, "Fehler", "Datensatz kann nicht gelöscht oder geändert "
				      "werden,\nda die Daten noch benötigt werden! - 1451");
		break;
	case 1452:
		QMessageBox::critical(0, "Fehler", "Datensatz kann nicht eingefügt oder geändert "
				      "werden,\nda Schüler/Lehrer/Buch noch nicht existiert! - 1452");
		break;
	default:
		QMessageBox::information(0, "Sql-Abfrage", query.lastQuery());
		QMessageBox::critical(0, "Fehler", QString("%1 - %2").arg(e.text()).arg(e.number()));
		break;
	}
}

/*!
 * \brief Helferfunktion für die Ausführung einer SQL-Abfrage
 * \param query SQL-Abfrage
 * \param qObj QSqlQuery-Objekt, mit dem die Abfrage ausgeführt werden soll
 * \return true - Abfrage erfolgreich, false - Fehler
 *
 * Diese Funktion dient dazu, eine Abfrage auszuführen und ggf. die Abfrage ausgeben.
 */
bool exec(QString query, QSqlQuery q) {
	return q.exec(query);
}

/*!
 * \brief SQL-Konforme Bearbeitung von Strings
 * \param par Der zu bearbeitende String
 * \return  Der bearbeitete String
 */
QString escape(QString par) {
	QString res;
	QString r;
	for (int i = 0; i < par.length(); ++i) {
		switch (par[i].toLatin1()) {
		case '\'':
			r = "\\\'";
			break;
		case '\"':
			r = "\\\"";
			break;
		case '\\':
			r = "\\\\";
			break;
		case '\n':
			r = "\\n";
			break;
		case '\t':
			r = "\\t";
			break;
		default:
			r = par[i];
		}
		res += r;
	}
	return res;
}

/*!
 * \brief Helferfunktion für die Ausführung eines Prepared Statements
 * \param q Enthält das Statement
 * \return true - Abfrage erfolgreich
 *
 * Das Prepared Statement wird ausgeführt und im Falle eines Fehlers sqlError() aufgerufen.
 */
bool exec(QSqlQuery &q) {
	bool b = q.exec();
	if (!b)
		sqlError(q);
	return b;
}

/*!
 * \brief Führt ein Prepared Statement aus und wählt falls vorhanden den ersten Datensatz an
 * \param q Enthält das Statement
 * \return true - alles erfolgreich
 */
bool exec_first(QSqlQuery *q) {
	if (!q->exec()) {
		sqlError(*q);
		return false;
	}
	return (q->first() || (q->size() == 0));
}

void load_settings() {
	QSettings settings;
	if (!settings.contains("db/port"))
		settings.setValue("db/port", DB_PORT);
	if (!settings.contains("db/ip"))
		settings.setValue("db/ip", DB_IP);
	if (!settings.contains("db/name"))
		settings.setValue("db/name", DB_NAME);
	if (!settings.contains("db/usr"))
		settings.setValue("db/usr", DB_USER);
	if (!settings.contains("db/pwd"))
		settings.setValue("db/pwd", DB_PASSWORD);
	if (!settings.contains("allg/pdf-viewer"))
		settings.setValue("allg/pdf-viewer", PDF_VIEWER);
	if (!settings.contains("allg/doc"))
		settings.setValue("allg/doc", DOC_FILE);
	DB_PORT = settings.value("db/port").toInt();
	DB_IP = settings.value("db/ip").toString();
	DB_NAME = settings.value("db/name").toString();
	DB_USER = settings.value("db/usr").toString();
	DB_PASSWORD = settings.value("db/pwd").toString();
	PDF_VIEWER = settings.value("allg/pdf-viewer").toString();
	DOC_FILE = settings.value("allg/doc").toString();
}

/*!
 * \brief Hauptprogramm
 * \param argc Anzahl der Argumente
 * \param argv Argumente in Form eines char **
 * \return 0 im Falle eines Erfolgs
 *
 * Zuerst wird das Textformat auf UTF-8 gesetzt, dann die Datenbankverbindung aufgebaut
 * und schließlich das Hauptfenster instanziiert und angezeigt.
 */
int main(int argc, char *argv[])
{
	QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
	QApplication a(argc, argv);
	QCoreApplication::setApplicationName(APP_NAME);
	QCoreApplication::setApplicationVersion(APP_VERS);
	QCoreApplication::setOrganizationName(ORG_NAME);
	QCoreApplication::setOrganizationDomain(ORG_DOMAIN);

	load_settings();
	QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
	db.setPort(DB_PORT);
	db.setHostName(DB_IP);
	db.setDatabaseName(DB_NAME);
	db.setUserName(DB_USER);
	db.setPassword(DB_PASSWORD);
	bool ok = db.open();

	if (!ok) {
	  QMessageBox::critical(NULL, "Fehler", db.lastError().text());
	}
	a.setWindowIcon(QIcon(":images/icon.png"));
	View *v = new View;
	v->setAttribute(Qt::WA_QuitOnClose, true);
	v->setWindowState(Qt::WindowMaximized);
	v->show();
	return a.exec();
}
