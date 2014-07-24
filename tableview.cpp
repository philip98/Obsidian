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
	return !a_models[tabIndex()]->filter().isEmpty();
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

	for (int i = 0; i < 7; ++i)
		a_tabs[i] = new QTableView;
	a_models[0] = new RORelationalTableModel;
	a_models[1] = new RORelationalTableModel;
	a_models[2] = new RORelationalTableModel;
	a_models[3] = new QSqlRelationalTableModel;
	a_models[4] = new QSqlTableModel;
	a_models[5] = new QSqlTableModel;
	a_models[6] = new QSqlTableModel;
}

/*!
 * \brief Ordnet die Komponenten an
 */
void TableView::m_alignComponents() {
	a_tabWidget->addTab(a_tabs[0], tr("Schülerausleihe"));
	a_tabWidget->addTab(a_tabs[1], tr("Lehrerausleihe"));
	a_tabWidget->addTab(a_tabs[2], tr("Büchertausch"));
	a_tabWidget->addTab(a_tabs[3], tr("Aliase"));
	a_tabWidget->addTab(a_tabs[4], tr("Schüler"));
	a_tabWidget->addTab(a_tabs[5], tr("Lehrer"));
	a_tabWidget->addTab(a_tabs[6], tr("Bücher"));
	QHBoxLayout *layout = new QHBoxLayout;

	layout->addWidget(a_tabWidget);
	setLayout(layout);
}

/*!
 * \brief Setzt die Anfangswerte
 */
void TableView::m_setInitialValues() {
	QSqlRelationalTableModel *m = qobject_cast<QSqlRelationalTableModel *>(a_models[0]);
	m->setTable("Sausleihe");
	m->setHeaderData(0, Qt::Horizontal, tr("Klasse"));
	m->setHeaderData(1, Qt::Horizontal, tr("Schüler"));
	m->setHeaderData(2, Qt::Horizontal, tr("Buch"));
	m->setHeaderData(3, Qt::Horizontal, tr("Anzahl"));
	m->setHeaderData(4, Qt::Horizontal, tr("Datum"));
	m->setRelation(0, QSqlRelation("SSchueler", "id", "Klasse"));
	m->setRelation(1, QSqlRelation("SSchueler", "id", "Name"));
	m->setRelation(2, QSqlRelation("Buch", "isbn", "titel"));
	m->select();

	m = qobject_cast<QSqlRelationalTableModel *>(a_models[1]);
	m->setTable("lausleihe");
	m->setHeaderData(0, Qt::Horizontal, tr("Lehrer"));
	m->setHeaderData(1, Qt::Horizontal, tr("Buch"));
	m->setHeaderData(2, Qt::Horizontal, tr("Anzahl"));
	m->setHeaderData(3, Qt::Horizontal, tr("Datum"));
	m->setRelation(0, QSqlRelation("lehrer", "id", "name"));
	m->setRelation(1, QSqlRelation("Buch", "isbn", "titel"));
	m->select();

	m = qobject_cast<QSqlRelationalTableModel *>(a_models[2]);
	m->setTable("Btausch");
	m->setHeaderData(0, Qt::Horizontal, tr("Klasse"));
	m->setHeaderData(1, Qt::Horizontal, tr("Schüler"));
	m->setHeaderData(2, Qt::Horizontal, tr("Buch"));
	m->setHeaderData(3, Qt::Horizontal, tr("Datum"));
	m->setRelation(0, QSqlRelation("SSchueler", "id", "Klasse"));
	m->setRelation(1, QSqlRelation("SSchueler", "id", "Name"));
	m->setRelation(2, QSqlRelation("Buch", "isbn", "titel"));
	m->select();

	m = qobject_cast<QSqlRelationalTableModel *>(a_models[3]);
	m->setTable("Aliasse");
	m->setHeaderData(0, Qt::Horizontal, tr("Alias"));
	m->setHeaderData(1, Qt::Horizontal, tr("ISBN"));
	m->setHeaderData(2, Qt::Horizontal, tr("Titel"));
	m->setRelation(2, QSqlRelation("Buch", "isbn", "titel"));
	m->select();

	a_models[4]->setTable("schueler");
	a_models[4]->setHeaderData(0, Qt::Horizontal, tr("id"));
	a_models[4]->setHeaderData(1, Qt::Horizontal, tr("Name"));
	a_models[4]->setHeaderData(2, Qt::Horizontal, tr("Abschlussjahr"));
	a_models[4]->setHeaderData(3, Qt::Horizontal, tr("Klassenbuchstabe"));
	a_models[4]->select();
	a_models[4]->setEditStrategy(QSqlTableModel::OnFieldChange);
	a_tabs[4]->verticalHeader()->hide();

	a_models[5]->setTable("lehrer");
	a_models[5]->setHeaderData(0, Qt::Horizontal, tr("id"));
	a_models[5]->setHeaderData(1, Qt::Horizontal, tr("Name"));
	a_models[5]->setHeaderData(2, Qt::Horizontal, tr("Kürzel"));
	a_models[5]->select();
	a_models[5]->setEditStrategy(QSqlTableModel::OnFieldChange);
	a_tabs[5]->verticalHeader()->hide();

	a_models[6]->setTable("buch");
	a_models[6]->setHeaderData(0, Qt::Horizontal, tr("ISBN"));
	a_models[6]->setHeaderData(1, Qt::Horizontal, tr("Titel"));
	a_models[6]->setHeaderData(2, Qt::Horizontal, tr("Jgst."));
	a_models[6]->select();
	a_models[6]->setEditStrategy(QSqlTableModel::OnFieldChange);

	for (int i = 0; i < 7; ++i) {
		a_tabs[i]->setModel(a_models[i]);
		a_tabs[i]->setSortingEnabled(true);
		a_tabs[i]->sortByColumn(0, Qt::AscendingOrder);
		a_tabs[i]->horizontalHeader()->setSortIndicator(0, Qt::AscendingOrder);
		a_tabs[i]->horizontalHeader()->setSortIndicatorShown(true);
		a_tabs[i]->setSelectionBehavior(QAbstractItemView::SelectRows);
		a_tabs[i]->setSelectionMode(QAbstractItemView::SingleSelection);
	}

	a_tabWidget->setTabPosition(QTabWidget::South);
	a_tabs[0]->resizeColumnsToContents();
}

