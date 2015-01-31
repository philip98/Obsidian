#ifndef BASEDLG_H
#define BASEDLG_H

#include "main.h"
#include <QDialog>
#include <QWidget>
#include <QSqlRecord>
#include <QSqlQuery>
#include <QLabel>
#include <QLineEdit>
#include <QGroupBox>
#include <QRadioButton>
#include <QPushButton>
#include <QDialogButtonBox>

/*!
 * \brief Die Basisklasse für Ausleih- und Rückgabedialog
 */
class BaseDialog : public QDialog {
	Q_OBJECT
public:
	BaseDialog(bool endOfTerm, QWidget *parent = 0);

protected:
	void m_createInterface(QSqlRecord record = QSqlRecord());
	virtual void m_createComponents();
	virtual void m_alignComponents() = 0;
	virtual void m_setInitialValues(QSqlRecord record);
	virtual void m_connectComponents();

	static constexpr int a_numIsbns = 16;    ///< Anzahl der ISBN-Felder
	int a_id;                                ///< Id des Schülers/Lehrers
	bool a_eot;                              ///< Ist es eine normale Ausleihe mitten im Schuljahr?
	QString a_isbn[a_numIsbns];                  ///< Enthält die eindeutige ISBN des Buches
	QSqlQuery a_q;                           ///< Zu verwendendes QSqlQuery-Objekt
	QLabel *a_name;                          ///< Enthält den Namen des Schülers/Lehrers
	QLabel *a_form;                          ///< Enthält die Klasse des Schülers/das Kürzel des Lehrers
	QLabel *a_title[a_numIsbns];             ///< Enthält den Titel des Buches
	QLineEdit *a_alias[a_numIsbns];          ///< Enthält die ISBN/die Abkürzung des Buches
	QGroupBox *a_lender;                     ///< Enthält die Auswahlmöglichkeit Lehrer/Schüler
	QRadioButton *a_teacher;                 ///< Ist anzuwählen falls ein Lehrer ausleiht oder zurückgibt
	QRadioButton *a_student;                 ///< Ist anzuwählen falls ein Schüler ausleiht oder zurückgibt
	QPushButton *a_choose;                   ///< Öffnet den Dialog, mit dem man den Lehrer/Schüler auswählen kann
	QDialogButtonBox *a_buttonBox;           ///< Ok/Abbrechen

protected slots:
	void chooseLender();
	void toggle();
	void bookLookup();
};

#endif
