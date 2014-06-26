#include "tableview.h"
#include "insertdlg.h"
#include "deletedlg.h"
#include "selectdlg.h"
#include "settingsdlg.h"
#include "importdlg.h"
#include <QtCore>
#include <QtGui>
#include <QtSql>
#include <QtWidgets>

/*!
 * \brief Konstruktor von TableModel
 * \param table Name der anzuzeigenden Tabelle
 * \param parent Elternobjekt
 */
TableModel::TableModel(QString table, QObject *parent) : QSqlQueryModel(parent) {
	a_tableName = table;
}

/*!
 * \brief Gibt Eigenschaften der Felder aus
 * \param index Verweist auf das Feld
 * \return Anzeigeoptionen für das Feld
 *
 * Wenn es sich bei der anzuzeigenden Tabelle um `buch`, `lehrer` oder `schueler` handelt und nicht gerade
 * das Feld, das den Primärschlüssel enthält gemeint ist, wird Qt::ItemIsEditable | Qt::ItemIsEnabled |
 * Qt::ItemIsSelectable zurückgegeben. Sonst nur Qt::ItemIsSelectable | Qt::ItemIsEnabled. Das bedeutet,
 * dass man die nicht Primärschlüssel-Felder der Tabellen `buch`, `schueler` und `lehrer` bearbeiten kann.
 */
Qt::ItemFlags TableModel::flags(const QModelIndex &index) const {
	if (a_tableName == tr("buch") || a_tableName == tr("schueler") || a_tableName == tr("lehrer")) {
		QString heading = headerData(index.column(), Qt::Horizontal, Qt::DisplayRole).toString();
		if (heading == tr("ISBN") || heading == tr("id"))
			return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
		else
			return Qt::ItemIsEditable | Qt::ItemIsSelectable | Qt::ItemIsEnabled;
	}
	return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}

/*!
 * \brief Setzt die Felddaten
 * \param index Verweist auf das Feld
 * \param value Neuer Wert
 * \param role Art der Daten (normalerweise Qt::EditRole)
 * \return true - erfolgreich, false - ein Fehler ist aufgetreten
 *
 * Es wird entsprechend der Daten eine Abfrage generiert und ausgeführt.
 */
bool TableModel::setData(const QModelIndex &index, const QVariant &value, int role) {
	if (role != Qt::EditRole)
		return false;
	if (m_getFieldName(index.column()) == tr("isbn") ||
			m_getFieldName(index.column()) == tr("id"))
		return false;
	QSqlQuery q;
	q.prepare(tr("UPDATE %1 SET %2 = :value WHERE %3 = :ind").arg(a_tableName)
		  .arg(m_getFieldName(index.column())).arg(m_getFieldName(0)));
	q.bindValue(":value", value.toString());
	q.bindValue(":ind", data(createIndex(index.row(), 0)).toString());
	if (!::exec(q))
		return false;
	emit dataChanged(index, index, {role});
	return true;
}

/*!
 * \brief Ermittlung der Feldnamen der Tabelle
 * \param section Spalte
 * \return Name des Feldes
 *
 * Da die Felder des Views anders heißen als die Felder der zugrundeliegenden Tabelle, wird hier mit schnöden
 * if-Abfragen der Name des Feldes im View in den Namen des Feldes in der Tabelle umgewandelt.
 */
QString TableModel::m_getFieldName(int section) {
	QString viewName = headerData(section, Qt::Horizontal).toString();

	if (viewName == tr("ISBN"))
		return tr("isbn");
	else if (viewName == tr("Jahrgangsstufe"))
		return tr("jgst");
	else if (viewName == tr("Titel") || viewName == tr("Name"))
		return tr("name");
	else if (viewName == tr("Voraussichtl. Abschlussjahr"))
		return tr("vajahr");
	else if (viewName == tr("Klassenbuchstabe"))
		return tr("kbuchst");
	else if (viewName == tr("id"))
		return tr("id");
	else if (viewName == tr("Kürzel"))
		return tr("kuerzel");
	return QString();
}

/*!
 * \brief Konstruktor von ListenTab
 * \param tableName Name der Tabelle, in die eingefügt wird
 * \param viewName Name des Views, der angezeigt wird
 * \param parent Elternwidget
 * \see m_createComponents(), m_alignComponents(), m_setInitialValues(), m_connectComponents()
 *
 * Abgesehen von der Initialisierung der Attribute a_tableName und a_viewName werden hier
 * die restlichen Aufgaben delegiert.
 */
