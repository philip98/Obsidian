#include "listview.h"
#include "main.h"
#include "insertdlg.h"
#include "deletedlg.h"
#include <QtGui>
#include <QtCore>
#include <QtSql>
#include <QtWidgets>

/*!
 * \brief Zeichnet ein Kontrollelement
 * \param element Art des Kontrollelements
 * \param option Zusätzliche Anzeigeoptionen
 * \param painter QPainter-Objekt, mit dem gezeichnet wird
 * \param widget Ggf. ein Zeiger auf das zu zeichnende Objekt
 *
 * Sofern es sich bei dem zu zeichnenden Objekt um ein HeaderLabel handelt und das "Opa-Widget" des zu zeichnenden
 * Widgets zur Klasse ListView gehört wird der Text um 270 Grad rotiert dargestellt. Sonst werden die Parameter an
 * die Default-Methode übergeben.
 */
void ListStyle::drawControl(ControlElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const {
	if (element == QStyle::CE_HeaderLabel && qobject_cast<ListView *>(widget->parent()->parent()) != 0) {
		const QHeaderView *hv = qobject_cast<const QHeaderView *>(widget);
		if (!hv || hv->orientation() == Qt::Vertical)
			return QProxyStyle::drawControl(element, option, painter, widget);
		const QStyleOptionHeader *header = qstyleoption_cast<const QStyleOptionHeader *>(option);
		painter->save();
		painter->translate(header->rect.center().x() - header->fontMetrics.height() / 2, header->rect.bottom());
		painter->rotate(270);
		painter->drawText(0, 0, header->text);
		painter->restore();
		return;
	}
	return QProxyStyle::drawControl(element, option, painter, widget);
}

/*!
 * \brief Konstruktor von ListModel
 * \param parent Elternobjekt
 *
 * Es werden ledeglich alle Listen geleert.
 */
ListModel::ListModel(QObject *parent) : QAbstractTableModel(parent) {
	a_students.clear();
	a_books.clear();
	a_lent.clear();
}

/*!
 * \brief Lädt die Headers (also Schüler- und Bücherliste)
 *
 * Erst werden alle Lehrbücher für die entsprechende Jahrgangsstufe abgefragt und in
 * a_books gespeichert. Anschließend werden alle Schüler abgefragt, die in der
 * Klasse sind und in a_students eingetragen. Schließlich wird headerChanged() emittiert.
 */
void ListModel::loadHeader() {
	Q_ASSERT(!a_form.isEmpty());
	Q_ASSERT(a_displayedForm != 0);

	a_books.clear();
	a_q.prepare("SELECT * FROM `Buch` WHERE `titel` LIKE :jgst ORDER BY `titel`");
	a_q.bindValue(":jgst", tr("%%%1%%").arg(a_displayedForm));
	if (!::exec(a_q))
		return;
	while (a_q.next())
		a_books[a_q.value("titel").toString()] = a_q.value("isbn").toString();

	a_students.clear();
	a_showed.clear();
	a_q.prepare("SELECT * FROM `SSchueler` WHERE `Klasse` = :klasse ORDER BY `Name`");
	a_q.bindValue(":klasse", a_form);
	if (!::exec(a_q))
		return;
	while (a_q.next()) {
		a_students[a_q.value("Name").toString()] = a_q.value("id").toInt();
		a_showed.append(a_q.value("id").toInt());
	}

	a_books["sonstige"] = tr("Sonstige Ausleihen");

	emit headerDataChanged(Qt::Horizontal, 0, a_books.count());
	emit headerDataChanged(Qt::Vertical, 0, a_students.count());
}

/*!
 * \brief Gibt die Daten im jeweiligen Feld an
 * \param index Zeigt auf das Feld dessen Daten abgefragt werden
 * \param role Gibt an, was abgefragt wird
 * \return Qt::Checked - Ausgegeben, Qt::Unchecked - Nicht ausgegeben
 *
 * Mithilfe von m_isSet wird überprüft, ob das Buch dem Schüler gegeben wurde.
 */
QVariant ListModel::data(const QModelIndex &index, int role) const {
	if (role != Qt::CheckStateRole)
		return QVariant();
	return (a_lent.value(a_showed[index.row()]).value(
				a_books.values().at(index.column()))) ?
				Qt::Checked : Qt::Unchecked;
}

/*!
 * \brief Gibt die Daten der jeweiligen Zeilen-/Spaltenüberschrift an
 * \param section Zeile/Spalte
 * \param orientation Zeilen- oder Spaltenüberschrift?
 * \param role Gibt an, was abgefragt wird
 * \return Beschriftung der Zeile/Spalte
 *
 * Je nachdem, ob der horizontale oder der vertikale Header erfragt wird, wird entweder
 * a_books oder a_students zu Rate gezogen.
 */
QVariant ListModel::headerData(int section, Qt::Orientation orientation, int role) const {
	if (role == Qt::DisplayRole) {
		if (orientation == Qt::Horizontal)
			return a_books.keys().at(section);
		else
			return a_students.key(a_showed[section]);
	} else if (role == Qt::SizeHintRole && orientation == Qt::Horizontal)
		return QSize(qApp->fontMetrics().height(), qMin(qApp->fontMetrics().width(a_books.keys().at(section)) + 10, 200));
	return QVariant();
}

/*!
 * \brief Gibt die Zeilenanzahl an
 * \return Anzahl der Zeilen(Schüler)
 */
int ListModel::rowCount(const QModelIndex &) const {
	return a_students.count();
}

/*!
 * \brief Gibt die Spaltenanzahl an
 * \return Anzahl der Spalten(Bücher)
 */
int ListModel::columnCount(const QModelIndex &) const {
	return a_books.count();
}

/*!
 * \brief Lädt die Daten
 *
 * Für jeden Schüler wird aus `BTausch` eine SELECT-Abfrage gestartet, und danach zu jedem Buch
 * in a_lent das entsprechende Bit gesetzt (mithilfe vom m_set()). Anschließend wird dataChanged() emittiert.
 */
void ListModel::loadData() {
	QSqlQuery other;

	a_lent.clear();
	a_q.prepare("SELECT * FROM `btausch` WHERE `sid` = :sid");
	other.prepare("SELECT * FROM sausleihe WHERE sid = :sid");
	foreach (int sid, a_students.values()) {
		a_q.bindValue(":sid", sid);
		other.bindValue(":sid", sid);
		a_lent.insert(sid, {});
		if (!(::exec(a_q) && ::exec(other)))
			return;
		while (a_q.next()) {
			if (a_books.values().contains(a_q.value("bid").toString()))
				a_lent[sid].insert(a_q.value("bid").toString(), true);
		}
		a_lent[sid].insert("sonstige", other.size() > 0);
	}

	emit dataChanged(createIndex(0, 0), createIndex(a_students.count(), a_books.count()));
}

/*!
 * \brief Verändert die anzuzeigende Klasse
 * \param form Neue Klasse
 *
 * Setzt a_form und ruft anschließend loadHeader() und loadData() auf.
 */
void ListModel::setForm(QString form) {
	a_form = form;
}

/*!
 * \brief Setzt die Jahrgangsstufe, deren Bücher anzuzeigen sind
 * \param form Neue Jahrgangsstufe
 *
 * Die a_displayedForm wird gesetzt und anschließlich loadHeader() und loadData() aufgerufen.
 */
void ListModel::setDisplayedForm(int form) {
	a_displayedForm = form;
}

/*!
 * \brief Wird aufgerufen, wenn eine Taste gedrückt wird
 * \param event
 *
 * Der gesamte Vertikale Header
 */
void ListTable::keyPressEvent(QKeyEvent *event) {
	QTableView::keyPressEvent(event);
	if (!event->text().isEmpty() && event->text().size() == 1) {
		QStringList str = {};
		for (int i = 0; i < model()->rowCount(); ++i)
			str << model()->headerData(i, Qt::Vertical).toString();
		QRegularExpression regex = QRegularExpression(tr("^%1.*$").arg(event->text()[0].toUpper()));
		selectRow(str.indexOf(regex));
	}
}

/*!
 * \brief Konstruktor von ListView
 * \param parent Elternwidget
 * \see m_createComponents(), m_alignComponents(), m_setInitialValues(), m_connectComponents()
 */
ListView::ListView(QWidget *parent) : QWidget(parent) {
	m_createComponents();
	m_alignComponents();
	m_setInitialValues();
	m_connectComponents();
}

/*!
 * \brief Erzeugt die Komponenten
 */
void ListView::m_createComponents() {
	a_tableView = new ListTable;
	a_tableModel = new ListModel;
	a_form = new QComboBox;
	a_newList = new QRadioButton(tr("Neue Liste"));
	a_oldList = new QRadioButton(tr("Alte Liste"));
}

/*!
 * \brief Ordnet die Komponenten in Layouts an
 */
void ListView::m_alignComponents() {
	QHBoxLayout *a = new QHBoxLayout;
	a->addWidget(a_form);
	a->addWidget(a_newList);
	a->addWidget(a_oldList);
	a->addSpacerItem(new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Fixed));

	QVBoxLayout *layout = new QVBoxLayout;
	layout->addLayout(a);
	layout->addWidget(a_tableView);

	setLayout(layout);
}

