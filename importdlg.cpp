#include "importdlg.h"
#include "main.h"
#include <QtCore>
#include <QtGui>
#include <QtSql>
#include <QtWidgets>

/*!
 * \brief Konstruktor von ImportModel
 * \param parent Elternobjekt
 *
 * Abgesehen vom Aufruf des Elternkonstruktors wird hier nur a_columnCount auf 0 gesetzt
 */
ImportModel::ImportModel(QObject *parent) : QAbstractTableModel(parent) {
	a_columnCount = 0;
}

/*!
 * \brief Setzt den Dateinamen neu
 * \param fileName Dateiname
 * \param rowSeparator Zeichen, das Datensätze voneinander trennt
 * \param colSeparator Zeichen, das die Felder voneinander trennt
 *
 * Die gesamte Datei wird ausgelesen und mithilfe von QStringList::split() entlang der Zeilen aufgetrennt.
 * Danach wird über die Zeilen iteriert und dabei dann jeder Eintrag an den Feldtrennern getrennt und die höchste
 * Anzahl an Feldern pro Datensatz in a_columnCount gespeichert.
 */
void ImportModel::setFileName(QString fileName, QChar rowSeparator, QChar colSeparator) {
	if (fileName.isEmpty())
		return;
	QFile *f = new QFile(fileName);
	f->open(QFile::ReadOnly);
	QTextStream stream(f);
	QStringList str;
	a_columnCount = 0;
	a_data.clear();

	str = stream.readAll().split(rowSeparator);
	if (str.last() == QString())
		str.removeLast();
	for (int i = 0; i < str.count(); ++i) {
		a_data.append(str[i].split(colSeparator));
		if (a_data[i].count() > a_columnCount)
			a_columnCount = a_data.last().count();
	}
	emit dataChanged(createIndex(0, 0), createIndex(str.size(), a_columnCount));
	f->close();
}

/*!
 * \brief Gibt den Inhalt der Zellen aus
 * \param index QModelIndex, der auf die Zelle verweist
 * \param role Art der Daten die abgefragt wird
 * \return Daten
 *
 * Mit Qt::DisplayRole als role wird der Inhalt von a_data an der Stelle ausgegeben, ansonsten QVariant().
 */
QVariant ImportModel::data(const QModelIndex &index, int role) const {
	if (role == Qt::DisplayRole) {
		if (a_data.size() > index.row() && a_data[index.row()].size() > index.column())
			return a_data.at(index.row()).at(index.column());
	}
	return QVariant();
}

/*!
 * \brief Gibt Headerdaten aus
 * \param section Zeile/Spalte des Headereintrags
 * \param orientation Gibt an, ob es sich um vertikale oder horizontale Überschriften handelt
 * \param role Art der Daten die abgefragt werden
 * \return Abgefragte Daten
 *
 * Mit Qt::DisplayRole als Rolle wird im Falle des vertikalen Headers gar nichts und im Falle des horizontalen
 * Headers der entsprechende Inhalt von a_horizontalHeader ausgegeben.
 */
QVariant ImportModel::headerData(int section, Qt::Orientation orientation, int role) const {
	if (section >= a_columnCount || section >= a_horizontalHeader.size())
		return QVariant();
	if (role == Qt::DisplayRole) {
		if (orientation == Qt::Horizontal)
			return a_horizontalHeader.at(section);
		else
			return tr("%1").arg(section);

	}
	return QVariant();
}

/*!
 * \brief Gibt die Anzahl der Zeilen aus
 * \return Zeilenanzahl
 */
int ImportModel::rowCount(const QModelIndex &/*parent*/ ) const {
	return a_data.size();
}

/*!
 * \brief Gibt die Anzahl der Spalten aus
 * \return Spaltenzahl
 */
int ImportModel::columnCount(const QModelIndex &/*parent*/ ) const {
	return a_columnCount;
}

/*!
 * \brief Ändert die Spaltenüberschriften
 * \param header Enthält die neuen Spaltenüberschriften
 */
void ImportModel::changeHeader(QStringList header) {
	a_horizontalHeader = header;
	emit headerDataChanged(Qt::Horizontal, 0, header.count());
}

/*!
 * \brief Konstruktor des ImportDialog
 * \param parent Eltern-Widget
 * \see m_createComponents(), m_alignComponents(), m_setInitialValues(), m_connectComponents()
 */
ImportDialog::ImportDialog(QWidget *parent) : QDialog(parent)
{
	m_createComponents();
	m_alignComponents();
	m_setInitialValues();
	m_connectComponents();
}

/*!
 * \brief Erstellt alle Komponenten
 */
