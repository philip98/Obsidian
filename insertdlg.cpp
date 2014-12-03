#include "main.h"
#include "insertdlg.h"
#include "selectdlg.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QSpacerItem>
#include <QInputDialog>
#include <QMessageBox>
#include <QtSql>

/*!
 * \brief Konstruktor von InsertDialog
 * \param endOfTerm Gibt an, ob es sich um eine Ausleihe im Rahmen des Büchertauschs am Schuljahresende handelt
 * \param parent Elternwidget
 * \param record Datensatz, der einen Schüler identifiziert
 * \see BaseDialog(), m_createInterface()
 */
InsertDialog::InsertDialog(bool endOfTerm, QWidget *parent, QSqlRecord record)
	: BaseDialog(endOfTerm, parent) {
	m_createInterface(record);
}

/*!
 * \brief Ordnet die Komponenten in Layouts an
 */
void InsertDialog::m_alignComponents() {
	QHBoxLayout *a = new QHBoxLayout;
	a->addWidget(a_form);
	a->addWidget(a_name);
	a->addWidget(a_choose);

	QVBoxLayout *b = new QVBoxLayout;
	b->addWidget(a_student);
	b->addWidget(a_teacher);
	a_lender->setLayout(b);

	QHBoxLayout *c = new QHBoxLayout;
	c->addWidget(a_date);
	c->addSpacerItem(new QSpacerItem(1, 1, QSizePolicy::Expanding,
					      QSizePolicy::Fixed));
	c->addWidget(a_lender);

	QGridLayout *d = new QGridLayout;
	d->addLayout(a, 0, 0, 1, 2);
	d->addLayout(c, 1, 0, 1, 2);
	for (int i = 0; i < a_numIsbns; ++i) {
		d->addWidget(a_alias[i], i + 2, 0);
		d->addWidget(a_title[i], i + 2, 1);
	}

	QHBoxLayout *layout = new QHBoxLayout;
	layout->addLayout(d);
	layout->addWidget(a_buttonBox);
	setLayout(layout);
}

/*!
 * \brief Erzeugt die Komponenten
 * \see BaseDialog::m_createComponents()
 *
 * Neben dem Aufruf der Basismethode wird hier das Datumsfeld erzeugt.
 */
void InsertDialog::m_createComponents() {
	BaseDialog::m_createComponents();
	a_date = new QDateEdit(QDate::currentDate());
}

/*!
 * \brief Initialisiert die Komponenten
 * \param record Datensatz, der einen Schüler identifiziert
 *
 * Neben dem Aufruf der Basismethode wird hier auch der Titel des Fensters
 * gesetzt.
 */
void InsertDialog::m_setInitialValues(QSqlRecord record) {
	BaseDialog::m_setInitialValues(record);
	setWindowTitle(tr("Ausleihe buchen"));
}

/*!
 * \brief Wird bei Klick auf Ok aufgerufen
 *
 * Erst wird überprüft, ob überhaupt Bücher eingescannt wurden oder ob überhaupt ein Ausleihender ausgewählt wurde.
 * Bei normalen Ausleihen wird dann zunächst per SQL-Abfrage überprüft, ob das Buch von dem Schüler schon einmal
 * ausgeliehen wurde. In dem Falle wird einfach der Wert des `anzahl`-Feldes erhöht. Sonst wird ein neuer Datensatz
 * mit Schülernummer, den einzelnen ISBNs und Datum in die entsprechende Tabelle (`sausleihe`/`lausleihe`) eingefügt.
 *
 * Am Schuljahresende hingegen wird einfach nur ein Datensatz mit den entsprechenden Daten in `sausleihe` eingefügt.
 */
void InsertDialog::accept() {
	if (a_id == -1) {
		QMessageBox::warning(this, ("Fehlende Angaben"), tr("Noch kein %1 ausgewählt!")
				     .arg(a_teacher->isChecked() ? tr("Lehrer") : tr("Schüler")));
		return;
	}
	if (a_alias[0]->text().isEmpty()) {
		QMessageBox::warning(this, tr("Fehlende Angaben"), tr("Keine Bücher ausgewählt!"));
		return;
	}
	if (!a_eot)
		m_insertXAusleihe();
	else
		m_insertBTausch();
	QDialog::accept();
}

/*!
 * \brief Fügt die ISBNs in die btausch-Tabelle ein
 */