/*!
 * \brief Initialisiert die Komponenten
 *
 * Erst werden bei a_tableView ein paar Einstellungen getan, dann entsprechend
 * des aktuellen Monats a_newList oder a_oldList angewählt. Im Anschluss werden
 * die StatusTips gesetzt.
 */
void ListView::m_setInitialValues() {
	a_form->setInsertPolicy(QComboBox::NoInsert);

	a_tableView->setStyle(new ListStyle(a_tableView->style()));
	a_tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
	a_tableView->setSelectionMode(QAbstractItemView::SingleSelection);

	if (QDate::currentDate().month() >= 9)
		a_newList->setChecked(true);
	else
		a_oldList->setChecked(true);

	a_tableView->setStatusTip(tr("Zeigt an, ob ein Schüler ein Buch hat"));
	a_form->setStatusTip(tr("Klasse, die angezeigt wird"));
	a_newList->setStatusTip(tr("Zeigt die neue Liste an"));
	a_oldList->setStatusTip(tr("Zeigt die alte Liste an"));
	a_form->setCurrentIndex(0);

	m_populateCombo();
	changeForm(a_form->currentText());
	toggle();
	a_tableView->resizeColumnsToContents();
	a_tableView->setModel(a_tableModel);
}

/*!
 * \brief Verbindet die Komponenten mit den Slots
 */