void ImportDialog::m_createComponents() {
	a_buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
					 Qt::Vertical);
	a_open = new QPushButton(tr("Öffnen..."));
	a_fileName = new QLineEdit;
	for (int i = 0; i < a_numFields; ++i)
		a_field[i] = new QComboBox;
	a_commonGradYear = new QCheckBox(tr("Abschlussjahr"));
	a_commonFormLetter = new QCheckBox(tr("Klassenbuchstabe"));
	a_gradYear = new QLineEdit;
	a_formLetter = new QLineEdit;
	a_formattedInput = new QTableView;
	a_inputDataModel = new ImportModel;
	a_rowSeparator = new QComboBox;
	a_colSeparator = new QComboBox;
}

/*!
 * \brief Ordnet alle Komponenten in Layouts an
 */
void ImportDialog::m_alignComponents() {
	QHBoxLayout *a = new QHBoxLayout;
	a->addWidget(a_fileName);
	a->addWidget(a_open);

	QHBoxLayout *b = new QHBoxLayout;
	for (int i = 0; i < a_numFields; ++i) {
		b->addWidget(a_field[i]);
	}
	QGroupBox *fieldGroup = new QGroupBox(tr("Reihenfolge der Felder"));
	fieldGroup->setLayout(b);

	QGridLayout *c = new QGridLayout;
	c->addWidget(a_commonGradYear, 0, 0);
	c->addWidget(a_gradYear, 0, 1);
	c->addWidget(a_commonFormLetter, 1, 0);
	c->addWidget(a_formLetter, 1, 1);
	c->addWidget(new QLabel(tr("Zeilentrenner")), 0, 2);
	c->addWidget(a_rowSeparator, 0, 3);
	c->addWidget(new QLabel(tr("Feldtrenner")), 1, 2);
	c->addWidget(a_colSeparator, 1, 3);
	QGroupBox *commonGroup = new QGroupBox(tr("Allgemeine Daten"));
	commonGroup->setLayout(c);

	QVBoxLayout *d = new QVBoxLayout;
	d->addLayout(a);
	d->addWidget(fieldGroup);
	d->addWidget(commonGroup);
	d->addWidget(a_formattedInput);

	QHBoxLayout *layout = new QHBoxLayout;
	layout->addLayout(d);
	layout->addWidget(a_buttonBox);
	setLayout(layout);
}

/*!
 * \brief Setzt Anfangswerte und ToolTips
 */
void ImportDialog::m_setInitialValues() {
	setWindowTitle(tr("Importieren"));
	QStringList fields;
	fields << tr("Nicht übernehmen") << tr("Vorname") << tr("Nachname") << tr("Name") <<
		  tr("Abschlussjahr") << tr("Klassenbuchstabe");
	for (int i = 0; i < a_numFields; ++i)
		a_field[i]->addItems(fields);
	a_rowSeparator->addItem(tr("LF"));
	a_rowSeparator->addItem(tr("CR"));
	a_colSeparator->addItem(tr(","));
	a_colSeparator->addItem(tr(";"));
	a_colSeparator->addItem(tr("Tabulator"));

	a_formattedInput->setModel(a_inputDataModel);

	a_gradYear->setValidator(new QRegExpValidator(QRegExp("[0-9]{4}")));
	a_formLetter->setValidator(new QRegExpValidator(QRegExp("[a-gA-G]?")));

	a_open->setToolTip(tr("CSV-Datei öffnen"));
	a_fileName->setToolTip(tr("Pfad der CSV-Datei"));
	for (int i = 0; i < a_numFields; ++i)
		a_field[i]->setToolTip(tr("Was steht in dieser Spalte?"));
	a_commonGradYear->setToolTip(tr("Sind alle Schüler in einer Klassenstufe?"));
	a_commonFormLetter->setToolTip(tr("Sind alle Schüler in einer Klasse?"));
	a_gradYear->setToolTip(tr("Gemeinsames Abschlussjahr (Klassenstufe)"));
	a_formLetter->setToolTip(tr("Gemeinsamer Klassenbuchstabe"));
	a_rowSeparator->setToolTip(tr("Welches Zeichen trennt die Zeilen?"));
	a_colSeparator->setToolTip(tr("Welches Zeichen trennt die Spalten?"));
	a_formattedInput->setToolTip(tr("Vorschau der einzufügenden Daten"));
}

/*!
 * \brief Verbindet die Komponenten mit den entsprechenden Slots
 */
