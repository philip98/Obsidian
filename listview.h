#ifndef LISTVIEW_H
#define LISTVIEW_H

#include <QtContainerFwd>
#include <QWidget>
#include <QTableWidget>
#include <QAbstractTableModel>
#include <QComboBox>
#include <QPushButton>
#include <QRadioButton>
#include <QSqlQuery>
#include <QProxyStyle>

/*!
 * \brief Der Proxy-Style, mithilfe dessen die Bücher in der Listenansicht um 270 Grad rotiert werden
 */
class ListStyle : public QProxyStyle {
public:
	ListStyle(QStyle *style) : QProxyStyle(style) {}
	void drawControl(ControlElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const;
};

/*!
 * \brief Das Datenmodell der Tabelle im Schujahresend-Modus
 */
class ListModel : public QAbstractTableModel {
	Q_OBJECT
private:
	QString a_form;                          ///< Klasse, die angezeigt werden soll
	QMap<QString, int> a_students;           ///< Zuordnung Schüler->SId
	QList<int> a_showed;                     ///< Zuordnung Zeile->SId
	QMap<QString, QString> a_books;          ///< Zuordnung Titel->ISBN
	QHash<int, QHash<QString, bool>> a_lent; ///< Liste der Ausleihen
	int a_numStudents;                       ///< Anzahl der Schüler
	int a_displayedForm;                     ///< Jgst, deren Bücher anzuzeigen sind
	QSqlQuery a_q;                           ///< Zu verwendendes QSqlQuery-Objekt

public:
	ListModel(QObject *parent = 0);
	QVariant data(const QModelIndex &index, int role) const;
	QVariant headerData(int section, Qt::Orientation orientation, int role) const;
	int rowCount(const QModelIndex &) const;
	int columnCount(const QModelIndex &) const;
	void loadHeader();
	void loadData();
	void setForm(QString form);
	void setDisplayedForm(int form);
};

class ListTable : public QTableView {
	Q_OBJECT
public:
	ListTable(QWidget *parent = 0) : QTableView(parent) {}
protected:
	void keyPressEvent(QKeyEvent *event);
};

/*!
 * \brief Die Listenansicht
 */
class ListView : public QWidget {
	Q_OBJECT
private:
	ListTable *a_tableView;                 ///< Zeigt die Liste an
	ListModel *a_tableModel;                 ///< Datenmodell der Tabelle
	QComboBox *a_form;                       ///< Dient zur Auswahl der Klasse
	QRadioButton *a_newList;                 ///< Gibt an, ob die neue Liste angezeigt wird
	QRadioButton *a_oldList;                 ///< Gibt an, ob die alte Liste angezeigt wird
	QSqlQuery a_q;                           ///< Zu verwendendes QSqlQuery-Objekt

	void m_populateCombo();
	void m_createComponents();
	void m_alignComponents();
	void m_setInitialValues();
	void m_connectComponents();
	int m_calculateForm(QString form);

public:
	ListView(QWidget *parent = 0);

private slots:
	void changeForm(QString form);
	void toggle();

public slots:
	void refresh();
	void lendBook();
	void withdrawBook();

};

#endif
