#include "view.h"
#include <QApplication>
#include <QMessageBox>
#include <QtSql>
#include <QDebug>

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
//	QMessageBox::information(0, "SQL-Abfrage", query);
//	qDebug() << query;
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
bool exec(QSqlQuery q) {
	bool b = q.exec();
//	QString s = q.executedQuery() + '\n';
//	for (int i = 0; i < q.boundValues().size(); ++i)
//		s += QObject::tr("%1: %2, ").arg(q.boundValues().keys()[i]).arg(q.boundValues().values()[i].toString());
//	QMessageBox::information(0, "SQL-Abfrage", s);
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
	QSettings settings("buecherverwalter.conf", QSettings::IniFormat);
	QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
	db.setPort(settings.value("db/port", 3306).toInt());
	db.setHostName(settings.value("db/ip", "127.0.0.1").toString());
	db.setDatabaseName(settings.value("db/name", "biblio").toString());
	db.setUserName(settings.value("db/usr", "bverwaltung").toString());
	db.setPassword(settings.value("db/pwd", "Buecherverwaltungsprogramm").toString());
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


