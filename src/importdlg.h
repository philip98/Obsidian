#ifndef IMPORTDLG_H
#define IMPORTDLG_H

#include <QDialog>
#include <QDialogButtonBox>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>
#include <QCheckBox>
#include <QFile>
#include <QSqlQuery>
#include <QAbstractTableModel>
#include <QTableView>

/*!
 * \brief Das Datenmodell für den ImportDialog
 */
class ImportModel : public QAbstractTableModel {
	Q_OBJECT
private:
	QList<QStringList> a_data;               ///< Enthält die Daten
	QStringList a_horizontalHeader;          ///< Enthält den Tabellenkopf
	int a_columnCount;                       ///< Enthält die maximale Spaltenanzahl

public:
	ImportModel(QObject *parent = 0);
	QVariant data(const QModelIndex &index, int role) const;
	QVariant headerData(int section, Qt::Orientation orientation, int role) const;
	int rowCount(const QModelIndex &/*parent*/) const;
	int columnCount(const QModelIndex &/*&parent*/) const;

	void setFileName(QString fileName, QChar rowSeparator, QChar colSeparator);
	void changeHeader(QStringList header);
};

/*!
 * \brief Import-Dialog
 */
class ImportDialog : public QDialog {
	Q_OBJECT
private:
	static constexpr int a_numFields = 6;    ///< Anzahl der Felder
	QSqlQuery a_q;                           ///< Zu verwendendes QSqlQuery-Objekt
	QDialogButtonBox *a_buttonBox;           ///< Ok/Abbrechen
	QPushButton *a_open;                     ///< Dient dem Öffnen einer Datei
	QLineEdit *a_fileName;                   ///< Eingabefeld für den Dateinamen
	QComboBox *a_field[a_numFields];         ///< Legen die Reihenfolge der Felder fest
	QCheckBox *a_commonGradYear;             ///< Sind die Schüler in einer Jgst?
	QCheckBox *a_commonFormLetter;           ///< Sind die Schüler in einer Klasse?
	QLineEdit *a_gradYear;                   ///< Enthält das gemeinsame Abschlussjahr
	QLineEdit *a_formLetter;                 ///< Enthält den gemeinsamen Klassenbuchstaben
	QComboBox *a_rowSeparator;               ///< Enthält den Zeilentrenner
	QComboBox *a_colSeparator;               ///< Enthält den Spaltentrenner
	QTableView *a_formattedInput;            ///< Zeigt das Endergebnis an
	ImportModel *a_inputDataModel;           ///< Datenmodell für a_formattedInput

	int m_getIndex(QString par);
	void m_createComponents();
	void m_alignComponents();
	void m_setInitialValues();
	void m_connectComponents();
public:
	ImportDialog(QWidget *parent = 0);

private slots:
	void openClicked();
	void refresh();

public slots:
	void accept();

};

#endif