void InsertDialog::m_insertBTausch() {
	a_q.prepare("INSERT INTO `btausch` VALUES (:sid, :bid, :datum)");
	for (int i = 0; i < a_numIsbns; ++i) {
		if (a_alias[i]->text().isEmpty())
			continue;
		a_q.bindValue(":sid", a_id);
		a_q.bindValue(":bid", a_isbn[i]);
		a_q.bindValue(":datum", a_date->date().toString("yyyyMMdd"));
		if (a_q.exec())
			continue;
		else
			QMessageBox::warning(this, tr("Buch doppelt"),
					     tr("Das Buch %1 wurde doppelt eingescannt").arg(a_title[i]->text()));
	}
}

/*!
 * \brief Fügt die Bücher in lausleihe oder sausleihe ein
 */
void InsertDialog::m_insertXAusleihe() {
	QSqlQuery ins, sel, upd;
	sel.prepare(tr("SELECT * FROM `%1ausleihe` WHERE `%1id` = :lid AND `bid` = :bid")
		    .arg(a_student->isChecked() ? "s" : "l"));
	upd.prepare(tr("UPDATE `%1ausleihe` SET `anz` = `anz` + 1, `adatum` = :datum WHERE `%1id` = :lid AND `bid` = :bid")
		    .arg(a_student->isChecked() ? "s" : "l"));
	ins.prepare(tr("INSERT INTO `%1ausleihe` VALUES (:lid, :bid, :anz, :datum)").arg(a_student->isChecked() ? "s" : "l"));
	for (int i = 0; i < a_numIsbns; ++i) {
		if (a_alias[i]->text().isEmpty())
			continue;
		sel.bindValue(":lid", a_id);
		sel.bindValue(":bid", a_isbn[i]);
		if (!::exec(sel)) return;
		if (sel.size() > 0) {
			upd.bindValue(":lid", a_id);
			upd.bindValue(":bid", a_isbn[i]);
			upd.bindValue(":datum", a_date->date().toString("yyyyMMdd"));
			if (!::exec(upd)) return;
		} else {
			ins.bindValue(":lid", a_id);
			ins.bindValue(":bid", a_isbn[i]);
			ins.bindValue(":anz", 1);
			ins.bindValue(":datum", a_date->date().toString("yyyyMMdd"));
			if (!::exec(ins)) return;
		}
	}
}

/*!
 * \brief Konstruktor von InsertionDialog
 * \param tabIndex Gibt die Art des Datensatzes an (0 - Schüler, 1 - Lehrer, 2 - Buch)
 * \param parent Elternwidget
 * \see m_createComponents(), m_alignComponents(), m_setInitialValues(), m_connectComponents()
 *
 * Alle Aufgaben werden weiterdelegiert.
 */
InsertionDialog::InsertionDialog(int tabIndex, QWidget *parent) : QDialog(parent) {
	m_createComponents();
	m_alignComponents();
	tabChanged(tabIndex);
	m_setInitialValues();
	m_connectComponents();
}

/*!
 * \brief Erzeugt alle Komponenten
 */
void InsertionDialog::m_createComponents() {
	a_buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
					 Qt::Vertical);
	a_tabWidget = new QTabWidget;
	a_studentName = new QLineEdit;
	a_studentGradYear = new QLineEdit;
	a_studentFormLetter = new QLineEdit;
	a_teacherAbbrev = new QLineEdit;
	a_teacherName = new QLineEdit;
	a_bookIsbn = new QLineEdit;
	a_bookTitle = new QLineEdit;
	a_bookForm = new QLineEdit;
	a_aliasName = new QLineEdit;
	a_aliasIsbn = new QLineEdit;
}

/*!
 * \brief Ordnet alle Komponenten in Layouts an
 */
