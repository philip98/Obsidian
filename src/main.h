#ifndef MAIN_H
#define MAIN_H
#include <QSqlQuery>
#include <QString>

void sqlError(QSqlQuery query);
bool exec(QSqlQuery q);
bool exec(QString query, QSqlQuery q);
bool exec_first(QSqlQuery *q);
QString escape(QString par);
#endif
