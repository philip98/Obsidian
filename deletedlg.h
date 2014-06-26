#ifndef DELETEDLG_H
#define DELETEDLG_H

#include "basedlg.h"

/*!
 * \brief Rückgabe-Dialog
 */
class DeleteDialog : public BaseDialog {
	Q_OBJECT
public:
	DeleteDialog(bool endOfTerm, QWidget *parent = 0, QSqlRecord record = QSqlRecord());

private:
	void m_alignComponents();
	void m_setInitialValues(QSqlRecord record);

public slots:
	void accept();
};

#endif // DELETEDLG_H
