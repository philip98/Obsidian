#include "deletedlg.h"
#include "selectdlg.h"
#include <QtGui>
#include <QtWidgets>
#include <QtSql>

/*!
 * \brief Konstruktor von DeleteDialog
 * \param endOfTerm Gibt an, ob die Rückgabe im Rahmen des Büchertauschs am Schuljahresende stattfindet
 * \param parent Elternwidget
 * \param record Datensatz, der einen Schüler identifiziert
 * \see BaseDialog(), m_createInterface()
 */
DeleteDialog::DeleteDialog(bool endOfTerm, QWidget *parent, QSqlRecord record):
	BaseDialog(endOfTerm, parent) {
	m_createInterface(record);
}

/*!
 * \brief Ordnet die Komponenten in Layouts an
 */
void DeleteDialog::m_alignComponents() {
	QHBoxLayout *a = new QHBoxLayout;
	a->addWidget(a_form);
	a->addWidget(a_name);
	a->addWidget(a_choose);

	QVBoxLayout *b = new QVBoxLayout;
	b->addWidget(a_student);
	b->addWidget(a_teacher);
	a_lender->setLayout(b);

	QHBoxLayout *c = new QHBoxLayout;
	c->addSpacerItem(new QSpacerItem(1, 1, QSizePolicy::Expanding,
				QSizePolicy::Fixed));
	c->addWidget(a_lender);

	QGridLayout *d = new QGridLayout;
	d->addLayout(a, 0, 0, 1, 2);
	d->addLayout(c, 1, 0, 1, 2);
	for (int i = 0; i < a_numIsbns; ++i) {
		d->addWidget(a_alias[i], i + 2, 0);
		d->addWidget(a_title[i], i + 2, 1);
	}

	QHBoxLayout *layout = new QHBoxLayout;
	layout->addLayout(d);
	layout->addWidget(a_buttonBox);

	setLayout(layout);
}

/*!
 * \brief Setzt die Anfangswerte
 * \param record Datensatz, der den Schüler identifiziert
 * \see BaseDialog::m_setInitialValues()
 */
void DeleteDialog::m_setInitialValues(QSqlRecord record) {
	BaseDialog::m_setInitialValues(record);
	setWindowTitle(tr("Rückgabe buchen"));
}

/*!
 * \brief Überprüft Eingaben und führt SQL-Abfrage aus
 *
 * Im Normalen Modus werden zuerst bei allen Ausleihe-Datensätzen dieses Schülers/Lehrers und
 * des entsprechenden Buchs die Anzahl dekrementiert. Anschließend werden alle Ausleihdatensätze
 * gelöscht deren Anzahl 0 ist.
 *
 * Im Schuljahresendmodus werden alle BTausch-Datensätze mit der betreffenden Schüler-ID
 * und ISBN gelöscht.
 */
void DeleteDialog::accept() {
	if (a_id == -1) {
		QMessageBox::warning(this, tr("Fehlende Angaben"), tr("Noch kein %1 ausgewählt!")
				     .arg(a_teacher->isChecked() ? "Lehrer" : "Schüler"));
		return;
	}
	if (a_alias[0]->text().isEmpty()) {
		QMessageBox::warning(this, tr("Fehlende Angaben"), tr("Keine Bücher ausgewählt!"));
		return;
	}
	if (!a_eot) {
		a_q.prepare(tr("UPDATE %1ausleihe SET anz = anz - 1 WHERE %1id = :lid AND bid = :bid")
			  .arg(a_student->isChecked() ? "s" : "l"));
		for (int i = 0; i < a_numIsbns; ++i) {
			if (!a_isbn[i].isEmpty()) {
				a_q.bindValue(":lid", a_id);
				a_q.bindValue(":bid", a_isbn[i]);
				if (!::exec(a_q)) return;
			}
		}
		if (!a_q.exec(tr("DELETE FROM %1ausleihe WHERE anz <= 0").arg(a_student->isChecked()
									      ? "s" : "l"))) {
			sqlError(a_q);
			return;
		}
	} else {
		a_q.prepare("DELETE FROM `btausch` WHERE `sid` = :sid AND `bid` = :bid");
		for (int i = 0; i < a_numIsbns; ++i) {
			if (!a_isbn[i].isEmpty()) {
				a_q.bindValue(":sid", a_id);
				a_q.bindValue(":bid", a_isbn[i]);
				if (!::exec(a_q)) return;
			}
		}
	}
	QDialog::accept();
}
