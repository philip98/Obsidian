#include "selectdlg.h"
#include "main.h"
#include <QtCore>
#include <QtSql>
#include <QtWidgets>
#include <QtGui>

/*!
 * \brief Erstellt einen neuen SelectDialog
 * \param isStudent Gibt an, ob der Auszuwählende ein Schüler ist
 * \param parent Elternwidget
 * \see m_createComponents(), m_alignComponents(), m_setInitialValues(), m_connectComponents()
 */
SelectDialog::SelectDialog(bool isStudent, QWidget *parent) :
	QDialog(parent), a_isStudent(isStudent) {
	m_createComponents();
	m_alignComponents();
	m_setInitialValues();
	m_connectComponents();
}

/*!
 * \brief Erstellt die Komponenten
 */
void SelectDialog::m_createComponents() {
	a_tableView = new QTableView;
	a_search = new QPushButton(tr("Suchen"));
	a_nameFragment = new QLineEdit("");
	a_buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
					   Qt::Vertical);
	a_tableModel = new QSqlTableModel;
}

/*!
 * \brief Ordnet die Komponenten in Layouts an
 */
void SelectDialog::m_alignComponents() {
	QHBoxLayout *a = new QHBoxLayout;
	a->addWidget(a_nameFragment);
	a->addWidget(a_search);

	QVBoxLayout *b = new QVBoxLayout;
	b->addLayout(a);
	b->addWidget(a_tableView);

	QHBoxLayout *layout = new QHBoxLayout;
	layout->addLayout(b);
	layout->addWidget(a_buttonBox);
	setLayout(layout);
}

/*!
 * \brief Initialisiert die Komponenten
 *
 * Je nach Art der Person wird die Abfrage der Tabelle und der Titel des
 * Fensters gesetzt. Danach werden allgemeine Einstellungen an der Tabelle
 * getätigt
 */
void SelectDialog::m_setInitialValues() {
	if (a_isStudent) {
		a_tableModel->setQuery("SELECT * FROM `SSchueler`");
		setWindowTitle(tr("Schüler auswählen"));
	} else {
		a_tableModel->setQuery("SELECT * FROM `Lehrer`");
		setWindowTitle(tr("Lehrer auswählen"));
	}

	a_tableView->setAutoScroll(true);
	a_tableView->setModel(a_tableModel);
	a_tableView->setSelectionMode(QAbstractItemView::SingleSelection);
	a_tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
	a_tableView->resizeColumnsToContents();
	a_search->setDefault(true);
	a_tableView->verticalHeader()->hide();
}

/*!
 * \brief Verbindet die Komponenten mit den Slots
 */
