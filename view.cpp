#include "view.h"
#include "insertdlg.h"
#include "deletedlg.h"
#include "settingsdlg.h"
#include <QtCore>
#include <QtGui>
#include <QtWidgets>

/*!
 * \brief Konstruktor von View
 * \param parent Elternwidget
 * \see m_createComponents(), m_alignComponents(), m_setInitialValues(), m_connectComponents()
 */
View::View(QWidget *parent) :
	QMainWindow(parent)
{
	m_createComponents();
	m_alignComponents();
	m_setInitialValues();
	m_connectComponents();
}

/*!
 * \brief Erstellt die Komponenten und Aktionen
 */
void View::m_createComponents() {
	a_table = new TableView;
	a_list = new ListView;
	a_tools = addToolBar(tr("Werkzeuge"));
	a_status = statusBar();

	a_menuLend = menuBar()->addMenu(tr("&Ausleihe"));
	a_menuEdit = menuBar()->addMenu(tr("&Bearbeiten"));
	a_menuView = menuBar()->addMenu(tr("An&sicht"));
	a_menuHelp = menuBar()->addMenu(tr("&Hilfe"));
	menuBar()->setNativeMenuBar(false);

	a_lend = new QAction(QIcon(":/images/ausleihe.png"), tr("Aus&leihe buchen..."), this);
	a_withdraw = new QAction(QIcon(":/images/rueckgabe.png"), tr ("&Rückgabe buchen..."), this);
	a_about = new QAction(tr("Ü&ber..."), this);
	a_aboutQt = new QAction(tr("Über &Qt..."), this);
	a_doc = new QAction(QIcon(":/images/hilfe.png"), tr("&Dokumentation anzeigen..."), this);
	a_find = new QAction(QIcon(":/images/suchen.png"), tr("&Suchen..."), this);
	a_insert = new QAction(QIcon(":/images/einfuegen.png"), tr("&Einfügen..."), this);
	a_delete = new QAction(QIcon(":/images/loeschen.png"), tr("&Löschen"), this);
	a_reset = new QAction(QIcon(":/images/zurueck.png"), tr("Zu&rücksetzen"), this);
	a_settings = new QAction(tr("Ein&stellungen..."), this);
	a_refresh = new QAction(QIcon(":/images/aktualisieren.png"), tr("&Aktualisieren"), this);
	a_import = new QAction(tr("Schülerdaten &importieren..."), this);
	a_tableView = new QAction(tr("&Tabellenansicht"), this);
	a_listView = new QAction(tr("&Listenansicht"), this);
}

/*!
 * \brief Ordnet die Komponenten an und erstellt die Menüs
 */
void View::m_alignComponents() {
	a_menuLend->addAction(a_lend);
	a_menuLend->addAction(a_withdraw);

	a_menuEdit->addAction(a_insert);
	a_menuEdit->addAction(a_import);
	a_menuEdit->addAction(a_delete);
	a_menuEdit->addSeparator();
	a_menuEdit->addAction(a_find);
	a_menuEdit->addAction(a_reset);
	a_menuEdit->addAction(a_refresh);
	a_menuEdit->addSeparator();
	a_menuEdit->addAction(a_settings);

	QActionGroup *ag = new QActionGroup(this);
	ag->addAction(a_tableView);
	ag->addAction(a_listView);
	a_menuView->addAction(a_tableView);
	a_menuView->addAction(a_listView);

	a_menuHelp->addAction(a_about);
	a_menuHelp->addAction(a_aboutQt);
	a_menuHelp->addSeparator();
	a_menuHelp->addAction(a_doc);

	a_tools->addAction(a_lend);
	a_tools->addAction(a_withdraw);
	a_tools->addSeparator();
	a_tools->addAction(a_insert);
	a_tools->addAction(a_delete);
	a_tools->addSeparator();
	a_tools->addAction(a_find);
	a_tools->addAction(a_reset);
	a_tools->addAction(a_refresh);
	a_tools->addSeparator();
	a_tools->addAction(a_doc);

	QWidget *w = new QWidget;
	setCentralWidget(w);

	QStackedLayout *layout = new QStackedLayout;
	layout->addWidget(a_table);
	layout->addWidget(a_list);
	w->setLayout(layout);
}

