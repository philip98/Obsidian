#ifndef VIEW_H
#define VIEW_H

#include "tableview.h"
#include "listview.h"
#include <QMainWindow>

class View : public QMainWindow
{
	Q_OBJECT
private:
	bool a_showsTable;                       ///< Indikator, ob die Tabellenansicht angezeigt wird
	TableView *a_table;                      ///< Tabellenansicht
	ListView *a_list;                        ///< Listenansicht
	QToolBar *a_tools;                       ///< ToolBar
	QStatusBar *a_status;                    ///< StatusBar
	QMenu *a_menuLend;                       ///< Ausleih-Menü
	QMenu *a_menuHelp;                       ///< Hilfe-Menü
	QMenu *a_menuEdit;                       ///< Bearbeiten-Menü
	QMenu *a_menuView;                       ///< Ansicht-Menü
	QAction *a_lend;                         ///< Ausleihe|Ausleihen
	QAction *a_withdraw;                     ///< Ausleihe|Zurückgeben
	QAction *a_about;                        ///< Hilfe|Über
	QAction *a_aboutQt;                      ///< Hilfe|Über Qt
	QAction *a_doc;                          ///< Hilfe|Dokumentation anzeigen
	QAction *a_find;                         ///< Bearbeiten|Suchen
	QAction *a_insert;                       ///< Bearbeiten|Einfügen
	QAction *a_delete;                       ///< Bearbeiten|Löschen
	QAction *a_reset;                        ///< Bearbeiten|Zurücksetzen
	QAction *a_settings;                     ///< Bearbeiten|Einstellungen
	QAction *a_refresh;                      ///< Bearbeiten|Aktualisieren
	QAction *a_import;                       ///< Bearbeiten|Importieren
	QAction *a_tableView;                    ///< Ansicht|Tabellenansicht
	QAction *a_listView;                     ///< Ansicht|Listenansicht

	void m_createComponents();
	void m_alignComponents();
	void m_setInitialValues();
	void m_connectComponents();
	void m_updateMenus();

public:
	View(QWidget *parent = 0);

private slots:
	void showAbout();
	void showDoc();
	void find();
	void refresh();
	void toggle();
};

#endif