void SelectDialog::m_connectComponents() {
	connect(a_search, SIGNAL(clicked()), this, SLOT(searchName()));
	connect(a_buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
	connect(a_buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
	connect(a_tableView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(accept()));
}

/*!
 * \brief Wird bei Klick auf a_search aufgerufen
 *
 * Die Abfrage von a_tableModel wird so verändert, dass nur noch Schüler/Lehrer, deren Name
 * das Namensfragment enthält, angezeigt werden.
 */
void SelectDialog::searchName() {
	if (a_isStudent) {
		a_tableModel->setQuery(QString("SELECT * FROM `SSchueler` WHERE `Name` LIKE '\%%1\%';")
					 .arg(escape(a_nameFragment->text())));
	} else {
		a_tableModel->setQuery(QString("SELECT * FROM `Lehrer` WHERE `Name` LIKE '\%%1\%';")
					 .arg(escape(a_nameFragment->text())));
	}
}

/*!
 * \brief Gibt den ausgewählten Datensatz zurück
 * \return Ausgewählter Datensatz
 *
 * Der Datensatz kommt aus dem View `SSchueler` und hat daher folgende Werte:
 * <ul>
 *	<li>id : Nummer des Schülers</li>
 *	<li>Klasse : Klasse des Schülers</li>
 *	<li>Name : Name des Schülers</li>
 * </ul>
 *
 * Wenn ein Lehrer auszuwählen war, dann handelt es sich um einen Datensatz des `Lehrer`-Views mit folgenden Werten:
 * <ul>
 *	<li>id : Laufende Nummer des Lehrers</li>
 *	<li>Kürzel : Kürzel des Lehrers</li>
 *	<li>Name : Name des Lehrers (wer hätte das gedacht?)</li>
 * </ul>
 */
QSqlRecord SelectDialog::getRecord() {
	if (a_tableView->selectionModel()->selectedRows().isEmpty())
		return QSqlRecord();
	return a_tableModel->record(a_tableView->selectionModel()->selectedRows()[0].row());
}

/*!
 * \brief Konstruktor von FindDialog
 * \param tabIndex Verweist auf zu durchsuchende Tabelle
 * \param parent Elternwidget
 * \see m_createComponents(), m_alignComponents(), m_setInitialValues(), m_connectComponents()
 */
FindDialog::FindDialog(int tabIndex, QWidget *parent) : QDialog(parent) {
	m_createComponents();
	m_alignComponents();
	indexChanged(tabIndex);
	m_setInitialValues();
	m_connectComponents();
}

/*!
 * \brief Erzeugt alle Komponenten
 */
void FindDialog::m_createComponents() {
	a_buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Vertical);
	a_tabWidget = new QTabWidget;

	a_studentId = new QLineEdit;
	a_studentName = new QLineEdit;
	a_studentGradYear = new QLineEdit;
	a_studentFormLetter = new QLineEdit;

	a_teacherId = new QLineEdit;
	a_teacherAbbrev = new QLineEdit;
	a_teacherName = new QLineEdit;

	a_bookIsbn = new QLineEdit;
	a_bookTitle = new QLineEdit;
	a_bookForm = new QLineEdit;

	a_sLendForm = new QLineEdit;
	a_sLendName = new QLineEdit;
	a_sLendTitle = new QLineEdit;
	a_sLendDate = new QDateEdit;

	a_tLendAbbrev = new QLineEdit;
	a_tLendName = new QLineEdit;
	a_tLendTitle = new QLineEdit;
	a_tLendDate = new QDateEdit;

	a_swapForm = new QLineEdit;
	a_swapName = new QLineEdit;
	a_swapTitle = new QLineEdit;
	a_swapDate = new QDateEdit;
	a_swapYear = new QLineEdit;
}

/*!
 * \brief Ordnet die Komponenten in Layouts an
 */
void FindDialog::m_alignComponents() {
	QGridLayout *d = new QGridLayout;
	d->addWidget(new QLabel(tr("ID:")), 0, 0);
	d->addWidget(a_studentId, 0, 1);
	d->addWidget(new QLabel(tr("Name:")), 1, 0);
	d->addWidget(a_studentName, 1, 1);
	d->addWidget(new QLabel(tr("Abschlussjahr:")), 2, 0);
	d->addWidget(a_studentGradYear, 2, 1);
	d->addWidget(new QLabel(tr("Klassenbuchstabe:")), 3, 0);
	d->addWidget(a_studentFormLetter, 3, 1);

	QGridLayout *e = new QGridLayout;
	e->addWidget(new QLabel(tr("ID:")), 0, 0);
	e->addWidget(a_teacherId, 0, 1);
	e->addWidget(new QLabel(tr("Kürzel:")), 1, 0);
	e->addWidget(a_teacherAbbrev, 1, 1);
	e->addWidget(new QLabel(tr("Name:")), 2, 0);
	e->addWidget(a_teacherName, 2, 1);

	QGridLayout *f = new QGridLayout;
	f->addWidget(new QLabel("ISBN:"), 0, 0);
	f->addWidget(a_bookIsbn, 0, 1);
	f->addWidget(new QLabel("Titel:"), 1, 0);
	f->addWidget(a_bookTitle, 1, 1);
	f->addWidget(new QLabel("Jahrgangsstufe"), 2, 0);
	f->addWidget(a_bookForm, 2, 1);

	QGridLayout *a = new QGridLayout;
	a->addWidget(new QLabel(tr("Klasse:")), 0, 0);
	a->addWidget(a_sLendForm, 0, 1);
	a->addWidget(new QLabel(tr("Name:")), 1, 0);
	a->addWidget(a_sLendName, 1, 1);
	a->addWidget(new QLabel(tr("Buch:")), 2, 0);
	a->addWidget(a_sLendTitle, 2, 1);
	a->addWidget(new QLabel(tr("Ausleihdatum:")), 4, 0);
	a->addWidget(a_sLendDate, 4, 1);

	QGridLayout *b = new QGridLayout;
	b->addWidget(new QLabel(tr("Kürzel:")), 0, 0);
	b->addWidget(a_tLendAbbrev, 0, 1);
	b->addWidget(new QLabel(tr("Name:")), 1, 0);
	b->addWidget(a_tLendName, 1, 1);
	b->addWidget(new QLabel(tr("Buch:")), 2, 0);
	b->addWidget(a_tLendTitle, 2, 1);
	b->addWidget(new QLabel(tr("Ausleihdatum:")), 4, 0);
	b->addWidget(a_tLendDate, 4, 1);

	QGridLayout *c = new QGridLayout;
	c->addWidget(new QLabel(tr("Klasse:")), 0, 0);
	c->addWidget(a_swapForm, 0, 1);
	c->addWidget(new QLabel(tr("Name:")), 1, 0);
	c->addWidget(a_swapName, 1, 1);
	c->addWidget(new QLabel(tr("Buch:")), 2, 0);
	c->addWidget(a_swapTitle, 2, 1);
	c->addWidget(new QLabel(tr("Ausgabedatum:")), 3, 0);
	c->addWidget(a_swapDate, 3, 1);
	c->addWidget(new QLabel(tr("Ausgabejahr:")), 4, 0);
	c->addWidget(a_swapYear, 4, 1);

	QWidget *aWidget = new QWidget;
	QWidget *bWidget = new QWidget;
	QWidget *cWidget = new QWidget;
	QWidget *dWidget = new QWidget;
	QWidget *eWidget = new QWidget;
	QWidget *fWidget = new QWidget;

	aWidget->setLayout(a);
	bWidget->setLayout(b);
	cWidget->setLayout(c);
	dWidget->setLayout(d);
	eWidget->setLayout(e);
	fWidget->setLayout(f);

	a_tabWidget->addTab(aWidget, tr("Schülerausleihen"));
	a_tabWidget->addTab(bWidget, tr("Lehrerausleihen"));
	a_tabWidget->addTab(cWidget, tr("Büchertausch"));
	a_tabWidget->addTab(dWidget, tr("Schüler"));
	a_tabWidget->addTab(eWidget, tr("Lehrer"));
	a_tabWidget->addTab(fWidget, tr("Bücher"));
	a_tabWidget->setCurrentIndex(a_tabIndex);

	QHBoxLayout *layout = new QHBoxLayout;
	layout->addWidget(a_tabWidget);
	layout->addWidget(a_buttonBox);
	setLayout(layout);
}

/*!
 * \brief Setzt die Anfangswerte
 *
 * Setzt den Titel des Fensters und anschließend die RegEx-Validatoren
 */
void FindDialog::m_setInitialValues() {
	setWindowTitle("Suchen");
	a_tabWidget->setTabPosition(QTabWidget::North);
	a_tabWidget->setCurrentIndex(a_tabIndex);

	QRegExpValidator *form = new QRegExpValidator(QRegExp("[5-9][a-gA-G]|10[a-gA-G]|1[12]"));
	QRegExpValidator *number = new QRegExpValidator(QRegExp("[1-9][0-9]*"));
	QRegExpValidator *formLetter = new QRegExpValidator(QRegExp("[a-gA-G]"));

	a_studentId->setValidator(number);
	a_teacherId->setValidator(number);
	a_bookIsbn->setValidator(number);
	a_studentGradYear->setValidator(number);
	a_swapYear->setValidator(number);
	a_swapForm->setValidator(form);
	a_studentFormLetter->setValidator(formLetter);
	a_sLendForm->setValidator(form);
}

/*!
 * \brief Verbindet die Komponenten mit den Slots
 */
void FindDialog::m_connectComponents() {
	connect(a_tabWidget, SIGNAL(currentChanged(int)), this, SLOT(indexChanged(int)));
	connect(a_sLendDate, SIGNAL(dateChanged(QDate)), this, SLOT(dateChanged()));
	connect(a_tLendDate, SIGNAL(dateChanged(QDate)), this, SLOT(dateChanged()));
	connect(a_swapDate, SIGNAL(dateChanged(QDate)), this, SLOT(dateChanged()));
	connect(a_buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
	connect(a_buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
}

/*!
 * \brief Eruiert aus den gegebenen Eingabefeldern die SQL-Abfrage
 * \return Erzeugte SQL-Abfrage oder ""
 *
 * Je nach aktuell angewähltem Tab werden nach ausgeklügeltem System die SQL-Abfragen erzeugt.
 */
QString FindDialog::getQuery() {
	QString query;
	bool added = false;

	switch (a_tabIndex) {
	case 0:
		query = tr("SELECT * FROM `SAusleihen` WHERE");
		if (a_sLendForm->text() != QString()) {
			query += tr(" `Klasse` = '%1'").arg(escape(a_sLendForm->text().toLower()).simplified());
			added = true;
		}
		if (a_sLendName->text() != QString()) {
			if (added)
				query += tr(" AND");
			query += tr(" `Name` LIKE '\%%1\%'").arg(escape(a_sLendName->text()).simplified());
			added = true;
		}
		if (a_sLendTitle->text() != QString()) {
			if (added)
				query += tr(" AND");
			query += tr(" `Buch` LIKE '\%%1\%'").arg(escape(a_sLendTitle->text()).simplified());
			added = true;
		}
		if (a_slDateChanged) {
			if (added)
				query += tr(" AND");
			query += tr(" `Datum` = %1").arg(a_sLendDate->date().toString("yyyyMMdd"));
			added = true;
		}
		break;

	case 1:
		query = tr("SELECT * FROM `LAusleihen` WHERE");
		if (a_tLendAbbrev->text() != QString()) {
			query += tr(" `Kürzel` = '%1'").arg(escape(a_tLendAbbrev->text().toLower()).simplified());
			added = true;
		}
		if (a_tLendName->text() != QString()) {
			if (added)
				query += tr(" AND");
			query += tr(" `Name` LIKE '\%%1\%'").arg(escape(a_tLendName->text()).simplified());
			added = true;
		}
		if (a_tLendTitle->text() != QString()) {
			if (added)
				query += tr(" AND");
			query  += tr(" `Buch` LIKE '\%%1\%'").arg(escape(a_tLendTitle->text()).simplified());
			added = true;
		}
		if (a_tlDateChanged) {
			if (added)
				query += tr(" AND");
			query += tr(" `Datum` = %1").arg(a_tLendDate->date().toString("yyyyMMdd"));
			added = true;
		}
		break;

	case 2:
		query = tr("SELECT * FROM `BTausch` WHERE");
		if (a_swapForm->text() != QString()) {
			query += tr(" `Klasse` = '%1'").arg(escape(a_swapForm->text().toLower()).simplified());
			added = true;
		}
		if (a_swapName->text() != QString()) {
			if (added)
				query += tr(" AND");
			query += tr(" `Name` LIKE '\%%1\%'").arg(escape(a_swapName->text()).simplified());
			added = true;
		}
		if (a_swapTitle->text() != QString()) {
			if (added)
				query += tr(" AND");
			query += tr(" `Buch` LIKE '\%%1\%'").arg(escape(a_swapTitle->text()).simplified());
			added = true;
		}
		if (a_sDateChanged) {
			if (added)
				query += tr(" AND");
			query += tr(" `Datum` = %1").arg(a_swapDate->date().toString("yyyyMMdd"));
			added = true;
		}
		if (a_swapYear->text() != QString()) {
			if (added)
				query += tr(" AND");
			query += tr(" YEAR(`Datum`) = %1").arg(escape(a_swapYear->text()).simplified());
			added = true;
		}
		break;

	case 3:
		query = tr("SELECT * FROM `Schueler` WHERE");
		if (a_studentId->text() != QString()) {
			query += tr(" `id` = %1").arg(escape(a_studentId->text()).simplified());
			added = true;
		}
		if (a_studentName->text() != QString()) {
			if (added)
				query += tr(" AND");
			query += tr(" `Name` LIKE '\%%1\%'").arg(escape(a_studentName->text()).simplified());
			added = true;
		}
		if (a_studentGradYear->text() != QString()) {
			if (added)
				query += tr(" AND");
			query += tr(" `Voraussichtl. Abschlussjahr` = %1").arg(escape(a_studentGradYear->text()).simplified());
			added = true;
		}
		if (a_studentFormLetter->text() != QString()) {
			if (added)
				query += tr(" AND");
			query += tr(" `Klassenbuchstabe` = '%1'").arg(escape(a_studentFormLetter->text().toLower()).simplified());
			added = true;
		}
		break;

	case 4:
		query = tr("SELECT * FROM `Lehrer` WHERE");
		if (a_teacherId->text() != QString()) {
			query += tr(" `id` = %1").arg(escape(a_teacherId->text()).simplified());
			added = true;
		}
		if (a_teacherAbbrev->text() != QString()) {
			if (added)
				query += tr(" AND");
			query += tr(" `Kürzel` LIKE '\%%1\%'").arg(escape(a_teacherAbbrev->text().toLower()).simplified());
			added = true;
		}
		if (a_teacherName->text() != QString()) {
			if (added)
				query += tr(" AND");
			query += tr(" `Name` LIKE '\%%1\%'").arg(escape(a_teacherName->text()).simplified());
			added = true;
		}
		break;

	case 5:
		query = tr("SELECT * FROM `Buecher` WHERE");
		if (a_bookIsbn->text() != QString()) {
			query += tr(" `ISBN` = %1").arg(escape(a_bookIsbn->text()).simplified());
			added = true;
		}
		if (a_bookTitle->text() != QString()) {
			if (added)
				query += tr(" AND");
			query += tr(" `Titel` LIKE '\%%1\%'").arg(escape(a_bookTitle->text()).simplified());
			added = true;
		}
		if (a_bookForm->text() != QString()) {
			if (added)
				query += tr(" AND");
			query += tr(" `Jahrgangsstufe` LIKE '%%%1%%'").arg(escape(a_bookForm->text()).simplified());
		}
		break;
	}
	if (!added)
		return QString();
	return query;
}

/*!
 * \brief Wird aufgerufen, wenn der Tab gewechselt wird.
 * \param tabIndex Neuer Index
 */
void FindDialog::indexChanged(int tabIndex) {
	a_tabIndex = tabIndex;
	switch (tabIndex) {
	case 0:
		a_sLendForm->setFocusPolicy(Qt::StrongFocus);
		a_sLendForm->setFocus();
		break;
	case 1:
		a_tLendAbbrev->setFocusPolicy(Qt::StrongFocus);
		a_tLendAbbrev->setFocus();
		break;
	case 2:
		a_swapForm->setFocusPolicy(Qt::StrongFocus);
		a_swapForm->setFocus();
		break;
	case 3:
		a_studentName->setFocusPolicy(Qt::StrongFocus);
		a_studentName->setFocus();
		break;
	case 4:
		a_teacherAbbrev->setFocusPolicy(Qt::StrongFocus);
		a_teacherAbbrev->setFocus();
		break;
	case 5:
		a_bookIsbn->setFocusPolicy(Qt::StrongFocus);
		a_bookIsbn->setFocus();
		break;
	}
}

void FindDialog::dateChanged() {
	if (sender() == a_sLendDate)
		a_slDateChanged = true;
	else if (sender() == a_tLendDate)
		a_tlDateChanged = true;
	else if (sender() == a_swapDate)
		a_sDateChanged = true;
}

/*!
 * \brief Gibt den Index
 * \return Index des aktuell angewählten Tabs
 */
int FindDialog::getIndex() {
	return a_tabIndex;
}