/*!
 * \brief Verbindet die Komponenten mit den Slots
 */
void TableView::m_connectComponents() {
	connect(a_tabWidget, SIGNAL(currentChanged(int)), this, SLOT(changeTab(int)));
	for (int i = 0; i < 7; ++i)
		connect(a_tabs[i]->horizontalHeader(), SIGNAL(clicked(QModelIndex)),
			this, SLOT(headerClicked(QModelIndex)));
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
	InsertionDialog *dlg = new InsertionDialog(tabIndex() - 3, this);
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
	if (a_tabs[tabIndex()]->selectionModel()->selectedRows().isEmpty())
		return;
	int row = a_tabs[tabIndex()]->selectionModel()->selectedRows()[0].row();
	a_models[tabIndex()]->removeRow(row);
	refresh();
}

/*!
 * \brief Sucht nach einem Datensatz
 *
 * Es wird ein neuer FindDialog erzeugt und ausgeführt. Im Anschluss wird die Abfrage des
 * aktuellen Tabs gesetzt. Danach wird der FindDialog wieder gelöscht.
 */
void TableView::find() {
	Q_ASSERT(tabIndex() >= 0);
	QString query;
	FindDialog *dlg = new FindDialog(a_tabWidget->currentIndex(), this);
	if (dlg->exec() && !(query = dlg->getFilter()).isEmpty())
		a_models[dlg->getIndex()]->setFilter(query);
	delete dlg;
	refresh();
//	QMessageBox::information(0, tr("Last Query"), a_models[tabIndex()]->query().lastQuery());
}

/*!
 * \brief Setzt die Selektion des aktuellen Tabs zurück
 *
 * Sofern der Index des aktuellen Tabs nicht -1 ist, wird die reset()-Methode des aktuellen
 * Tabs aufgerufen.
 */
void TableView::reset(){
	Q_ASSERT(tabIndex() >= 0);
	a_models[tabIndex()]->setFilter(QString());
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
	Q_ASSERT(tabIndex() >= 0);
	a_models[tabIndex()]->select();
	a_tabs[tabIndex()]->resizeColumnsToContents();
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
 * \brief Wird aufgerufen, wenn der Header des aktuellen Tabs angeklickt wurde
 * \param index Headerspalte
 *
 *
 */
void TableView::headerClicked(QModelIndex index) {
	Qt::SortOrder order;
	if (a_tabs[tabIndex()]->horizontalHeader()->sortIndicatorSection() ==
			index.column())
		order = (a_tabs[tabIndex()]->horizontalHeader()->sortIndicatorOrder() == Qt::AscendingOrder) ?
					Qt::DescendingOrder : Qt::AscendingOrder;
	else
		order = Qt::AscendingOrder;
	a_models[tabIndex()]->sort(index.column(), order);
	a_models[tabIndex()]->select();
	a_tabs[tabIndex()]->horizontalHeader()->setSortIndicator(index.column(), order);
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