void ListView::m_connectComponents() {
	connect(a_form, SIGNAL(currentTextChanged(QString)), this, SLOT(changeForm(QString)));
	connect(a_oldList, SIGNAL(toggled(bool)), this, SLOT(toggle()));
}

/*!
 * \brief Lädt die Klassen neu (ComboBox)
 *
 * Es wird der Index gespeichert, anschließend eine SELECT-Abfrage gestartet, um die Klassen zu
 * erfragend, alle Klassen hinzugefügt und schließlich der Index wieder gesetzt.
 */
void ListView::m_populateCombo() {
	a_form->clear();
	if (!a_q.exec("SELECT DISTINCT `Klasse` FROM `SSchueler` ORDER BY `Klasse`;")) return;
	if (a_q.first()) {
		do {
			a_form->addItem(a_q.record().value("Klasse").toString());
		} while (a_q.next());
	}
}

int ListView::m_calculateForm(QString form) {
	int result = 0;
	foreach (QChar c, form) {
		if (c.isDigit()) {
			result *= 10;
			result += QString(c).toInt();
		} else
			break;
	}
	return result;
}

/*!
 * \brief Lädt alles neu
 *
 * Zuerst werden die Klassen, dann die Tabellenheader und -daten neu geladen. Schließlich wird a_tableView angewiesen,
 * die Spalten nach Inhalt zu vergrößern/-kleinern.
 */
void ListView::refresh() {
	a_tableModel->loadHeader();
	a_tableModel->loadData();
	a_tableView->resizeColumnsToContents();
}

/*!
 * \brief Wird aufgerufen, wenn die Klasse gewechselt wird
 * \param form Neue Klasse
 *
 * Verändert die Klasse des Datenmodells und wählt dann je nach aktuellem Monat aus, welche Liste angezeigt werden soll.
 */
void ListView::changeForm(QString form) {
	a_tableModel->setForm(form);
	if (QDate::currentDate().month() >= 9)
		a_newList->setChecked(true);
	else
		a_oldList->setChecked(true);
	toggle();
}

/*!
 * \brief Verbucht eine Bücherausleihe
 *
 * Erzeugt einen neuen InsertDialog im Schuljahresendmodus, führt ihn aus, löscht ihn
 * und lädt anschließend alles neu. Dabei wird dem Dialog ein Schüler-Datensatz übergeben, wenn ein Schüler ausgewählt war.
 */
void ListView::lendBook() {
	QSqlRecord r;
	if (!a_tableView->selectionModel()->selectedRows().isEmpty()) {
		int index = a_tableView->selectionModel()->selectedRows()[0].row();

		a_q.prepare("SELECT * FROM `SSchueler` WHERE `Klasse`=:klasse AND `Name`=:name");
		a_q.bindValue(":klasse", a_form->currentText());
		a_q.bindValue(":name", a_tableModel->headerData(index, Qt::Vertical, Qt::DisplayRole));
		if (!::exec_first(&a_q)) return;
		r = a_q.record();
	}
	InsertDialog *dl = new InsertDialog(true, this, r);
	dl->setFocus();
	dl->exec();
	delete dl;
	refresh();
}