void InsertionDialog::m_alignComponents() {
	QGridLayout *a = new QGridLayout;
	a->addWidget(new QLabel(tr("Name:")), 0, 0);
	a->addWidget(a_studentName, 0, 1);
	a->addWidget(new QLabel(tr("Abschlussjahr:")), 1, 0);
	a->addWidget(a_studentGradYear, 1, 1);
	a->addWidget(new QLabel(tr("Klassenbuchstabe:")), 2, 0);
	a->addWidget(a_studentFormLetter, 2, 1);

	QGridLayout *b = new QGridLayout;
	b->addWidget(new QLabel(tr("Kürzel:")), 0, 0);
	b->addWidget(a_teacherAbbrev, 0, 1);
	b->addWidget(new QLabel(tr("Name:")), 1, 0);
	b->addWidget(a_teacherName, 1, 1);

	QGridLayout *c = new QGridLayout;
	c->addWidget(new QLabel(tr("ISBN:")), 0, 0);
	c->addWidget(a_bookIsbn, 0, 1);
	c->addWidget(new QLabel(tr("Titel:")), 1, 0);
	c->addWidget(a_bookTitle, 1, 1);
	c->addWidget(new QLabel(tr("Jahrgangsstufe:")), 2, 0);
	c->addWidget(a_bookForm, 2, 1);

	QGridLayout *d = new QGridLayout;
	d->addWidget(new QLabel(tr("Alias:")), 0, 0);
	d->addWidget(a_aliasName, 0, 1);
	d->addWidget(new QLabel(tr("ISBN:")), 1, 0);
	d->addWidget(a_aliasIsbn, 1, 1);

	QWidget *sWidget = new QWidget;
	QWidget *tWidget = new QWidget;
	QWidget *bWidget = new QWidget;
	QWidget *aWidget = new QWidget;

	sWidget->setLayout(a);
	tWidget->setLayout(b);
	bWidget->setLayout(c);
	aWidget->setLayout(d);

	a_tabWidget->addTab(aWidget, tr("Aliasse"));
	a_tabWidget->addTab(sWidget, tr("Schüler"));
	a_tabWidget->addTab(tWidget, tr("Lehrer"));
	a_tabWidget->addTab(bWidget, tr("Buch"));

	QHBoxLayout *layout = new QHBoxLayout;
	layout->addWidget(a_tabWidget);
	layout->addWidget(a_buttonBox);
	setLayout(layout);
}

/*!
 * \brief Initialisiert alle Komponenten
 */
void InsertionDialog::m_setInitialValues() {
	setWindowTitle(tr("Neuen Datensatz einfügen"));
	a_tabWidget->setCurrentIndex(a_tabIndex);
	a_tabWidget->currentWidget()->setFocus(Qt::MouseFocusReason);
	QRegExpValidator *gradYear = new QRegExpValidator(QRegExp("20[0-9]{2}"));
	QRegExpValidator *formLetter = new QRegExpValidator(QRegExp("[a-gA-G]"));

	a_studentGradYear->setValidator(gradYear);
	a_studentFormLetter->setValidator(formLetter);
}

/*!
 * \brief Verbindet die Komponenten mit den entsprechenden Slots
 */
void InsertionDialog::m_connectComponents() {
	connect(a_tabWidget, SIGNAL(currentChanged(int)), this, SLOT(tabChanged(int)));
	connect(a_buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
	connect(a_buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
}

/*!
 * \brief Wird aufgerufen, wenn ein anderer Tab angewählt wird
 * \param tabIndex Index des gewählten Tabs
 *
 * a_tabIndex wird gesetzt und abhängig vom Index dem jeweils ersten Feld
 * der Fokus verliehen.
 */
void InsertionDialog::tabChanged(int tabIndex) {
	a_tabIndex = tabIndex;
	switch (tabIndex) {
	case 0:
		setFocusProxy(a_aliasName);
		break;
	case 1:
		setFocusProxy(a_studentName);
		break;
	case 2:
		setFocusProxy(a_teacherAbbrev);
		break;
	case 3:
		setFocusProxy(a_bookIsbn);
		break;
	}
}

/*!
 * \brief Wird bei Klick auf Ok aufgerufen.
 *
 * Je nach Index werden in die verschiedenen Tabellen Daten eingefügt (SQL-Abfragen).
 */
void InsertionDialog::accept() {
	switch (a_tabIndex) {
	case 0:
		a_q.prepare("INSERT INTO `aliasse` VALUES (:alias, :isbn)");
		a_q.bindValue(":alias", a_aliasName->text().toLower());
		a_q.bindValue(":isbn", a_aliasIsbn->text());
		if (!::exec(a_q)) return;
		break;
	case 1:
		a_q.prepare("INSERT INTO `schueler` VALUES (NULL, :name, :vajahr, :kbuchst)");
		a_q.bindValue(":name", a_studentName->text());
		a_q.bindValue(":vajahr", a_studentGradYear->text());
		a_q.bindValue(":kbuchst", a_studentFormLetter->text());
		if (!::exec(a_q)) return;
		break;

	case 2:
		a_q.prepare("INSERT INTO `lehrer` VALUES (NULL, :name, :kuerzel)");
		a_q.bindValue(":name", a_teacherName->text());
		a_q.bindValue(":kuerzel", a_teacherAbbrev->text());
		if (!::exec(a_q)) return;
		break;

	case 3:
		a_q.prepare("INSERT INTO `buch` VALUES (:isbn, :titel, :jgst)");
		a_q.bindValue(":isbn", a_bookIsbn->text());
		a_q.bindValue(":titel", a_bookTitle->text());
		a_q.bindValue(":jgst", a_bookForm->text());
		if (!::exec(a_q)) return;
		break;
	}
	QDialog::accept();
}
