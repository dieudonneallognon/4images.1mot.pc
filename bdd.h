#ifndef BDD_H
#define BDD_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QVariant>
#include <QSqlError>
#include <QFile>
#include <QMessageBox>

bool creerBaseDeDonne();
bool connecterBaseDeDonnee();
void deconnecterBaseDeDonnee();

#endif // BDD_H