void ImportDialog::m_connectComponents() {
	connect(a_open, SIGNAL(clicked()), this, SLOT(openClicked()));
	connect(a_buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
	connect(a_buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
	for (int i = 0; i < a_numFields; ++i)
		connect(a_field[i], SIGNAL(currentIndexChanged(int)), this, SLOT(refresh()));
	connect(a_colSeparator, SIGNAL(currentIndexChanged(int)), this, SLOT(refresh()));
	connect(a_rowSeparator, SIGNAL(currentIndexChanged(int)), this, SLOT(refresh()));
}

/*!
 * \brief Gibt den Index des spezifizierten Feldes in der CSV-Datei an
 * \param par Zu suchende Feldbezeichnung
 * \return Gefundener Index oder -1
 */
int ImportDialog::m_getIndex(QString par) {
	for (int i = 0; i < a_numFields; ++i)
		if (a_field[i]->currentText() == par)
			return i;
	return -1;
}

/*!
 * \brief Wird bei Klick auf den a_open aufgerufen
 *
 * Setzt den Text von a_fileName mittels von QFileDialog::getOpenFileName()
 */
void ImportDialog::openClicked()
{
	a_fileName->setText(QFileDialog::getOpenFileName(this, tr("Öffnen"), a_fileName->text(),
						       "Comma-separated values (*.csv *.txt);;"
						       "Alle Dateien (*.*)"));
	refresh();
}

/*!
 * \brief Wird beim Schließen des Dialogs aufgerufen
 *
 * Da alle Daten bereits richtig aufgeteilt in a_formattedInput vorliegen, wird einfach über die Zeilen
 * iteriert und schließlich eine passende SQL-Abfrage generiert, die den eingegebenen Daten gerecht wird.
 */
void ImportDialog::accept()
{
	a_q.prepare("INSERT INTO `schueler` (`name`, `vajahr`, `kbuchst`) VALUES (:name,"
		  ":vajahr, :kbuchst)");
	QString name, grYear, foLetter;
	int indName, indGrYear, indFoLetter, indFName, indLName;

	indName = m_getIndex(tr("Name"));
	indFName = m_getIndex(tr("Vorname"));
	indLName = m_getIndex(tr("Nachname"));
	indGrYear = m_getIndex(tr("Abschlussjahr"));
	indFoLetter = m_getIndex(tr("Klassenbuchstabe"));

	for (int i = 0; i < a_inputDataModel->rowCount(QModelIndex()); ++i) {
		if (indName != -1)
			name = a_inputDataModel->data(a_inputDataModel->index(i, indName),
						      Qt::DisplayRole).toString().simplified();
		else if (indFName != -1 && indLName != -1) {
			QString fName = a_inputDataModel->data(a_inputDataModel->index(i, indFName),
							       Qt::DisplayRole).toString().simplified();
			QString lName = a_inputDataModel->data(a_inputDataModel->index(i, indLName),
							       Qt::DisplayRole).toString().simplified();
			name = tr("%1 %2").arg(lName).arg(fName);
		} else {
			QMessageBox::critical(this, tr("Fehlende Informationen"), tr("Um Schüler zu importieren,"
				"muss es entweder ein Namensfeld oder eins mit Vor- und eins mit Nachnamen geben!"));
			return;
		}
		if (a_commonGradYear->isChecked())
			grYear = a_gradYear->text();
		if (indGrYear != -1 && a_inputDataModel->data(a_inputDataModel->index(i, indGrYear),
										Qt::DisplayRole).toString().simplified() != QString())
			grYear = a_inputDataModel->data(a_inputDataModel->index(i, indGrYear),
							Qt::DisplayRole).toString().simplified();
		if (!a_commonGradYear->isChecked() && indGrYear == -1)
			a_q.prepare("INSERT INTO `schueler` (`name`) VALUES (:name)");

		if (a_commonFormLetter->isChecked())
			foLetter = a_formLetter->text();
		if (indFoLetter != -1 && a_inputDataModel->data(a_inputDataModel->index(i, indFoLetter),
								Qt::DisplayRole).toString().simplified() != QString())
			foLetter = a_inputDataModel->data(a_inputDataModel->index(i, indFoLetter),
							  Qt::DisplayRole).toString();
		a_q.bindValue(":name", name);
		a_q.bindValue(":vajahr", grYear);
		a_q.bindValue(":kbuchst", foLetter);
		if (!::exec(a_q)) return;
	}
	QDialog::accept();
}

/*!
 * \brief Wird aufgerufen um a_formattedInput zu aktualisieren
 *
 * Zuerst werden die Zeilen- und Feldtrenner in Zeichen umgewandelt und das Datenmodell dazu veranlasst,
 * seine Daten neu zu laden. Anschließend werden die Tabellenüberschriften neu gesetzt
 */
void ImportDialog::refresh() {
	QChar rowSeparator, colSeparator;
	QStringList headers;

	switch (a_rowSeparator->currentIndex()) {
	case 0:
		rowSeparator = '\n';
		break;
	case 1:
		rowSeparator = '\r';
		break;
	}
	switch (a_colSeparator->currentIndex()) {
	case 0:
		colSeparator = ',';
		break;
	case 1:
		colSeparator = ';';
		break;
	case 2:
		colSeparator = '\t';
		break;
	}

	a_inputDataModel->setFileName(a_fileName->text(), rowSeparator, colSeparator);

	for (int i = 0; i < a_numFields; ++i)
		headers.append(a_field[i]->currentText());
	a_inputDataModel->changeHeader(headers);
	a_formattedInput->resizeColumnsToContents();
}