TableTab::TableTab(QString tableName, QString viewName, QWidget *parent) : QWidget(parent),
	a_tableName(tableName), a_viewName(viewName) {
	m_createComponents();
	m_alignComponents();
	m_setInitialValues();
	m_connectComponents();
}

/*!
 * \brief Erstellt die Komponenten (also alle beide)
 */
void TableTab::m_createComponents() {
	a_tableView = new QTableView;
	a_tableModel = new TableModel(a_tableName);
}

/*!
 * \brief Ordnet die Komponente in einem Layout an
 */
void TableTab::m_alignComponents() {
	QHBoxLayout *a = new QHBoxLayout;
	a->addWidget(a_tableView);
	setLayout(a);
}

/*!
 * \brief Setzt die Anfangswerte
 */
void TableTab::m_setInitialValues() {
	a_selection = tr("SELECT * FROM %1").arg(a_viewName);
	if (a_tableName == tr("buch"))
		a_order = tr(" ORDER BY ISBN");
	else if (a_tableName == tr("lehrer") || a_tableName == tr("schueler"))
		a_order = tr(" ORDER BY id");
	else
		a_order = tr(" ORDER BY Datum");

	a_query = a_selection + a_order;
	a_tableModel->setQuery(a_query);
	a_tableView->setModel(a_tableModel);

	a_tableView->setSelectionBehavior(QAbstractItemView::SelectItems);
	a_tableView->setSelectionMode(QAbstractItemView::SingleSelection);
	a_tableView->resizeColumnsToContents();

	a_tableView->horizontalHeader()->setSectionsClickable(true);
	a_tableView->horizontalHeader()->setSortIndicatorShown(true);
}

/*!
 * \brief Verbindet die Komponente mit dem Slot
 */
void TableTab::m_connectComponents() {
	connect(a_tableView->horizontalHeader(), SIGNAL(sectionClicked(int)), this, SLOT(headerClicked(int)));
}

/*!
 * \brief Wird aufgerufen, wenn eine Spalte angeklickt wird.
 * \param section Gibt an, welche Spalte angeklickt wurde
 *
 * Zuerst wird die Abfrage des Datenmodells verändert und danach das Feld, nach dem zu sortieren ist,
 * in a_order gespeichert.
 */
void TableTab::headerClicked(int section) {
	Q_ASSERT(section >= 0);
	QString column;
	column = a_tableModel->headerData(section, Qt::Horizontal).toString();
	if (column == QString())
		return;
	if (a_order == tr(" ORDER BY `%1`").arg(column)) {
		a_order = tr(" ORDER BY `%1` DESC").arg(column);
		a_tableView->horizontalHeader()->setSortIndicator(section, Qt::DescendingOrder);
	} else {
		a_order = tr(" ORDER BY `%1`").arg(column);
		a_tableView->horizontalHeader()->setSortIndicator(section, Qt::AscendingOrder);
	}
	a_query = a_selection + a_order;
	refresh();
}

/*!
 * \brief Löscht den angewählten Datensatz
 *
 * Vorausgesetzt es ist ein Datensatz ausgewählt, wird dieser mithilfe der Methode m_deleteRecord()
 * entfernt.
 */
void TableTab::deleteItem() {
	if (a_tableView->selectionModel()->selectedIndexes().isEmpty())
		return;
	if (a_tableName == tr("schueler"))
		m_deleteRecord(tr("schueler"), tr("id"), a_tableModel->record(a_tableView->selectionModel()
									      ->selectedIndexes()[0].row()).value("id").toString());
	else if (a_tableName == tr("lehrer"))
		m_deleteRecord(tr("lehrer"), tr("id"), a_tableModel->record(a_tableView->selectionModel()
									    ->selectedIndexes()[0].row()).value("id").toString());
	else if (a_tableName == tr("buch"))
		m_deleteRecord(tr("buch"), tr("isbn"), a_tableModel->record(a_tableView->selectionModel()
									    ->selectedIndexes()[0].row()).value("ISBN").toString());
	refresh();
}

/*!
 * \brief Löscht einen Datensatz
 * \param table Tabelle
 * \param primaryName Name des Primärschlüssels
 * \param primaryKey Wert des Primärschlüssels
 */