/*!
 * \brief Setzt die StatusTips und Shortcuts
 */
void View::m_setInitialValues() {
	a_menuLend->setStatusTip(tr("Bietet Optionen zum Verbuchen von Ausleihen und Rückgaben"));
	a_menuEdit->setStatusTip(tr("Bietet Optionen zum Bearbeiten der Datensätze"));
	a_menuView->setStatusTip(tr("Bietet die Möglichkeit zwischen Tabellen- und Listenansicht umzuschalten"));
	a_menuHelp->setStatusTip(tr("Informationen über die Bedienung des Programms sind hier zu finden"));

	a_lend->setStatusTip(tr("Öffnet den Ausleihe-Dialog"));
	a_withdraw->setStatusTip(tr("Öffnet den Rückgabe-Dialog"));
	a_about->setStatusTip(tr("Zeigt Informationen über das Programm an"));
	a_aboutQt->setStatusTip(tr("Zeigt Informationen über Qt an"));
	a_doc->setStatusTip(tr("Zeigt die (hoffentlich nicht so schlechte) Dokumentation an"));
	a_find->setStatusTip(tr("Öffnet den Suchen-Dialog"));
	a_insert->setStatusTip(tr("Fügt einen neuen Datensatz ein"));
	a_delete->setStatusTip(tr("Löscht einen Datensatz"));
	a_reset->setStatusTip(tr("Setzt die Suche zurück"));
	a_settings->setStatusTip(tr("Öffnet die Einstellungen für dieses Programm"));
	a_refresh->setStatusTip(tr("Lädt die aktuelle Ansicht neu"));
	a_import->setStatusTip(tr("Importiert Schülerdaten aus einer CSV-Datei"));
	a_tableView->setStatusTip(tr("Wechselt in die Tabellen-Ansicht"));
	a_listView->setStatusTip(tr("Wechselt in die Listenansicht"));

	a_lend->setShortcut(QKeySequence("Ctrl+L"));
	a_withdraw->setShortcut(QKeySequence("Ctrl+R"));
	a_doc->setShortcut(QKeySequence(Qt::Key_F1));
	a_find->setShortcut(QKeySequence("Ctrl+F"));
	a_insert->setShortcut(QKeySequence(Qt::Key_Insert));
	a_delete->setShortcut(QKeySequence(Qt::Key_Delete));
	a_refresh->setShortcut(QKeySequence(Qt::Key_F5));
	a_import->setShortcut(QKeySequence("Ctrl+I"));

	a_tableView->setCheckable(true);
	a_listView->setCheckable(true);
	a_tableView->setChecked(true);

	a_showsTable = true;
	m_updateMenus();
	a_tools->setAllowedAreas(Qt::TopToolBarArea);
}

/*!
 * \brief Verbindet die Aktionen mit den Slots
 */
