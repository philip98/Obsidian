#ifndef INSERTDLG_S_H
#define INSERTDLG_S_H

#include "main.h"
#include "basedlg.h"
#include <QDateEdit>
#include <QTabWidget>

/*!
 * \brief Der Ausleih-Dialog
 */
class InsertDialog : public BaseDialog {
	Q_OBJECT
public:
	InsertDialog(bool endOfTerm, QWidget *parent = 0, QSqlRecord record = QSqlRecord());

private:
	QDateEdit *a_date;                       ///< Enthält das Ausleihdatum
	void m_createComponents();
	void m_alignComponents();
	void m_setInitialValues(QSqlRecord record);
	void m_insertBTausch();
	void m_insertXAusleihe();

public slots:
	void accept();
};

/*!
 * \brief Der Einfügen-Dialog in der Tabellenansicht
 */
class InsertionDialog : public QDialog {
	Q_OBJECT
private:
	int a_tabIndex;                          ///< Tab in den Eingefügt werden soll
	QSqlQuery a_q;                           ///< Zu verwendendes QSqlQuery-Objekt
	QDialogButtonBox *a_buttonBox;           ///< Ok/Abbrechen
	QTabWidget *a_tabWidget;                 ///< Strukturiert die Eingabefelder
	QLineEdit *a_studentName;                ///< Enthält den Namen eines Schülers
	QLineEdit *a_studentGradYear;            ///< Enthält das Abschlussjahr eines Schülers
	QLineEdit *a_studentFormLetter;          ///< Enthält den Klassenbuchstaben eines Schülers
	QLineEdit *a_teacherAbbrev;              ///< Enthält das Kürzel eines Lehrers
	QLineEdit *a_teacherName;                ///< Enthält den Namen eines Lehrers
	QLineEdit *a_bookIsbn;                   ///< Enthält die ISBN eines Buchs
	QLineEdit *a_bookTitle;                  ///< Enthält den Titel eines Buchs
	QLineEdit *a_bookForm;                   ///< Enthält die Jgst eines Buchs

	void m_createComponents();
	void m_alignComponents();
	void m_setInitialValues();
	void m_connectComponents();

public:
	InsertionDialog(int tabIndex, QWidget *parent);

private slots:
	void tabChanged(int tabIndex);

public slots:
	void accept();
};

#endif // INSERTDLG_S_H