void TableTab::m_deleteRecord(QString table, QString primaryName, QString primaryKey) {
	QSqlQuery q;

	q.prepare(tr("DELETE FROM %1 WHERE %2 = :pk").arg(table).arg(primaryName));
	q.bindValue(":pk", primaryKey);
	::exec(q);
}

/*!
 * \brief Aktualisiert die Daten
 *
 * Zuerst wird die Abfrage abgefragt, dann neu gesetzt. Zum Schluss wird die Tabelle dem Inhalt
 * angepasst.
 */
void TableTab::refresh() {
	a_tableModel->setQuery(a_query);
	a_tableView->resizeColumnsToContents();
	update();
}

/*!
 * \brief Gibt an, ob selektiert wird
 * \return true - Selektion, false - Alle Datensätze angezeigt
 */
bool TableTab::isSelected() {
	return a_selection != tr("SELECT * FROM %1").arg(a_viewName);
}

/*!
 * \brief Setzt die Selektion zurück
 *
 * Die Abfrage von a_tableModel wird zurückgesetzt und a_selection zurückgesetzt.
 */
void TableTab::reset() {
	a_selection = tr("SELECT * FROM %1").arg(a_viewName);
	a_query = a_selection + a_order;
	a_tableModel->setQuery(a_query);
}

/*!
 * \brief Setzt den ersten Teil der SQL-Abfrage
 * \param selection Erster Teil der Abfrage "SELECT * FROM viewname [WHERE bedingung]
 *
 * Zuerst wird a_query gesetzt und anschließend die Abfrage von a_tableModel.
 */
void TableTab::setSelection(QString selection) {
	a_selection = selection;
	a_query = a_selection + a_order;
	a_tableModel->setQuery(a_query);
}

/*!
 * \brief Versteckt die Zeilenüberschriften wegen Redundanz.
 */
void TableTab::hideVHeader() {
	a_tableView->verticalHeader()->hide();
}

/*!
 * \brief Konstruktor von TableView
 * \param parent Elternwidget
 * \see m_createComponents(), m_alignComponents(), m_setInitialValues(), m_connectComponents()
 */
TableView::TableView(QWidget *parent) : QWidget(parent) {
	m_createComponents();
	m_alignComponents();
	m_setInitialValues();
	m_connectComponents();
}

/*!
 * \brief Gibt an, ob im aktuellen Tab selektiert wird
 * \return true - Selektion, false - keine Selektion
 */
bool TableView::isSelecting() const {
	return a_tabs[a_tabWidget->currentIndex()]->isSelected();
}

/*!
 * \brief Gibt den Index des aktuellen Tabs an
 * \return Index des aktuellen Tabs
 */
int TableView::tabIndex() const {
	return a_tabWidget->currentIndex();
}

/*!
 * \brief Erstellt die Komponenten
 */
void TableView::m_createComponents() {
	a_tabWidget = new QTabWidget;

	a_tabs[0] = new TableTab(QString(), tr("SAusleihen"));
	a_tabs[1] = new TableTab(QString(), tr("LAusleihen"));
	a_tabs[2] = new TableTab(QString(), tr("BTausch"));
	a_tabs[3] = new TableTab(tr("schueler"), tr("Schueler"));
	a_tabs[4] = new TableTab(tr("lehrer"), tr("Lehrer"));
	a_tabs[5] = new TableTab(tr("buch"), tr("Buecher"));
}

/*!
 * \brief Ordnet die Komponenten an
 */
void TableView::m_alignComponents() {
	a_tabWidget->addTab(a_tabs[0], tr("Schülerausleihe"));
	a_tabWidget->addTab(a_tabs[1], tr("Lehrerausleihe"));
	a_tabWidget->addTab(a_tabs[2], tr("Büchertausch"));
	a_tabWidget->addTab(a_tabs[3], tr("Schüler"));
	a_tabWidget->addTab(a_tabs[4], tr("Lehrer"));
	a_tabWidget->addTab(a_tabs[5], tr("Bücher"));
	QHBoxLayout *layout = new QHBoxLayout;

	layout->addWidget(a_tabWidget);
	setLayout(layout);
}

/*!
 * \brief Setzt die Anfangswerte
 */
void TableView::m_setInitialValues() {
	a_tabs[3]->hideVHeader();
	a_tabs[4]->hideVHeader();
	a_tabWidget->setTabPosition(QTabWidget::South);
	a_tabWidget->setCurrentIndex(0);
	a_tabs[0]->refresh();
}

