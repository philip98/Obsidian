#include "basedlg.h"
#include "selectdlg.h"
#include <QtCore>
#include <QtGui>
#include <QtWidgets>
#include <QtSql>

/*!
 * \brief Konstruktor des BaseDialogs
 * \param endOfTerm Gibt an, ob es sich um eine Büchertausch-Ausleihe handelt
 * \param parent Elternwidget
 *
 *  Außer den QDialog-Konstruktor aufzurufen und a_endOfTerm zu initialisieren,
 *  wird hier nichts getan.
 */
BaseDialog::BaseDialog(bool endOfTerm, QWidget *parent) : QDialog(parent),
	a_eot(endOfTerm) {
}

/*!
 * \brief Ist für den Aufbau der Benutzeroberfläche zuständig
 * \param record Datensatz, der einen Schüler identifiziert
 * \see m_createComponents(), m_alignComponents(), m_setInitialValues(), m_connectComponents()
 *
 * Alle Aufgaben werden weiterdelegiert
 */
void BaseDialog::m_createInterface(QSqlRecord record) {
	m_createComponents();
	m_alignComponents();
	m_setInitialValues(record);
	m_connectComponents();
}

/*!
 * \brief Ist für die Erstellung der Komponenten zuständig
 */
void BaseDialog::m_createComponents() {
	a_buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
					   Qt::Vertical);
	a_choose = new QPushButton(tr("Schüler auswählen..."));
	a_name = new QLabel;
	a_form = new QLabel;
	for (int i = 0; i < a_numIsbns; ++i) {
		a_alias[i] = new QLineEdit;
		a_title[i] = new QLabel;
	}
	a_teacher = new QRadioButton(tr("Lehrer"));
	a_student = new QRadioButton(tr("Schüler"));
	a_lender = new QGroupBox;
}

/*!
 * \brief Initialisiert die Komponenten
 * \param record Datensatz, der einen Schüler identifiziert
 *
 * Für jedes ISBN-Feld werden Maximallänge und StatusTip gesetzt. Schließlich wird
 * der Inhalt von a_name und a_form sowie a_id entsprechend des Datensatzes gesetzt.
 * Zum Schluss wird a_student angewählt und ggf. a_teacher deaktiviert.
 */
void BaseDialog::m_setInitialValues(QSqlRecord record) {
	a_id = -1;
	for (int i = 0; i < a_numIsbns; ++i) {
		a_alias[i]->setMaxLength(13);
		a_alias[i]->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
		a_alias[i]->setStatusTip(tr("Bitte die ISBN nicht eintippen, sondern einscannen!"));
	}

	if (!record.isEmpty()) {
		a_name->setText(record.value("Name").toString());
		a_id = record.value("ID").toInt();
		a_form->setText(record.value("Klasse").toString());
	}
	a_student->setChecked(true);
	a_teacher->setDisabled(a_eot);
}

/*!
 * \brief Verbindet alle Komponenten mit den entsprechenden Slots
 */
void BaseDialog::m_connectComponents() {
	connect(a_choose, SIGNAL(clicked()), this, SLOT(chooseLender()));
	connect(a_buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
	connect(a_buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
	connect(a_student, SIGNAL(toggled(bool)), this, SLOT(toggle()));
	connect(a_teacher, SIGNAL(toggled(bool)), this, SLOT(toggle()));

	for (int i = 0; i < a_numIsbns; ++i)
		connect(a_alias[i], SIGNAL(editingFinished()), this, SLOT(bookLookup()));
}

/*!
 * \brief Wird bei Klick auf a_choose aufgerufen
 *
 * Ein neuer SelectDialog wird erzeugt und ausgeführt. Anschließend werden
 * seine Hinterlassenschaften ausgewertet.
 */
void BaseDialog::chooseLender() {
	SelectDialog *sd = new SelectDialog(a_student->isChecked(), this);
	if (sd->exec()) {
		QSqlRecord r = sd->getRecord();
		a_id = r.value("id").toInt();
		a_form->setText(r.value("Klasse").toString());
		a_name->setText(r.value("Name").toString());
	}
	delete sd;
}

/*!
 * \brief Wird bei Klick auf a_teacher oder a_student aufgerufen
 *
 * Entsprechend der Auswahl wird der Text von a_choose verändert.
 * Anschließend werden a_name, a_form und a_id zurückgesetzt.
 */
void BaseDialog::toggle() {
	if (a_student->isChecked())
		a_choose->setText(tr("Schüler auswählen..."));
	else
		a_choose->setText(tr("Lehrer auswählen..."));

	a_name->clear();
	a_form->clear();
	a_id = 0;
}

/*!
 * \brief Wird beim Verlassen eines a_alias-Feldes aufgerufen
 *
 * Eine SQL-Abfrage nach soeben eingegebenen ISBN wird ausgeführt und im
 * a_title-Feld neben dem a_alias-Feld wird der Titel des gerade eingescannten
 * Buches angezeigt.
 */
void BaseDialog::bookLookup() {
	QString text = qobject_cast<QLineEdit *>(sender())->text();
	int i;

	if (text == QString())
		return;
	for (i = 0; i < a_numIsbns; ++i)
		if (a_alias[i] == sender())
			break;

	a_q.prepare("SELECT * FROM `aliasse` WHERE `alias` = :alias");
	a_q.bindValue(":alias", text.toLower());
	a_q.exec();
	if (a_q.next())
		a_isbn[i] = a_q.value("isbn").toString();
	else if (a_q.size() == 0)
		a_isbn[i] = text;
	a_q.prepare("SELECT * FROM `buch` WHERE `isbn` = :isbn");
	a_q.bindValue(":isbn", a_isbn[i]);

	if (!a_q.exec() || !a_q.first()) {
		QMessageBox::warning(this, tr("Buch nicht gefunden"),
				     tr("Das Buch mit der ISBN %1 konnte nicht gefunden werden.").arg(text));
		return;
	}
	a_title[i]->setText(QString("%1 %2").arg(a_q.value("name").toString()).arg(a_q.value("jgst").toString()));
}