/*!
 * \brief Verbucht eine Bücherrückgabe
 *
 * Erzeugt einen neuen DeleteDialog im Schuljahresendmodus, führt ihn aus, löscht ihn wieder
 * und lädt anschließend alles neu. Dabei wird dem Dialog ein Schüler-Datensatz übergeben, wenn ein Schüler ausgewählt war.
 */
void ListView::withdrawBook() {
	QSqlRecord r;
	if (!a_tableView->selectionModel()->selectedRows().isEmpty()) {
		int index = a_tableView->selectionModel()->selectedRows()[0].row();

		a_q.prepare("SELECT * FROM `SSchueler` WHERE `Klasse` = :klasse AND `Name` = :name");
		a_q.bindValue(":klasse", a_form->currentText());
		a_q.bindValue(":name", a_tableModel->headerData(index, Qt::Vertical, Qt::DisplayRole).toString());
		if (!::exec_first(&a_q)) return;
		r = a_q.record();
	}
	DeleteDialog *dl = new DeleteDialog(true, this, r);
	dl->setFocus();
	dl->exec();
	delete dl;
	refresh();
}

/*!
 * \brief Wird bei Klick auf a_oldList und a_newList aufgerufen
 *
 * Zuerst wird aus der Klassenbezeichnung die jeweiligen Jahrgangsstufen der alten und neuen
 * Liste ausgerechnet (Im neuen Jahr ist die derzeitige Liste alt, im alten Jahr neu).
 */
void ListView::toggle() {
	int oldForm, newForm;
	if (QDate::currentDate().month() >= 9) {
		newForm = m_calculateForm(a_form->currentText());
		oldForm = newForm - 1;
	} else {
		oldForm = m_calculateForm(a_form->currentText());
		newForm = oldForm + 1;
	}
	if (a_newList->isChecked()) {
		a_tableModel->setDisplayedForm(newForm);
	} else {
		a_tableModel->setDisplayedForm(oldForm);
	}
	refresh();
}

/*!
 * \brief Wird bei Klick auf Bearbeiten|Exportieren aufgerufen
 */
void ListView::exportLendings() {
	QString fname = QFileDialog::getSaveFileName(this, tr("Ausleihen exportieren"),
						     QDir::homePath(), "Textdateien (*.txt)");
	if (fname.isEmpty())
		return;
	QFile file{fname};
	QSqlRecord record;
	QTextStream stream{&file};

	if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
		return;
	stream.setCodec("UTF-8");

	stream.setFieldAlignment(QTextStream::AlignLeft);
	stream << "Sonstige ausstehende Ausleihen (" << a_tableModel->form() << ")\n\n";
	a_q.prepare("SELECT SSchueler.name AS name, Buch.titel AS titel FROM (sausleihe LEFT JOIN SSchueler ON SSchueler.id = sausleihe.sid) LEFT JOIN Buch ON sausleihe.bid = Buch.isbn WHERE `Klasse` = :klasse ORDER BY SSchueler.Name");
	a_q.bindValue(":klasse", a_tableModel->form());
	if (!::exec_first(&a_q)) return;
	do {
		record = a_q.record();
		stream << qSetFieldWidth(30) << qSetPadChar('.') << record.value("name").toString() << qSetFieldWidth(40)
		       << qSetPadChar(' ') << record.value("titel").toString() << endl;
	} while (a_q.next());

	stream.reset();
	stream.setFieldAlignment(QTextStream::AlignLeft);
	stream << "\n\n\nNicht zurückgegebene Bücher (" << a_tableModel->form() << ")\n\n";
	a_q.prepare("SELECT SSchueler.name AS name, Buch.titel AS titel FROM (btausch LEFT JOIN SSchueler ON SSchueler.id = btausch.sid) LEFT JOIN Buch ON btausch.bid = Buch.isbn WHERE SSchueler.Klasse = :klasse ORDER BY SSchueler.Name");
	a_q.bindValue(":klasse", a_tableModel->form());
	if(!::exec_first(&a_q)) return;
	do {
		record = a_q.record();
		stream << qSetFieldWidth(30) << qSetPadChar('.') << record.value("name").toString() << qSetFieldWidth(40)
		       << qSetPadChar(' ') << record.value("titel").toString() << endl;
	} while (a_q.next());
}
