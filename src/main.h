#ifndef MAIN_H
#define MAIN_H
#include <QSqlQuery>
#include <QString>
#include <QBrush>

void sqlError(QSqlQuery query);
bool exec(QSqlQuery &q);
bool exec(QString query, QSqlQuery q);
bool exec_first(QSqlQuery *q);
QString escape(QString par);
void load_settings();

extern QString APP_NAME;
extern QString ORG_NAME;
extern QString APP_VERS;
extern QString ORG_DOMAIN;

extern short DB_PORT;
extern QString DB_IP;
extern QString DB_USER;
extern QString DB_PASSWORD;
extern QString DB_NAME;
extern QString PDF_VIEWER;
extern QString DOC_FILE;

extern QBrush STUDENT_FREE;
#endif
