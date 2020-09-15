#include "bdd.h"

static QSqlDatabase baseDeDonnee = QSqlDatabase::addDatabase("QSQLITE");

bool creerBaseDeDonne()
{
    baseDeDonnee.setDatabaseName("bdd.db");

    if (!baseDeDonnee.open()) return false;

        QSqlQuery requete;

        QString instruction = "CREATE TABLE Mots ("
                              "numMot INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, "
                              "mot    VARCHAR(12) UNIQUE NOT NULL"
                              ");";

        requete.prepare(instruction);
        requete.exec();

        instruction = "CREATE TABLE Images ("
                      "numImage INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, "
                      "nomImage VARCHAR UNIQUE NOT NULL"
                      ");";

        requete.prepare(instruction);
        requete.exec();

        instruction = "CREATE TABLE Parties ("
                      "numPartie    INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, "
                      "joueurPartie VARCHAR NOT NULL, "
                      "scorePartie  INTEGER NOT NULL, "
                      "tempsPartie  TIME    NOT NULL DEFAULT (CURRENT_TIME), "
                      "datePartie   DATE    NOT NULL DEFAULT (CURRENT_DATE)"
                      ");";

        requete.prepare(instruction);
        requete.exec();


        instruction = "CREATE TABLE Correspondre ("
                      "numMot   VARCHAR(12) NOT NULL "
                      "CONSTRAINT FK_Correspondre_Mots REFERENCES Mots (numMot) ON DELETE SET NULL, "
                      "numImage INTEGER NOT NULL "
                      "CONSTRAINT FK_Correspondre_Images REFERENCES Images (numImage) ON DELETE SET NULL, "
                      "CONSTRAINT PK_Correspondre PRIMARY KEY (numMot, numImage)"
                      ") WITHOUT ROWID;";

        requete.prepare(instruction);
        requete.exec();

        instruction = "CREATE TABLE Admins ("
                      "login      VARCHAR PRIMARY KEY NOT NULL, "
                      "motDePasse VARCHAR NOT NULL"
                      ") WITHOUT ROWID;";

        requete.prepare(instruction);
        requete.exec();

        instruction = "INSERT INTO Admins VALUES(:login, :motDePasse);";

        requete.prepare(instruction);
        requete.bindValue(":login", "admin");
        requete.bindValue(":motDePasse", "admin");

        requete.exec();

        return true;
}

bool connecterBaseDeDonnee()
{
    baseDeDonnee.setDatabaseName("bdd.db");
    if (!baseDeDonnee.open()) return false;
    return true;
}