void View::m_connectComponents() {
	connect(a_lend, SIGNAL(triggered()), a_table, SLOT(lendBook()));
	connect(a_withdraw, SIGNAL(triggered()), a_table, SLOT(withdrawBook()));
	connect(a_about, SIGNAL(triggered()), this, SLOT(showAbout()));
	connect(a_aboutQt, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
	connect(a_doc, SIGNAL(triggered()), this, SLOT(showDoc()));
	connect(a_find, SIGNAL(triggered()), this, SLOT(find()));
	connect(a_insert, SIGNAL(triggered()), a_table, SLOT(insertRecord()));
	connect(a_delete, SIGNAL(triggered()), a_table, SLOT(deleteRecord()));
	connect(a_reset, SIGNAL(triggered()), a_table, SLOT(reset()));
	connect(a_settings, SIGNAL(triggered()), a_table, SLOT(showSettings()));
	connect(a_refresh, SIGNAL(triggered()), this, SLOT(refresh()));
	connect(a_import, SIGNAL(triggered()), a_table, SLOT(import()));
	connect(a_tableView, SIGNAL(toggled(bool)), this, SLOT(toggle()));
//	connect(a_listView, SIGNAL(toggled(bool)), this, SLOT(toggle()));
	connect(a_table, SIGNAL(tabChanged(int)), this, SLOT(refresh()));
}

/*!
 * \brief Ändert die Menüs
 *
 * Bei einem Wechsel der Ansicht werden a_lend und a_withdraw umgestöpselt und
 * entsprechend der Ansicht, des Selektionsstatus' und dem aktuellen Tab das Menü
 * angepasst.
 */
void View::m_updateMenus() {
	a_menuEdit->setEnabled(a_showsTable);
	a_insert->setEnabled(a_showsTable);
	a_delete->setEnabled(a_showsTable);
	a_find->setEnabled(a_showsTable);
	a_reset->setEnabled(a_showsTable);
	if (a_showsTable) {
		a_lend->disconnect();
		a_withdraw->disconnect();
		connect(a_lend, SIGNAL(triggered()), a_table, SLOT(lendBook()));
		connect(a_withdraw, SIGNAL(triggered()), a_table, SLOT(withdrawBook()));
		if (a_table->tabIndex() < 3) {
			a_insert->setEnabled(false);
			a_delete->setEnabled(false);
			a_import->setEnabled(false);
		} else {
			a_insert->setEnabled(true);
			a_delete->setEnabled(true);
			a_import->setEnabled(true);
		}

		if (a_table->isSelecting())
			a_reset->setEnabled(true);
		else
			a_reset->setEnabled(false);
	} else {
		a_lend->disconnect();
		a_withdraw->disconnect();
		connect(a_lend, SIGNAL(triggered()), a_list, SLOT(lendBook()));
		connect(a_withdraw, SIGNAL(triggered()), a_list, SLOT(withdrawBook()));

	}
}

/*!
 * \brief Zeigt den Über-Dialog
 */
void View::showAbout() {
	QMessageBox::about(this, "Über", "<img src=\":/images/titel.png\" alt=\"\" /></br>"
			   "<h1>B&uuml;cherverwaltung</h1><br/> <p>Dies ist ein Open-Source-Programm, "
			   "das dazu dient, der Lehrmittelbibliothek des Michaeli-Gymnasiums die Arbeit "
			   "zu erleichtern. </p><p>Von Martin Hirschberger, Philip Schl&ouml;sser (philipschloesser@gmx.net)</p>");
}

/*!
 * \brief Zeigt die Dokumentation an
 */
void View::showDoc() {
	QSettings settings("buecherverwalter.conf", QSettings::IniFormat);
	QProcess *pdfViewer = new QProcess;
	QStringList arguments;
	arguments << qApp->applicationDirPath() + "/doc.pdf";
	pdfViewer->start(settings.value("allg/pdf-viewer", "/usr/bin/evince").toString(), arguments);
}

/*!
 * \brief Sucht einen Datensatz
 */
void View::find() {
	Q_ASSERT(a_showsTable);
	a_table->find();
	m_updateMenus();
}

/*!
 * \brief Lädt alles neu
 */
void View::refresh() {
	if (a_showsTable)
		a_table->refresh();
	else
		a_list->refresh();
	m_updateMenus();
}

/*!
 * \brief Schaltet zwischen Tabellen- und Listenansicht um
 */
void View::toggle() {
	if (a_showsTable)
		qobject_cast<QStackedLayout *>(centralWidget()->layout())->setCurrentIndex(1);
	else
		qobject_cast<QStackedLayout *>(centralWidget()->layout())->setCurrentIndex(0);

	a_showsTable = !a_showsTable;
	refresh();
}
