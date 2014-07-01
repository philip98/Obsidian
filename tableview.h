#ifndef TABLEVIEW_H
#define TABLEVIEW_H

#include "main.h"
#include <QMainWindow>
#include <QDockWidget>
#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QTableView>
#include <QSqlQueryModel>

/*!
 * \brief Das Datenmodell für die Tabellen der Tabellenansicht
 */
class TableModel: public QSqlQueryModel {
	Q_OBJECT
private:
	QString a_tableName;                     ///< Der Name der Tabelle

public:
	TableModel(QString table, QObject *parent = 0);
	Qt::ItemFlags flags(const QModelIndex &index) const;
	bool setData(const QModelIndex &index, const QVariant &value, int role);
	QString m_getFieldName(int section);
};

/*!
 * \brief Tab in der Listenansicht
 */
class TableTab : public QWidget {
	Q_OBJECT
private:
	QString a_tableName;                     ///< Dem View zugrunde liegende Tabelle
	QString a_viewName;                      ///< Anzuzeigender View
	QString a_selection;                     ///< Erster Teil der Abfrage "SELECT * FROM tabelle [WHERE bedingung]"
	QString a_query;                         ///< Gesamtabfrage, die immer wieder neu zusammengesetzt wird
	QString a_order;                         ///< Zweiter Teil der Abfrage " ORDER BY feld [ASC/DESC]"
	QTableView *a_tableView;                 ///< Zeigt den Inhalt des Views an
	TableModel *a_tableModel;                ///< Datenmodell von a_tableView

	void m_deleteRecord(QString table, QString primaryName, QString primaryKey);
	void m_createComponents();
	void m_alignComponents();
	void m_setInitialValues();
	void m_connectComponents();

public:
	TableTab(QString tableName, QString viewName, QWidget *parent = 0);

	bool isSelected();
	void setSelection(QString selection);
	void hideVHeader();

private slots:
	void headerClicked(int section);

public slots:
	void deleteItem();
	void reset();
	void refresh();
};

/*!
 * \brief Das Fenster der Listenansicht
 */
class TableView : public QWidget {
	Q_OBJECT
private:
	QTabWidget *a_tabWidget;                 ///< Komprimiert die Anzeige der Tabellen
	TableTab *a_tabs[6];                     ///< Enthält die einzelnen Tabs


	void m_createComponents();
	void m_alignComponents();
	void m_setInitialValues();
	void m_connectComponents();

public:
	TableView(QWidget *parent = 0);
	bool isSelecting() const;
	int tabIndex() const;

private slots:
	void changeTab(int index);

public slots:
	void lendBook();
	void withdrawBook();
	void insertRecord();
	void deleteRecord();
	void find();
	void reset();
	void showSettings();
	void refresh();
	void import();

signals:
	void tabChanged(int index);
};


#endif
