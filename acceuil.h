#ifndef ACCEUIL_H
#define ACCEUIL_H

#include <QWidget>

#include <QDesktopWidget>
#include <QResizeEvent>
#include <QMessageBox>
#include <QFileDialog>
#include <QVector>
#include <QTimer>
#include <QTime>
#include <QTextStream>
#include <QSqlQueryModel>

#include <ctime>
#include <fstream>

#include "bdd.h"
#include "motimages.h"
#include "partiejeu.h"

#define MAX 11
#define MIN 0
#define NOMBRE_JEU_SERIE 10

namespace Ui {
class Acceuil;
}

class Acceuil : public QWidget
{
    Q_OBJECT

public:
    explicit Acceuil(QWidget *parent = nullptr);
    ~Acceuil();

private slots:
    void on_pBScores_clicked();

    void on_pBAcceuil_clicked();

    void on_pBParametres_clicked();

    void on_pB_AP_Authentification_clicked();

    void on_pB_OP_AjouterMot_clicked();

    void on_pB_OP_ModifierSupprimerMot_clicked();

    void on_pB_OP_AjouterAdmin_clicked();

    void on_pB_OP_ModifierSupprimerAdmin_clicked();

    void on_pB_AA_Ajouter_clicked();

    void on_pB_R_Ok_clicked();

    void on_pBPause_clicked();

    void on_pB_AA_Retour_clicked();

    void on_pB_AA_Reinitialiser_clicked();

    void on_pB_MA_Retour_clicked();

    void on_pB_MA_Reinitialiser_clicked();

    void on_pB_MA_SupprimerAdmin_clicked();

    void on_pB_MA_ModifierAdmin_clicked();

    void on_pB_AM_ChoisirImage1_clicked();

    void on_pB_AM_ChoisirImage2_clicked();

    void on_pB_AM_ChoisirImage3_clicked();

    void on_pB_AM_ChoisirImage4_clicked();

    void on_pB_AM_Retour_clicked();

    void on_pB_AM_Reinitialiser_clicked();

    void on_pB_AM_AjouterMot_clicked();

    void on_pB_MM_Retour_clicked();

    void on_pB_MM_Reinitialiser_clicked();

    void on_cBChoixMot_currentIndexChanged(int index);

    void on_pB_MM_ChoisirImage1_clicked();

    void on_pB_MM_ChoisirImage2_clicked();

    void on_pB_MM_ChoisirImage3_clicked();

    void on_pB_MM_ChoisirImage4_clicked();

    void on_pB_MM_SupprimerMot_clicked();

    void on_pBNouvellePartie_clicked();

    void on_pB_P_ReprendrePartie_clicked();

    void on_pBLettre1_clicked();

    void on_pBLettre2_clicked();

    void on_pBLettre3_clicked();

    void on_pBLettre4_clicked();

    void on_pBLettre5_clicked();

    void on_pBLettre6_clicked();

    void on_pBLettre7_clicked();

    void on_pBLettre8_clicked();

    void on_pBLettre9_clicked();

    void on_pBLettre10_clicked();

    void on_pBLettre11_clicked();

    void on_pBLettre12_clicked();

    void on_pB_J_LettreChoisie1_clicked();

    void on_pB_J_LettreChoisie2_clicked();

    void on_pB_J_LettreChoisie3_clicked();

    void on_pB_J_LettreChoisie4_clicked();

    void on_pB_J_LettreChoisie5_clicked();

    void on_pB_J_LettreChoisie6_clicked();

    void on_pB_J_LettreChoisie7_clicked();

    void on_pB_J_LettreChoisie8_clicked();

    void actualiserTemps();

    void on_pBContinuerPartie_clicked();

    void on_stackedWidget_currentChanged(int arg1);

private:
    Ui::Acceuil *ui;
    bool premiereUtilisation();
    QString ajouterImage(QString nomImageBase);
    void mettreAJourCB();
    void reponseTrouvee();
    void afficherMot(int numMot);
    int choisirMot();

    QDir* dossierImages;
    QTimer* timer;

    MotImages* mot_AM;
    MotImages* mot_MM;
    MotImages* mot_MM_Original;
    MotImages* mot_J;
    PartieJeu* partie;

    bool motTrouve;
    int nbPartie;
    QTime tempsJeu, tempsCopie;
};

#endif // ACCEUIL_H
