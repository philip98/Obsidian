#ifndef SELECTDLG_H
#define SELECTDLG_H

#include <QDialog>
#include <QTableView>
#include <QDialogButtonBox>
#include <QSqlQueryModel>
#include <QPushButton>
#include <QLineEdit>
#include <QCheckBox>
#include <QDateEdit>
#include <QTableWidget>
#include <QSqlRecord>

/*!
 * \brief Dialog, mit dem ein Schüler oder Lehrer ausgewählt wird
 */
class SelectDialog : public QDialog {
	Q_OBJECT
public:
	SelectDialog(bool isStudent, QWidget *parent = 0);
	QSqlRecord getRecord();

private:
	bool a_isStudent;                        ///< Verweist auf die Art der Person
	QTableView *a_tableView;                 ///< Zeigt die Vorschläge an
	QPushButton *a_search;                   ///< Dient zur Suche nach einem Teil des Namens
	QLineEdit *a_nameFragment;               ///< Enthält einen Teil des Namens
	QDialogButtonBox *a_buttonBox;           ///< Ok/Abbrechen
	QSqlQueryModel *a_tableModel;            ///< Datenmodell für a_tableWidget

	void m_createComponents();
	void m_alignComponents();
	void m_setInitialValues();
	void m_connectComponents();

private slots:
	void searchName();

public slots:

};

/*!
 * \brief Suchen-Dialog der Tabellenansicht
 */
class FindDialog : public QDialog {
	Q_OBJECT
private:
	int a_tabIndex;                          ///< Aktuell ausgewählter Tab

	QDialogButtonBox *a_buttonBox;           ///< Ok/Abbrechen
	QTabWidget *a_tabWidget;                 ///< Strukturierung der einzelnen Abfragekriterien
	QLineEdit *a_studentId;                  ///< Enthält die lfd.Nr. eines Schülers
	QLineEdit *a_studentName;                ///< Enthält den Namen eines Schülers
	QLineEdit *a_studentGradYear;            ///< Enthält das Abschlussjahr eines Schülers
	QLineEdit *a_studentFormLetter;          ///< Enthält den Klassenbuchstaben eines Schülers
	QLineEdit *a_teacherId;                  ///< Enthält die lfd.Nr. eines Lehrers
	QLineEdit *a_teacherAbbrev;              ///< Enthält das Kürzel eines Lehrers
	QLineEdit *a_teacherName;                ///< Enthält den Namen eines Lehrers
	QLineEdit *a_bookIsbn;                   ///< Enthält die ISBN eines Buches
	QLineEdit *a_bookTitle;                  ///< Enthält den Titel eines Buches
	QLineEdit *a_bookForm;                   ///< Enthält die Jgst. eines Buches
	QLineEdit *a_sLendForm;                  ///< Enthält die Klasse eines Schülers (`sausleihe`)
	QDateEdit *a_sLendDate;                  ///< Enthält ein Ausleihdatum (`sausleihe`)
	QLineEdit *a_sLendName;                  ///< Enthält den Namen eines Schülers (`sausleihe`)
	QLineEdit *a_sLendTitle;                 ///< Enthält den Titel eines Buches (`sausleihe`)
	QLineEdit *a_tLendAbbrev;                ///< Enthält das Kürzel eines Lehrers (`lausleihe`)
	QLineEdit *a_tLendName;                  ///< Enthält den Namen eines Lehrers(`lausleihe`)
	QLineEdit *a_tLendTitle;                 ///< Enthält den Titel eines Buches (`lausleihe`)
	QDateEdit *a_tLendDate;                  ///< Enthält ein Ausleihdatum (`lausleihe`)
	QLineEdit *a_swapForm;                   ///< Enthält die Klasse eines Schülers (`btausch`)
	QLineEdit *a_swapName;                   ///< Enthält den Namen eines Schülers (`btausch`)
	QLineEdit *a_swapTitle;                  ///< Enthält den Titel eines Buches (`btausch`)
	QDateEdit *a_swapDate;                   ///< Enthält ein Ausgabedatum (`btausch`)
	QLineEdit *a_swapYear;                   ///< Enthält ein Ausleihjahr (`btausch`)

	void m_createComponents();
	void m_alignComponents();
	void m_setInitialValues();
	void m_connectComponents();

public:
	FindDialog(int tabIndex, QWidget *parent = 0);
	QString getQuery();
	int getIndex();

private slots:
	void indexChanged(int tabIndex);
};

#endif
