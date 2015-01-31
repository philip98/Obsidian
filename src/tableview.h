#ifndef TABLEVIEW_H
#define TABLEVIEW_H

#include "main.h"
#include <QMainWindow>
#include <QDockWidget>
#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QTableView>
#include <QSqlTableModel>
#include <QSqlRelationalTableModel>

/*!
 * \brief Das Datenmodell für die nicht veränderbaren Tabellen der Tabellenansicht
 */
class RORelationalTableModel: public QSqlRelationalTableModel {
	Q_OBJECT
public:
	RORelationalTableModel(QObject *parent = 0): QSqlRelationalTableModel(parent) {}
	Qt::ItemFlags flags(const QModelIndex &index) const {
		return QSqlRelationalTableModel::flags(index) &
				~Qt::ItemIsEditable;
	}
};

/*!
 * \brief Das Fenster der Listenansicht
 */
class TableView : public QWidget {
	Q_OBJECT
private:
	QTabWidget *a_tabWidget;                 ///< Komprimiert die Anzeige der Tabellen
	QTableView *a_tabs[7];                   ///< Enthält die einzelnen Tabs
	QSqlTableModel *a_models[7];

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
	void headerClicked(QModelIndex index);

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