/*!
 * \brief Verbindet die Komponenten mit den Slots
 */
void TableView::m_connectComponents() {
	connect(a_tabWidget, SIGNAL(currentChanged(int)), this, SLOT(changeTab(int)));
}

/*!
 * \brief Bucht eine Bücherausleihe
 *
 * Es wird ein neuer InsertDialog erzeugt, ausgeführt und gelöscht. Anschließend wird der
 * aktuelle Tab neu geladen.
 */
void TableView::lendBook() {
	InsertDialog *dl = new InsertDialog(false, this);
	dl->exec();
	delete dl;
	refresh();
}

/*!
 * \brief Bucht eine Bücherrückgabe
 *
 * Es wird ein neuer DeleteDialog erzeugt, ausgeführt und gelöscht. Anschließend wird, analog zu
 * lendBook() der aktuelle Tab neu geladen.
 */
void TableView::withdrawBook() {
	DeleteDialog *dl = new DeleteDialog(false, this);
	dl->exec();
	delete dl;
	refresh();
}


/*!
 * \brief Fügt einen neuen Datensatz ein
 *
 * Zuerst wird ein neuer InsertionDialog erzeugt und angezeigt. Danach wird der aktuelle Tab neu
 * geladen und der Dialog wieder gelöscht.
 */
void TableView::insertRecord() {
	Q_ASSERT(a_tabWidget->currentIndex() >= 3);
	InsertionDialog *dlg = new InsertionDialog(a_tabWidget->currentIndex() - 3, this);
	dlg->exec();
	delete dlg;
	refresh();
}

/*!
 * \brief Löscht den markierten Datensatz
 *
 * Sofern der Index des aktuellen Tabs nicht -1 ist, wird die deleteItem()-Methode des aktuellen Tabs
 * aufgerufen.
 */
void TableView::deleteRecord() {
	Q_ASSERT(a_tabWidget->currentIndex() >= 3);
	a_tabs[a_tabWidget->currentIndex()]->deleteItem();
	refresh();
}

/*!
 * \brief Sucht nach einem Datensatz
 *
 * Es wird ein neuer FindDialog erzeugt und ausgeführt. Im Anschluss wird die Abfrage des
 * aktuellen Tabs gesetzt. Danach wird der FindDialog wieder gelöscht.
 */
void TableView::find() {
	Q_ASSERT(a_tabWidget->currentIndex() >= 0);
	QString query;
	FindDialog *dlg = new FindDialog(a_tabWidget->currentIndex(), this);
	if (dlg->exec() && !(query = dlg->getQuery()).isEmpty())
		a_tabs[dlg->getIndex()]->setSelection(query);
	delete dlg;
	refresh();
}

/*!
 * \brief Setzt die Selektion des aktuellen Tabs zurück
 *
 * Sofern der Index des aktuellen Tabs nicht -1 ist, wird die reset()-Methode des aktuellen
 * Tabs aufgerufen.
 */
void TableView::reset(){
	Q_ASSERT(a_tabWidget->currentIndex() >= 0);
	a_tabs[a_tabWidget->currentIndex()]->reset();
	refresh();
}

/*!
 * \brief Bearbeitet die Einstellungen für dieses Programm
 *
 * Es wird ein neuer SettingsDialog erzeugt, ausgeführt und gelöscht.
 */
void TableView::showSettings() {
	SettingsDialog *dlg = new SettingsDialog(this);
	dlg->exec();
	delete dlg;
}

/*!
 * \brief Aktualisiert die Ansicht
 *
 * Ruft die refresh()-Methode des aktuellen Tabs auf
 */
void TableView::refresh() {
	Q_ASSERT(a_tabWidget->currentIndex() >= 0);
	a_tabs[tabIndex()]->refresh();
}

/*!
 * \brief Wird aufgerufen, wenn ein anderer Tab ausgewählt wurde
 * \param index Index des neuen aktuellen Tabs
 *
 * Es wird tabChanged() emittiert.
 */
void TableView::changeTab(int index) {
	emit tabChanged(index);
}

/*!
 * \brief Importiert Schülerdaten
 *
 * Erst wird ein ImportDialog erzeugt und angezeigt. Anschließend wird das Fenster aktualisiert.
 */
void TableView::import() {
	ImportDialog *dlg = new ImportDialog(this);
	dlg->exec();
	delete dlg;
	refresh();
}
