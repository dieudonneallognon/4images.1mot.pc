#include "acceuil.h"
#include "ui_acceuil.h"

Acceuil::Acceuil(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Acceuil)
{
    srand(time(NULL));
    QFile fichierBaseDeDonne ("bdd.db");

    if (!fichierBaseDeDonne.exists()) creerBaseDeDonne();
    else connecterBaseDeDonnee();

    dossierImages = new QDir(QDir::homePath()+"/4_Images_1_Mot/imagesJeu/");

    if (!dossierImages->exists()) {
        dossierImages->mkpath(dossierImages->absolutePath());

        QDir dir(":/imagesJeu/images");

        QFileInfoList list = dir.entryInfoList();

        QString instruction = "INSERT INTO Mots (mot) VALUES(:mot);";
        QString instruction2 = "INSERT INTO Images (nomImage) VALUES(:nomImage);";
        QString instruction4 = "INSERT INTO Correspondre (numMot, numImage) VALUES(:numMot, :numImage)";
        QString instruction3 = "SELECT numMot FROM Mots WHERE mot = :mot;";
        QSqlQuery requete;

        for (int i = 0; i < list.size(); i++) {

            QFile image(":/imagesJeu/images/"+list.at(i).fileName());

            image.copy(dossierImages->absolutePath()+"/img"+QString::number(i+1)+"."+image.fileName().section('.', -1));

            QString mot = list.at(i).absoluteFilePath();

            mot = mot.trimmed();
            mot = mot.toUpper();

            QString autreMot = mot.replace(QRegExp("[0-9]...."), "");

            mot = autreMot.section('/', 3);


            requete.prepare(instruction);
            requete.bindValue(":mot", mot);
            requete.exec();


            requete.prepare(instruction2);
            requete.bindValue(":nomImage", "img"+QString::number(i+1)+"."+image.fileName().section('.', -1));
            requete.exec();

            unsigned int numMot;

            requete.prepare(instruction3);
            requete.bindValue(":mot", mot);
            requete.exec();
            requete.next();

            numMot = requete.value("numMot").toUInt();

            requete.prepare(instruction4);
            requete.bindValue(":numMot", numMot);
            requete.bindValue(":numImage", i+1);
            requete.exec();
        }
    }

    mot_AM = new MotImages();
    mot_MM = new MotImages();
    mot_MM_Original = new MotImages();
    mot_J = new MotImages();

    ui->setupUi(this);

    this->setFixedSize(520, 730);
    this->setGeometry(QApplication::desktop()->availableGeometry(this));
    this->move(QApplication::desktop()->width()/3, 2);

    QFile fichierSauvegarde("partie.save");
    if (!fichierSauvegarde.exists()) {
        this->ui->pBContinuerPartie->setDisabled(true);
    }
    this->ui->stackedWidget->setCurrentWidget(ui->pageAcceuil);
}

Acceuil::~Acceuil()
{
    delete ui;
}

void Acceuil::on_pBScores_clicked()
{
    this->ui->stackedWidget->setCurrentWidget(ui->pageScores);
    this->ui->pBScores->setDisabled(true);
    this->ui->pBAcceuil->setDisabled(false);
    this->ui->pBParametres->setDisabled(false);

    QSqlQueryModel *model = new QSqlQueryModel;
    model->setQuery("SELECT joueurPartie, scorePartie, tempsPartie, datePartie FROM Parties ORDER BY scorePartie DESC, tempsPartie ASC");
    model->setHeaderData(0, Qt::Horizontal, tr("Nom du Joueur"));
    model->setHeaderData(1, Qt::Horizontal, tr("Score"));
    model->setHeaderData(2, Qt::Horizontal, tr("Temps de Jeu"));
    model->setHeaderData(3, Qt::Horizontal, tr("Date de la partie"));

    ui->tVScores->setModel(model);

    ui->tVScores->show();
}

void Acceuil::on_pBAcceuil_clicked()
{
    if (this->ui->stackedWidget->currentIndex() == this->ui->stackedWidget->indexOf(ui->pageJeu))
    {
        disconnect(timer, SIGNAL(timeout()), this, SLOT(actualiserTemps()));
        partie->setDuree(tempsJeu);
        int reponse = QMessageBox::question(this, "Sauvegarde de la partie", "Voulez-vous sauvegarder la partie en cours ?");
        if (reponse == QMessageBox::Yes) {
            std::ofstream monFlux("partie.save");
            if (monFlux) {
                monFlux << QString::number(partie->score()).toStdString()+"\n";
                monFlux << partie->duree().toString("hh:mm:ss").toStdString()+"\n";
                for (int i = 0; i <= partie->score(); i++) {
                    monFlux << partie->numMots(i);
                    monFlux << "\n";
                }
                monFlux.close();
            }
        }
    }
    if (this->ui->stackedWidget->currentIndex() == this->ui->stackedWidget->indexOf(ui->pagePause))
    {
        disconnect(timer, SIGNAL(timeout()), this, SLOT(actualiserTemps()));
        partie->setDuree(tempsJeu);
        int reponse = QMessageBox::question(this, "Partie en cours", "Voulez-vous sauvegarder la partie en cours ?");
        if (reponse == QMessageBox::Yes) {
            std::ofstream monFlux("partie.save");
            if (monFlux) {
                monFlux << QString::number(partie->score()).toStdString()+"\n";
                monFlux << partie->duree().toString("hh:mm:ss").toStdString()+"\n";
                for (int i = 0; i <= partie->score(); i++) {
                    monFlux << partie->numMots(i);
                    monFlux << "\n";
                }
                monFlux.close();
            }
        }
    }
    else if (this->ui->stackedWidget->currentIndex() == this->ui->stackedWidget->indexOf(ui->pageAccesParametres)) {
        this->ui->lE_AP_Mdp->clear();
        this->ui->lE_AP_Login->clear();
    }
    else if (this->ui->stackedWidget->currentIndex() == this->ui->stackedWidget->indexOf(ui->pageAjoutAdmin)) {
        this->ui->lE_AA_Mdp->clear();
        this->ui->lE_AA_Login->clear();
    }
    else if (this->ui->stackedWidget->currentIndex() == this->ui->stackedWidget->indexOf(ui->pageModifierSupprimerAdmin)) {
        this->ui->lE_MA_Login->clear();
        this->ui->lE_MA_Mdp->clear();
        this->ui->lE_MA_MdpConfirmation->clear();
        this->ui->lE_MA_LoginNv->clear();
        this->ui->lE_MA_MdpNv->clear();
    }
    else if (this->ui->stackedWidget->currentIndex() == this->ui->stackedWidget->indexOf(ui->pageAjouterMot)) {
        this->ui->l_AM_Image1->clear();
        this->ui->l_AM_Image2->clear();
        this->ui->l_AM_Image3->clear();
        this->ui->l_AM_Image4->clear();

        mot_AM->setImage(1, "");
        mot_AM->setImage(2, "");
        mot_AM->setImage(3, "");
        mot_AM->setImage(4, "");

        mot_AM->setTag("");

        this->ui->lE_AM_TagAssocie->clear();
        this->ui->l_AM_TagExistant->hide();
    }
    else if (this->ui->stackedWidget->currentIndex() == this->ui->stackedWidget->indexOf(ui->pageModifier_SupprimerMot)) {
        this->ui->cBChoixMot->setCurrentIndex(0);
        this->ui->l_MM_Image1->clear();
        this->ui->l_MM_Image2->clear();
        this->ui->l_MM_Image3->clear();
        this->ui->l_MM_Image4->clear();

        mot_MM->setImage(1, "");
        mot_MM->setImage(2, "");
        mot_MM->setImage(3, "");
        mot_MM->setImage(4, "");
        mot_MM->setTag("");

        this->ui->l_MM_TagExistant->hide();
    }
    this->ui->stackedWidget->setCurrentWidget(ui->pageAcceuil);
    this->ui->pBScores->setDisabled(false);
    this->ui->pBParametres->setDisabled(false);
}

void Acceuil::on_pBParametres_clicked()
{
    this->ui->stackedWidget->setCurrentWidget(ui->pageAccesParametres);
    this->ui->pBScores->setDisabled(false);
    this->ui->pBAcceuil->setDisabled(false);
    this->ui->pBParametres->setDisabled(true);
    this->ui->l_AP_DonneesErronees->hide();
}

void Acceuil::on_pB_AP_Authentification_clicked()
{
    if (!ui->lE_AP_Login->text().trimmed().isEmpty() &&
            !ui->lE_AP_Mdp->text().trimmed().isEmpty()) {

        QSqlQuery requete;

        QString instruction = "SELECT * FROM Admins WHERE login = :login";

        requete.prepare(instruction);
        requete.bindValue(":login", ui->lE_AP_Login->text());
        requete.exec();

        requete.next();

        if (ui->lE_AP_Login->text().compare(requete.value("login").toString()) == 0 &&
                ui->lE_AP_Mdp->text().compare(requete.value("motDePasse").toString()) == 0) {

            this->ui->stackedWidget->setCurrentWidget(ui->pageOptionsParametres);
            this->ui->lE_AP_Mdp->clear();
            this->ui->lE_AP_Login->clear();
        }
        else ui->l_AP_DonneesErronees->setText("<html><head/><body><p align=\"center\"><span style=\" font-size:7pt; font-weight:600; color:#e56e5e;\">Login ou Mot de passe incorrect !</span></p></body></html>");
    }
    else {
        ui->l_AP_DonneesErronees->setText("<html><head/><body><p align=\"center\"><span style=\" font-size:7pt; font-weight:600; color:#e56e5e;\">Veuillez remplir tous les champs !</span></p></body></html>");
    }
    ui->l_AP_DonneesErronees->show();
}

void Acceuil::on_pB_OP_AjouterMot_clicked()
{
    this->ui->stackedWidget->setCurrentWidget(ui->pageAjouterMot);
    this->ui->l_AM_TagExistant->hide();
    this->ui->lE_AM_TagAssocie->setText("Tag Mot");
}

void Acceuil::on_pB_OP_ModifierSupprimerMot_clicked()
{
    this->mettreAJourCB();
    this->ui->l_MM_TagExistant->hide();
    this->ui->stackedWidget->setCurrentWidget(ui->pageModifier_SupprimerMot);
}

void Acceuil::on_pB_OP_AjouterAdmin_clicked()
{
    this->ui->stackedWidget->setCurrentWidget(ui->pageAjoutAdmin);
    this->ui->l_AA_LoginExistant->hide();
}
void Acceuil::on_pB_OP_ModifierSupprimerAdmin_clicked()
{
    this->ui->stackedWidget->setCurrentWidget(ui->pageModifierSupprimerAdmin);
    this->ui->l_MA_DonneesErronees->hide();
}

void Acceuil::on_pB_AA_Ajouter_clicked()
{
    if (!ui->lE_AA_Login->text().trimmed().isEmpty() &&
            !ui->lE_AA_Mdp->text().trimmed().isEmpty()) {

        QString login, mdp = "";

        QSqlQuery requete;

        QString instruction = "SELECT COUNT() AS nbr FROM Admins WHERE login = :login;";

        requete.prepare(instruction);
        requete.bindValue(":login", ui->lE_AA_Login->text());
        requete.exec();

        requete.next();

        if (requete.value("nbr").toInt() == 0) {
            instruction = "INSERT INTO Admins VALUES(:login, :mdp);";
            requete.prepare(instruction);
            requete.bindValue(":login", ui->lE_AA_Login->text());
            requete.bindValue(":mdp", ui->lE_AA_Mdp->text());
            requete.exec();

            ui->l_AA_LoginExistant->setText("<html><head/><body><p align=\"center\"><span style=\" font-size:7pt; font-weight:600; color:#428bca;\">Données ajoutées !</span></p></body></html>");
        }

        else ui->l_AA_LoginExistant->setText("<html><head/><body><p align=\"center\"><span style=\" font-size:7pt; font-weight:600; color:#e56e5e;\">Login existant !</span></p></body></html>");
    }
    else {
        ui->l_AA_LoginExistant->setText("<html><head/><body><p align=\"center\"><span style=\" font-size:7pt; font-weight:600; color:#e56e5e;\">Veuillez remplir tous les champs !</span></p></body></html>");
    }
    ui->l_AA_LoginExistant->show();
}

void Acceuil::on_pB_R_Ok_clicked()
{
    if (!ui->lE_R_nom->text().trimmed().isEmpty()) {
        QString instruction = "INSERT INTO Parties (joueurPartie, scorePartie, tempsPartie) "
                              "VALUES (:joueurPartie, :scorePartie, :tempsPartie);";
        QSqlQuery requete;
        requete.prepare(instruction);
        requete.bindValue(":joueurPartie", ui->lE_R_nom->text().trimmed());
        requete.bindValue(":scorePartie", partie->score());
        requete.bindValue(":tempsPartie", partie->duree().toString("hh:mm:ss"));
        requete.exec();

        this->ui->stackedWidget->setCurrentWidget(ui->pageAcceuil);
    }
    ui->lE_R_nom->setStyleSheet("border: 2px solid #e56e5e; border-radius: 4px; padding-left: 5px;");
}

void Acceuil::on_pBPause_clicked()
{
    this->ui->stackedWidget->setCurrentWidget(ui->pagePause);
    this->ui->l_P_Score->setText(ui->l_J_Score->text());
    this->ui->l_P_Temps->setText(ui->l_J_Temps->text());
    partie->setDuree(tempsJeu);
}

void Acceuil::on_pB_AA_Retour_clicked()
{
    this->ui->stackedWidget->setCurrentWidget(ui->pageOptionsParametres);
    this->ui->lE_AA_Mdp->clear();
    this->ui->lE_AA_Login->clear();
}

void Acceuil::on_pB_AA_Reinitialiser_clicked()
{
    this->ui->lE_AA_Login->clear();
    this->ui->lE_AA_Mdp->clear();
}

void Acceuil::on_pB_MA_Retour_clicked()
{
    this->ui->stackedWidget->setCurrentWidget(ui->pageOptionsParametres);
    this->ui->lE_MA_Login->clear();
    this->ui->lE_MA_Mdp->clear();
    this->ui->lE_MA_MdpConfirmation->clear();
    this->ui->lE_MA_LoginNv->clear();
    this->ui->lE_MA_MdpNv->clear();
}

void Acceuil::on_pB_MA_Reinitialiser_clicked()
{
    this->ui->lE_MA_Login->clear();
    this->ui->lE_MA_Mdp->clear();
    this->ui->lE_MA_MdpConfirmation->clear();
    this->ui->lE_MA_LoginNv->clear();
    this->ui->lE_MA_MdpNv->clear();
}

void Acceuil::on_pB_MA_SupprimerAdmin_clicked()
{
    if (!ui->lE_MA_Login->text().trimmed().isEmpty() &&
            !ui->lE_MA_Mdp->text().trimmed().isEmpty() &&
            !ui->lE_MA_MdpConfirmation->text().trimmed().isEmpty()) {

        if (ui->lE_MA_Mdp->text().compare(ui->lE_MA_MdpConfirmation->text()) == 0) {
            QSqlQuery requete;

            QString instruction = "SELECT * FROM Admins WHERE login = :login;";

            requete.prepare(instruction);
            requete.bindValue(":login", ui->lE_MA_Login->text());
            requete.exec();

            requete.next();

            if (ui->lE_MA_Login->text().compare(requete.value("login").toString()) == 0 &&
                    ui->lE_MA_Mdp->text().compare(requete.value("motDePasse").toString()) == 0) {

                instruction = "DELETE FROM Admins WHERE login = :login;";
                requete.prepare(instruction);
                requete.bindValue(":login", ui->lE_MA_Login->text());
                requete.exec();

                this->ui->lE_MA_Login->clear();
                this->ui->lE_MA_Mdp->clear();
                this->ui->lE_MA_MdpConfirmation->clear();
                this->ui->lE_MA_LoginNv->clear();
                this->ui->lE_MA_MdpNv->clear();

                ui->l_MA_DonneesErronees->setText("<html><head/><body><p align=\"center\"><span style=\" font-size:7pt; font-weight:600; color:#428bca;\">Administrateur supprimé !</span></p></body></html>");
            }
            else ui->l_MA_DonneesErronees->setText("<html><head/><body><p align=\"center\"><span style=\" font-size:7pt; font-weight:600; color:#e56e5e;\">Login ou Mot de Passe incorrect !</span></p></body></html>");
        }
        else ui->l_MA_DonneesErronees->setText("<html><head/><body><p align=\"center\"><span style=\" font-size:7pt; font-weight:600; color:#e56e5e;\">Veuillez bien confirmer le Mot de Passe !</span></p></body></html>");
    }
    else ui->l_MA_DonneesErronees->setText("<html><head/><body><p align=\"center\"><span style=\" font-size:7pt; font-weight:600; color:#e56e5e;\">Veuillez remplir tous les champs de base!</span></p></body></html>");
    ui->l_MA_DonneesErronees->show();
}

void Acceuil::on_pB_MA_ModifierAdmin_clicked()
{
    if (!ui->lE_MA_Login->text().trimmed().isEmpty() &&
            !ui->lE_MA_Mdp->text().trimmed().isEmpty() &&
            !ui->lE_MA_MdpConfirmation->text().trimmed().isEmpty() &&
            !ui->lE_MA_LoginNv->text().trimmed().isEmpty() &&
            !ui->lE_MA_MdpNv->text().trimmed().isEmpty()) {

        if (ui->lE_MA_Mdp->text().compare(ui->lE_MA_MdpConfirmation->text()) == 0) {
            QSqlQuery requete;

            QString instruction = "SELECT * FROM Admins WHERE login = :login;";

            requete.prepare(instruction);
            requete.bindValue(":login", ui->lE_MA_Login->text());
            requete.exec();

            requete.next();

            if (ui->lE_MA_Login->text().compare(requete.value("login").toString()) == 0 &&
                    ui->lE_MA_Mdp->text().compare(requete.value("motDePasse").toString()) == 0) {

                instruction = "SELECT * FROM Admins WHERE login != :login;";
                requete.prepare(instruction);
                requete.bindValue(":login", ui->lE_MA_Login->text());
                requete.exec();

                bool loginExiste = false;

                while (requete.next() && !loginExiste) {
                    if (ui->lE_MA_LoginNv->text().compare(requete.value("login").toString()) == 0)
                        loginExiste = true;
                }

                if (!loginExiste) {
                    instruction = "UPDATE Admins SET login = :loginNv, motDePasse = :mdpNv WHERE login = :login;";
                    requete.prepare(instruction);
                    requete.bindValue(":loginNv", ui->lE_MA_LoginNv->text());
                    requete.bindValue(":mdpNv", ui->lE_MA_MdpNv->text());
                    requete.bindValue(":login", ui->lE_MA_Login->text());
                    requete.exec();

                    this->ui->lE_MA_Login->clear();
                    this->ui->lE_MA_Mdp->clear();
                    this->ui->lE_MA_MdpConfirmation->clear();
                    this->ui->lE_MA_LoginNv->clear();
                    this->ui->lE_MA_MdpNv->clear();
                    ui->l_MA_DonneesErronees->setText("<html><head/><body><p align=\"center\"><span style=\" font-size:7pt; font-weight:600; color:#428bca;\">Données du compte modifiées !</span></p></body></html>");
                }
                else ui->l_MA_DonneesErronees->setText("<html><head/><body><p align=\"center\"><span style=\" font-size:7pt; font-weight:600; color:#e56e5e;\">Nouveau Login existant !</span></p></body></html>");
            }
            else ui->l_MA_DonneesErronees->setText("<html><head/><body><p align=\"center\"><span style=\" font-size:7pt; font-weight:600; color:#e56e5e;\">Login ou Mot de Passe du compte à modifier incorrect !</span></p></body></html>");
        }
        else ui->l_MA_DonneesErronees->setText("<html><head/><body><p align=\"center\"><span style=\" font-size:7pt; font-weight:600; color:#e56e5e;\">Veuillez bien confirmer le Mot de Passe !</span></p></body></html>");
    }
    else ui->l_MA_DonneesErronees->setText("<html><head/><body><p align=\"center\"><span style=\" font-size:7pt; font-weight:600; color:#e56e5e;\">Veuillez remplir tous les champs !</span></p></body></html>");
    ui->l_MA_DonneesErronees->show();
}

void Acceuil::on_pB_AM_ChoisirImage1_clicked()
{
    QString cheminImage = "";
    QString filtre = "Toutes (*.bmp *.jpeg *.jpg *.png) ;; "
                     "BitMap (*.bmp) ;; "
                     "JPEG (*.jpeg) ;; "
                     "JPG (*.jpg) ;; "
                     "PNG (*.png)";

    cheminImage = QFileDialog::getOpenFileName(this, "Choix de l'image", QDir::homePath()+"/Pictures", filtre);

    if (!cheminImage.trimmed().isEmpty()) {

        QImage image1(cheminImage), image2(mot_AM->image(2)), image3(mot_AM->image(3)), image4(mot_AM->image(4));

        if ((image1 != image2) && (image1 != image3) && (image1 != image4)) {

            /*if((cheminImage != mot_AM->image(2)) &&
                (cheminImage != mot_AM->image(3)) &&
                (cheminImage != mot_AM->image(4))) {*/

            mot_AM->setImage(1, cheminImage);
            ui->l_AM_Image1->setPixmap(cheminImage);
            ui->l_AM_Image1->setScaledContents(true);

            QPixmap image (cheminImage);
            if (image.size().width() > 211 || image.size().height() > 172)
                QMessageBox::information(this, "Affichage de l'image", "Choisissez une image de taille 211x172 "
                                                                       "pour un meilleur affichage !");
        }
        else QMessageBox::critical(this, "Images identiques", "Vous ne pouvez pas choisir les mêmes images "
                                                              "pour un même mot !");
    }
}

void Acceuil::on_pB_AM_ChoisirImage2_clicked()
{
    QString cheminImage = "";
    QString filtre = "Toutes (*.bmp *.jpeg *.jpg *.png) ;; "
                     "BitMap (*.bmp) ;; "
                     "JPEG (*.jpeg) ;; "
                     "JPG (*.jpg) ;; "
                     "PNG (*.png)";

    cheminImage = QFileDialog::getOpenFileName(this, "Choix de l'image", QDir::homePath()+"/Pictures", filtre);

    if (!cheminImage.trimmed().isEmpty()) {

        QImage image1(mot_AM->image(1)), image2(cheminImage), image3(mot_AM->image(3)), image4(mot_AM->image(4));

        if ((image2 != image1) && (image2 != image3) && (image2 != image4)) {

            mot_AM->setImage(2, cheminImage);
            ui->l_AM_Image2->setPixmap(cheminImage);
            ui->l_AM_Image2->setScaledContents(true);

            QPixmap image (cheminImage);
            if (image.size().width() > 211 || image.size().height() > 172)
                QMessageBox::information(this, "Affichage de l'image", "Choisissez une image de taille 211x172 "
                                                                       "pour un meilleur affichage !");
        }
        else QMessageBox::critical(this, "Images identiques", "Vous ne pouvez pas choisir les mêmes images "
                                                              "pour un même mot !");
    }
}

void Acceuil::on_pB_AM_ChoisirImage3_clicked()
{
    QString cheminImage = "";
    QString filtre = "Toutes (*.bmp *.jpeg *.jpg *.png) ;; "
                     "BitMap (*.bmp) ;; "
                     "JPEG (*.jpeg) ;; "
                     "JPG (*.jpg) ;; "
                     "PNG (*.png)";

    cheminImage = QFileDialog::getOpenFileName(this, "Choix de l'image", QDir::homePath()+"/Pictures", filtre);

    if (!cheminImage.trimmed().isEmpty()) {

        QImage image1(mot_AM->image(1)), image2(mot_AM->image(2)), image3(cheminImage), image4(mot_AM->image(4));

        if ((image3 != image1) && (image3 != image2) && (image3 != image4)) {

            mot_AM->setImage(3, cheminImage);
            ui->l_AM_Image3->setPixmap(cheminImage);
            ui->l_AM_Image3->setScaledContents(true);

            QPixmap image (cheminImage);
            if (image.size().width() > 211 || image.size().height() > 172)
                QMessageBox::information(this, "Affichage de l'image", "Choisissez une image de taille 211x172 "
                                                                       "pour un meilleur affichage !");
        }
        else QMessageBox::critical(this, "Images identiques", "Vous ne pouvez pas choisir les mêmes images "
                                                              "pour un même mot !");
    }
}

void Acceuil::on_pB_AM_ChoisirImage4_clicked()
{
    QString cheminImage = "";
    QString filtre = "Toutes (*.bmp *.jpeg *.jpg *.png) ;; "
                     "BitMap (*.bmp) ;; "
                     "JPEG (*.jpeg) ;; "
                     "JPG (*.jpg) ;; "
                     "PNG (*.png)";

    cheminImage = QFileDialog::getOpenFileName(this, "Choix de l'image", QDir::homePath()+"/Pictures", filtre);

    if (!cheminImage.trimmed().isEmpty()) {

        QImage image1(mot_AM->image(1)), image2(mot_AM->image(2)), image3(mot_AM->image(3)), image4(cheminImage);

        if ((image4 != image1) && (image4 != image2) && (image4 != image3)) {

            mot_AM->setImage(4, cheminImage);
            ui->l_AM_Image4->setPixmap(cheminImage);
            ui->l_AM_Image4->setScaledContents(true);

            QPixmap image (cheminImage);
            if (image.size().width() > 211 || image.size().height() > 172)
                QMessageBox::information(this, "Affichage de l'image", "Choisissez une image de taille 211x172 "
                                                                       "pour un meilleur affichage !");
        }
        else QMessageBox::critical(this, "Images identiques", "Vous ne pouvez pas choisir les mêmes images "
                                                              "pour un même mot !");
    }
}

void Acceuil::on_pB_AM_Retour_clicked()
{
    this->ui->stackedWidget->setCurrentWidget(ui->pageOptionsParametres);

    this->ui->l_AM_Image1->clear();
    this->ui->l_AM_Image2->clear();
    this->ui->l_AM_Image3->clear();
    this->ui->l_AM_Image4->clear();

    mot_AM->setImage(1, "");
    mot_AM->setImage(2, "");
    mot_AM->setImage(3, "");
    mot_AM->setImage(4, "");

    mot_AM->setTag("");

    this->ui->lE_AM_TagAssocie->clear();
    this->ui->l_AM_TagExistant->hide();
}

void Acceuil::on_pB_AM_Reinitialiser_clicked()
{
    this->ui->l_AM_Image1->clear();
    this->ui->l_AM_Image2->clear();
    this->ui->l_AM_Image3->clear();
    this->ui->l_AM_Image4->clear();

    mot_AM->setImage(1, "");
    mot_AM->setImage(2, "");
    mot_AM->setImage(3, "");
    mot_AM->setImage(4, "");

    mot_AM->setTag("");

    this->ui->lE_AM_TagAssocie->clear();
    this->ui->l_AM_TagExistant->hide();
    this->ui->lE_AM_TagAssocie->setText("Tag Mot");
}

QString Acceuil::ajouterImage(QString nomImageBase)
{
    QString fichierImage = dossierImages->absolutePath()+"/";

    QImage imageBase(nomImageBase);

    QString instruction = "SELECT numImage FROM Images ORDER BY numImage DESC;";
    QSqlQuery requete;
    requete.prepare(instruction);
    requete.exec();
    requete.next();
    QString nomImageNv = "img"+QString::number(requete.value("numImage").toUInt()+1)+"."+nomImageBase.section('.', -1);
    QString nomImageBDD;

    instruction = "SELECT nomImage FROM Images;";
    requete.prepare(instruction);
    requete.exec();

    bool imageExiste = false;

    while (requete.next() && !imageExiste) {
        nomImageBDD = requete.value("nomImage").toString();
        QImage imageBDD(fichierImage+nomImageBDD);
        if (imageBase == imageBDD) imageExiste = true;
    }

    if (!imageExiste) {
        QFile::copy(nomImageBase, fichierImage+nomImageNv);
    }
    else {
        nomImageNv =nomImageBDD;
    }
    return nomImageNv;
}

void Acceuil::on_pB_AM_AjouterMot_clicked()
{
    mot_AM->setTag(this->ui->lE_AM_TagAssocie->text());
    if(mot_AM->complet() && mot_AM->tag().count() >= 3) {

        QString instruction = "SELECT COUNT() AS nbr FROM Mots WHERE mot = :mot;";

        QSqlQuery requete;
        requete.prepare(instruction);
        requete.bindValue(":mot", this->mot_AM->tag());
        requete.exec();
        requete.next();

        if (requete.value("nbr").toInt() == 0) {

            instruction = "INSERT INTO Mots (mot) VALUES(:mot);";
            requete.prepare(instruction);
            requete.bindValue(":mot", this->mot_AM->tag());
            requete.exec();


            instruction = "SELECT numMot FROM Mots WHERE mot = :mot;";
            requete.prepare(instruction);
            requete.bindValue(":mot", this->mot_AM->tag());
            requete.exec();
            requete.next();

            unsigned int numMot = requete.value("numMot").toUInt();

            instruction = "INSERT INTO Images (nomImage) VALUES(:nomImage);";
            QString instruction2 = "SELECT numImage FROM Images WHERE nomImage = :nomImage;";
            QString instruction3 = "INSERT INTO Correspondre (numMot, numImage) VALUES(:numMot, :numImage);";

            for (unsigned int i = 1; i <= 4; i++) {
                mot_AM->setImage(i, ajouterImage(mot_AM->image(i)));
                requete.prepare(instruction);
                requete.bindValue(":nomImage", mot_AM->image(i));
                requete.exec();

                requete.prepare(instruction2);
                requete.bindValue(":nomImage", mot_AM->image(i));
                requete.exec();
                requete.next();

                unsigned int numImage = requete.value("numImage").toUInt();

                requete.prepare(instruction3);
                requete.bindValue(":numMot", numMot);
                requete.bindValue(":numImage", numImage);
                requete.exec();
            }
            this->ui->l_AM_TagExistant->setText("<html><head/><body><p align=\"center\"><span style=\" font-size:12pt; font-weight:600; color:#428bca;\">Mot ajouté !</span></p></body></html>");
            ui->l_AM_Image1->clear();
            ui->l_AM_Image2->clear();
            ui->l_AM_Image3->clear();
            ui->l_AM_Image4->clear();
            ui->lE_AM_TagAssocie->clear();
            this->ui->lE_AM_TagAssocie->setText("Tag Mot");
        }
        else this->ui->l_AM_TagExistant->setText("<html><head/><body><p align=\"center\"><span style=\" font-size:12pt; font-weight:600; color:#901313;\">Tag Existant !</span></p></body></html>");
    }
    else this->ui->l_AM_TagExistant->setText("<html><head/><body><p align=\"center\"><span style=\" font-size:7pt; font-weight:600; color:#901313;\">Champs manquants ou mot moins de trois lettre !</span></p></body></html>");
    ui->l_AM_TagExistant->show();
}

void Acceuil::mettreAJourCB()
{
    this->ui->cBChoixMot->clear();
    this->ui->cBChoixMot->addItem("---");

    QString instruction = "SELECT mot FROM Mots ORDER BY mot ASC;";
    QSqlQuery requete;
    requete.prepare(instruction);
    requete.exec();

    while (requete.next()) {
        ui->cBChoixMot->addItem(requete.value("mot").toString());
    }
}

void Acceuil::on_pB_MM_Retour_clicked()
{
    this->ui->stackedWidget->setCurrentWidget(ui->pageOptionsParametres);

    this->mettreAJourCB();

    this->ui->l_MM_Image1->clear();
    this->ui->l_MM_Image2->clear();
    this->ui->l_MM_Image3->clear();
    this->ui->l_MM_Image4->clear();

    mot_MM->setImage(1, "");
    mot_MM->setImage(2, "");
    mot_MM->setImage(3, "");
    mot_MM->setImage(4, "");
    mot_MM->setTag("");

    this->ui->l_MM_TagExistant->hide();
}

void Acceuil::on_pB_MM_Reinitialiser_clicked()
{
    this->mettreAJourCB();

    this->ui->l_MM_Image1->clear();
    this->ui->l_MM_Image2->clear();
    this->ui->l_MM_Image3->clear();
    this->ui->l_MM_Image4->clear();

    mot_MM->setImage(1, "");
    mot_MM->setImage(2, "");
    mot_MM->setImage(3, "");
    mot_MM->setImage(4, "");
    mot_MM->setTag("");

    this->ui->l_MM_TagExistant->hide();
}

void Acceuil::on_cBChoixMot_currentIndexChanged(int index)
{
    if (ui->cBChoixMot->itemText(index) == "---")
    {
        ui->l_MM_Image1->clear();
        ui->l_MM_Image2->clear();
        ui->l_MM_Image3->clear();
        ui->l_MM_Image4->clear();

        mot_MM->aZero();
    }
    else {
        mot_MM->setTag(ui->cBChoixMot->itemText(index));

        QString instruction = "SELECT * FROM Mots INNER JOIN Correspondre INNER JOIN Images "
                              "ON Mots.numMot = Correspondre.numMot AND Correspondre.numImage = Images.numImage "
                              "WHERE Mots.mot = :mot;";
        QSqlQuery requete;

        requete.prepare(instruction);
        requete.bindValue(":mot", mot_MM->tag());
        requete.exec();

        unsigned int i = 1;

        while (requete.next()) mot_MM->setImage(i++, requete.value("nomImage").toString());
        ui->l_MM_Image1->setScaledContents(true);
        ui->l_MM_Image2->setScaledContents(true);
        ui->l_MM_Image3->setScaledContents(true);
        ui->l_MM_Image4->setScaledContents(true);

        ui->l_MM_Image1->setPixmap(dossierImages->absolutePath()+"/"+mot_MM->image(1));
        ui->l_MM_Image2->setPixmap(dossierImages->absolutePath()+"/"+mot_MM->image(2));
        ui->l_MM_Image3->setPixmap(dossierImages->absolutePath()+"/"+mot_MM->image(3));
        ui->l_MM_Image4->setPixmap(dossierImages->absolutePath()+"/"+mot_MM->image(4));
    }
    ui->l_MM_TagExistant->hide();
}

void Acceuil::on_pB_MM_ChoisirImage1_clicked()
{
    QString cheminImage = "";
    QString filtre = "Toutes (*.bmp *.jpeg *.jpg *.png) ;; "
                     "BitMap (*.bmp) ;; "
                     "JPEG (*.jpeg) ;; "
                     "JPG (*.jpg) ;; "
                     "PNG (*.png)";

    cheminImage = QFileDialog::getOpenFileName(this, "Choix de l'image", QDir::homePath()+"/Pictures", filtre);

    if (!cheminImage.trimmed().isEmpty()) {

        QImage image1(cheminImage), image2(mot_MM->image(2)), image3(mot_MM->image(3)), image4(mot_MM->image(4));

        if ((image1 != image2) && (image1 != image3) && (image1 != image4)) {

            mot_MM->setImage(1, cheminImage);
            ui->l_MM_Image1->setPixmap(cheminImage);
            ui->l_MM_Image1->setScaledContents(true);

            QPixmap image (cheminImage);
            if (image.size().width() > 211 || image.size().height() > 172)
                QMessageBox::information(this, "Affichage de l'image", "Choisissez une image de taille 211x172 "
                                                                       "pour un meilleur affichage !");
        }
        else QMessageBox::critical(this, "Images identiques", "Vous ne pouvez pas choisir les mêmes images "
                                                              "pour un même mot !");
    }
}

void Acceuil::on_pB_MM_ChoisirImage2_clicked()
{
    QString cheminImage = "";
    QString filtre = "Toutes (*.bmp *.jpeg *.jpg *.png) ;; "
                     "BitMap (*.bmp) ;; "
                     "JPEG (*.jpeg) ;; "
                     "JPG (*.jpg) ;; "
                     "PNG (*.png)";

    cheminImage = QFileDialog::getOpenFileName(this, "Choix de l'image", QDir::homePath()+"/Pictures", filtre);

    if (!cheminImage.trimmed().isEmpty()) {

        QImage image1(mot_MM->image(1)), image2(cheminImage), image3(mot_MM->image(3)), image4(mot_MM->image(4));

        if ((image2 != image1) && (image2 != image3) && (image2 != image4)) {

            mot_MM->setImage(2, cheminImage);
            ui->l_MM_Image2->setPixmap(cheminImage);
            ui->l_MM_Image2->setScaledContents(true);

            QPixmap image (cheminImage);
            if (image.size().width() > 211 || image.size().height() > 172)
                QMessageBox::information(this, "Affichage de l'image", "Choisissez une image de taille 211x172 "
                                                                       "pour un meilleur affichage !");
        }
        else QMessageBox::critical(this, "Images identiques", "Vous ne pouvez pas choisir les mêmes images "
                                                              "pour un même mot !");
    }
}

void Acceuil::on_pB_MM_ChoisirImage3_clicked()
{
    QString cheminImage = "";
    QString filtre = "Toutes (*.bmp *.jpeg *.jpg *.png) ;; "
                     "BitMap (*.bmp) ;; "
                     "JPEG (*.jpeg) ;; "
                     "JPG (*.jpg) ;; "
                     "PNG (*.png)";

    cheminImage = QFileDialog::getOpenFileName(this, "Choix de l'image", QDir::homePath()+"/Pictures", filtre);

    if (!cheminImage.trimmed().isEmpty()) {

        QImage image1(mot_MM->image(1)), image2(mot_MM->image(2)), image3(cheminImage), image4(mot_MM->image(4));

        if ((image3 != image1) && (image3 != image2) && (image3 != image4)) {

            mot_MM->setImage(3, cheminImage);
            ui->l_MM_Image3->setPixmap(cheminImage);
            ui->l_MM_Image3->setScaledContents(true);

            QPixmap image (cheminImage);
            if (image.size().width() > 211 || image.size().height() > 172)
                QMessageBox::information(this, "Affichage de l'image", "Choisissez une image de taille 211x172 "
                                                                       "pour un meilleur affichage !");
        }
        else QMessageBox::critical(this, "Images identiques", "Vous ne pouvez pas choisir les mêmes images "
                                                              "pour un même mot !");
    }
}

void Acceuil::on_pB_MM_ChoisirImage4_clicked()
{
    QString cheminImage = "";
    QString filtre = "Toutes (*.bmp *.jpeg *.jpg *.png) ;; "
                     "BitMap (*.bmp) ;; "
                     "JPEG (*.jpeg) ;; "
                     "JPG (*.jpg) ;; "
                     "PNG (*.png)";

    cheminImage = QFileDialog::getOpenFileName(this, "Choix de l'image", QDir::homePath()+"/Pictures", filtre);

    if (!cheminImage.trimmed().isEmpty()) {

        QImage image1(mot_MM->image(1)), image2(mot_MM->image(2)), image3(mot_MM->image(3)), image4(cheminImage);

        if ((image4 != image1) && (image4 != image2) && (image4 != image3)) {

            mot_MM->setImage(4, cheminImage);
            ui->l_MM_Image4->setPixmap(cheminImage);
            ui->l_MM_Image4->setScaledContents(true);

            QPixmap image (cheminImage);
            if (image.size().width() > 211 || image.size().height() > 172)
                QMessageBox::information(this, "Affichage de l'image", "Choisissez une image de taille 211x172 "
                                                                       "pour un meilleur affichage !");
        }
        else QMessageBox::critical(this, "Images identiques", "Vous ne pouvez pas choisir les mêmes images "
                                                              "pour un même mot !");
    }
}

void Acceuil::on_pB_MM_SupprimerMot_clicked()
{

    QString instruction = "SELECT COUNT() AS nbr FROM Mots;";

    QSqlQuery requete;
    requete.prepare(instruction);
    requete.exec();
    requete.next();

    int nbMot = requete.value("nbr").toInt();

    if (nbMot > NOMBRE_JEU_SERIE) {
        if (mot_MM->complet()) {
            instruction = "SELECT numMot FROM Mots WHERE mot = :mot;";

            requete.prepare(instruction);
            requete.bindValue(":mot", mot_MM->tag());
            requete.exec();
            requete.next();

            unsigned int numMot = requete.value("numMot").toUInt();

            instruction = "DELETE FROM Correspondre WHERE numMot = :numMot;";
            requete.prepare(instruction);
            requete.bindValue(":numMot", numMot);
            requete.exec();

            instruction = "DELETE FROM Mots WHERE numMot = :numMot;";
            requete.prepare(instruction);
            requete.bindValue(":numMot", numMot);
            requete.exec();

            instruction = "SELECT numImage FROM Images WHERE nomImage = :nomImage;";
            QString instruction2 = "SELECT COUNT() AS nbr FROM Correspondre WHERE numImage = :numImage;";
            QString instruction3 = "DELETE FROM Images WHERE numImage = :numImage;";
            unsigned int numImage;
            for (unsigned int i = 1; i <= 4; ++i) {
                requete.prepare(instruction);
                requete.bindValue(":nomImage", mot_MM->image(i));
                requete.exec();
                requete.next();

                numImage = requete.value("numImage").toUInt();

                requete.prepare(instruction2);
                requete.bindValue(":numImage", numImage);
                requete.exec();
                requete.next();

                if (requete.value("nbr").toInt() == 0) {
                    requete.prepare(instruction3);
                    requete.bindValue(":numImage", numImage);
                    requete.exec();
                    QFile::remove(dossierImages->absolutePath()+"/"+mot_MM->image(i));
                }
            }

            ui->l_MM_TagExistant->setText("<html><head/><body><p><span style=\" font-size:12pt; font-weight:600; color:#428bca;\">Mot supprimé !</span></p></body></html>");

            this->mettreAJourCB();

            this->ui->l_MM_Image1->clear();
            this->ui->l_MM_Image2->clear();
            this->ui->l_MM_Image3->clear();
            this->ui->l_MM_Image4->clear();

            mot_MM->setImage(1, "");
            mot_MM->setImage(2, "");
            mot_MM->setImage(3, "");
            mot_MM->setImage(4, "");
            mot_MM->setTag("");

            this->ui->l_MM_TagExistant->show();
        }
        else ui->l_MM_TagExistant->setText("<html><head/><body><p><span style=\" font-size:12pt; font-weight:600; color:#901313;\">Aucun mot choisi !</span></p></body></html>");
    }
    else QMessageBox::information(this, "Nombre de mot insuffisant", "Il n'y a que "+QString::number(nbMot)+" mots dans le jeu !\n"
                                                                                                            " Ajouter-en "+QString::number(10 - nbMot)+" avant d'en supprimer");
}

void Acceuil::afficherMot(int numMot)
{
    ui->pB_J_LettreChoisie1->show();
    ui->pB_J_LettreChoisie2->show();
    ui->pB_J_LettreChoisie3->show();
    ui->pB_J_LettreChoisie4->show();
    ui->pB_J_LettreChoisie5->show();
    ui->pB_J_LettreChoisie6->show();
    ui->pB_J_LettreChoisie7->show();
    ui->pB_J_LettreChoisie8->show();

    ui->pB_J_LettreChoisie1->setText("");
    ui->pB_J_LettreChoisie2->setText("");
    ui->pB_J_LettreChoisie3->setText("");
    ui->pB_J_LettreChoisie4->setText("");
    ui->pB_J_LettreChoisie5->setText("");
    ui->pB_J_LettreChoisie6->setText("");
    ui->pB_J_LettreChoisie7->setText("");
    ui->pB_J_LettreChoisie8->setText("");

    ui->l_J_Temps->setText("<html><head/><body><p align=\"center\"><span style=\" font-size:12pt; font-weight:600; color:#f5c70e;\">"+partie->duree().toString("hh:mm:ss")+"</span></p></body></html>");
    ui->l_J_Temps->setVisible(true);
    QString instruction = "SELECT Mots.mot, Images.nomImage FROM Mots INNER JOIN Correspondre INNER JOIN Images "
                          "ON Mots.numMot = Correspondre.numMot AND Correspondre.numImage = Images.numImage "
                          "WHERE Mots.numMot = :numMot;";
    QSqlQuery requete;

    ui->l_J_Score->setText("<html><head/><body><p align=\"center\"><span style=\" font-size:12pt; font-weight:600; color:#a6cb75;\">"+QString::number(partie->score())+"</span></p></body></html>");
    ui->l_J_Score->setVisible(true);
    requete.prepare(instruction);
    requete.bindValue(":numMot", numMot);
    requete.exec();

    for (unsigned int j = 1; j <= 4 && requete.next(); j++) {
        mot_J->setTag(requete.value("mot").toString());
        mot_J->setImage(j, dossierImages->absolutePath()+"/"+requete.value("nomImage").toString());
    }

    ui->l_J_Image1->setPixmap(mot_J->image(1));
    ui->l_J_Image2->setPixmap(mot_J->image(2));
    ui->l_J_Image3->setPixmap(mot_J->image(3));
    ui->l_J_Image4->setPixmap(mot_J->image(4));

    ui->l_J_Image1->setScaledContents(true);
    ui->l_J_Image2->setScaledContents(true);
    ui->l_J_Image3->setScaledContents(true);
    ui->l_J_Image4->setScaledContents(true);

    switch (mot_J->tag().size()) {
    case 3:
        ui->pB_J_LettreChoisie8->hide();
        ui->pB_J_LettreChoisie1->hide();
        ui->pB_J_LettreChoisie7->hide();
        ui->pB_J_LettreChoisie2->hide();
        ui->pB_J_LettreChoisie6->hide();
        break;
    case 4:
        ui->pB_J_LettreChoisie8->hide();
        ui->pB_J_LettreChoisie1->hide();
        ui->pB_J_LettreChoisie7->hide();
        ui->pB_J_LettreChoisie2->hide();
        break;
    case 5:
        ui->pB_J_LettreChoisie8->hide();
        ui->pB_J_LettreChoisie1->hide();
        ui->pB_J_LettreChoisie7->hide();
        break;

    case 6:
        ui->pB_J_LettreChoisie8->hide();
        ui->pB_J_LettreChoisie1->hide();;
        break;

    case 7:
        ui->pB_J_LettreChoisie8->hide();
    }

    int listePositions[12];
    QVector<QChar> listeLettre;

    bool memeNombre;


    listePositions[0] = ((rand() % (MAX - MIN + 1)) + MIN);

    for (int j = 1; j <  12; j++) {
        do {
            memeNombre = false;
            listePositions[j] = ((rand() % (MAX - MIN + 1)) + MIN);

            for (int k = 0; k < j ; k++) {
                if (listePositions[k] == listePositions[j]) {
                    memeNombre = true;
                    break;
                }
            }
        } while (memeNombre);
    }

    for (int j = 0; j < mot_J->tag().count(); j++) {
        listeLettre.append(mot_J->tag().at(j));
    }
    for (int j = mot_J->tag().count(); j < 12; j++) {
        listeLettre.append((rand() % (90 - 65 + 1)) + 65);
    }

    ui->pBLettre1->setText(listeLettre.value(listePositions[0]));
    ui->pBLettre2->setText(listeLettre.value(listePositions[1]));
    ui->pBLettre3->setText(listeLettre.value(listePositions[2]));
    ui->pBLettre4->setText(listeLettre.value(listePositions[3]));
    ui->pBLettre5->setText(listeLettre.value(listePositions[4]));
    ui->pBLettre6->setText(listeLettre.value(listePositions[5]));
    ui->pBLettre7->setText(listeLettre.value(listePositions[6]));
    ui->pBLettre8->setText(listeLettre.value(listePositions[7]));
    ui->pBLettre9->setText(listeLettre.value(listePositions[8]));
    ui->pBLettre10->setText(listeLettre.value(listePositions[9]));
    ui->pBLettre11->setText(listeLettre.value(listePositions[10]));
    ui->pBLettre12->setText(listeLettre.value(listePositions[11]));

    tempsJeu = partie->duree();

    timer = new QTimer() ;
    connect(timer, SIGNAL(timeout()), this, SLOT(actualiserTemps()));
    timer->start(1000);
}

int Acceuil::choisirMot()
{
    QString instruction = "SELECT numMot FROM Mots ORDER BY numMot DESC";
    QSqlQuery requete;

    int numChoisi;

    requete.prepare(instruction);
    requete.exec();
    requete.next();

    int numMax = requete.value("numMot").toInt();

    instruction = "SELECT numMot FROM Mots ORDER BY numMot ASC";
    requete.prepare(instruction);
    requete.exec();
    requete.next();

    int numMin = requete.value("numMot").toInt();

    bool numOk1 = false;
    bool numOk2;

    do {

        numChoisi = ((rand() % (numMax - numMin + 1)) + numMin);

        numOk2 = true;

        instruction = "SELECT COUNT() AS nbr FROM Mots WHERE numMot = :numMot;";

        requete.prepare(instruction);
        requete.bindValue(":numMot", numChoisi);
        requete.exec();
        requete.next();

        if (requete.value("nbr").toUInt() != 0) numOk1 = true;

        for (int i = 0; i < partie->score() ; i++) {
            if (partie->numMots(i) == numChoisi) {
                numOk2 = false;
                break;
            }
        }

    } while (!numOk1 || !numOk2);

    partie->seTnumMots(numChoisi);
    return numChoisi;
}

void Acceuil::on_pBNouvellePartie_clicked()
{

    QString instruction = "SELECT COUNT() AS nbr FROM Mots;";
    QSqlQuery requete;
    requete.prepare(instruction);
    requete.exec();
    requete.next();

    unsigned int nbMot = requete.value("nbr").toUInt();

    if (nbMot >= NOMBRE_JEU_SERIE) {

        partie = new PartieJeu();

        instruction = "SELECT numMot FROM Mots ORDER BY numMot DESC";
        requete.prepare(instruction);
        requete.exec();
        requete.next();

        int numMax = requete.value("numMot").toInt();

        instruction = "SELECT numMot FROM Mots ORDER BY numMot ASC";
        requete.prepare(instruction);
        requete.exec();
        requete.next();

        int numMin = requete.value("numMot").toInt();

        int numMot = ((rand() % (numMax - numMin + 1)) + numMin);

        bool numOk = false;
        instruction = "SELECT COUNT() AS nbr FROM Mots WHERE numMot = :numMot;";

        do {
            requete.prepare(instruction);
            requete.bindValue(":numMot", numMot);
            requete.exec();
            requete.next();

            if (requete.value("nbr").toUInt() != 0) numOk = true;
            else {
                numOk = false;
                numMot++;
                if (numMot == numMax) numMot = numMin;
            }
        } while (!numOk);

        partie->seTnumMots(numMot);
        this->afficherMot(partie->numMots(partie->score()));
        this->ui->stackedWidget->setCurrentWidget(ui->pageJeu);
    }
    else QMessageBox::information(this, "Nombre de mot insufisant", "Il n'y a pas assez de mot pour lancer une partie !\n"
                                                                    "Ajoutez-en "+QString::number(10 - nbMot)+" dans les paramètres");
}

void Acceuil::on_pB_P_ReprendrePartie_clicked()
{
    this->ui->stackedWidget->setCurrentWidget(ui->pageJeu);
    this->ui->l_J_Score->setText(ui->l_P_Score->text());
    tempsJeu = partie->duree();

    this->ui->l_J_Temps->setText(ui->l_P_Temps->text());
}

void::Acceuil::reponseTrouvee()
{
    QString proposition = ui->pB_J_LettreChoisie1->text()+
            ui->pB_J_LettreChoisie2->text()+
            ui->pB_J_LettreChoisie3->text()+
            ui->pB_J_LettreChoisie4->text()+
            ui->pB_J_LettreChoisie5->text()+
            ui->pB_J_LettreChoisie6->text()+
            ui->pB_J_LettreChoisie7->text()+
            ui->pB_J_LettreChoisie8->text();

    if (proposition == mot_J->tag()) {
        disconnect(timer, SIGNAL(timeout()), this, SLOT(actualiserTemps()));
        partie->setScore(partie->score()+1);
        partie->setDuree(tempsJeu);
        QMessageBox::information(this, "Mot trouvé", "Vous avez trouvé la bonne réponse !");
        if (partie->score() != NOMBRE_JEU_SERIE) afficherMot(choisirMot());
        else {
            ui->l_R_Score->setText("<html><head/><body><p align=\"center\"><span style=\" font-size:36pt; font-weight:600; color:#e5ae23;\">"+QString::number(partie->score())+"</span></p></body></html>");
            ui->l_R_Temps->setText("<html><head/><body><p align=\"center\"><span style=\" font-size:28pt; font-weight:600; color:#e57d9c;\">"+partie->duree().toString("hh:mm:ss")+"</span></p></body></html>");
            ui->stackedWidget->setCurrentWidget(ui->pageResultat);

            ui->pBAcceuil->setDisabled(true);
            ui->pBScores->setDisabled(true);
            ui->pBParametres->setDisabled(true);
        }
    }
}

void Acceuil::on_pBLettre1_clicked()
{
    if (ui->pB_J_LettreChoisie1->isVisible() &&
            ui->pB_J_LettreChoisie1->text().trimmed().isEmpty()) {
        ui->pB_J_LettreChoisie1->setText(ui->pBLettre1->text());
        ui->pBLettre1->setText("");
    }
    else if (ui->pB_J_LettreChoisie2->isVisible() &&
             ui->pB_J_LettreChoisie2->text().trimmed().isEmpty()) {
        ui->pB_J_LettreChoisie2->setText(ui->pBLettre1->text());
        ui->pBLettre1->setText("");
    }
    else if (ui->pB_J_LettreChoisie3->isVisible() &&
             ui->pB_J_LettreChoisie3->text().trimmed().isEmpty()) {
        ui->pB_J_LettreChoisie3->setText(ui->pBLettre1->text());
        ui->pBLettre1->setText("");
    }
    else if (ui->pB_J_LettreChoisie4->isVisible() &&
             ui->pB_J_LettreChoisie4->text().trimmed().isEmpty()) {
        ui->pB_J_LettreChoisie4->setText(ui->pBLettre1->text());
        ui->pBLettre1->setText("");
    }
    else if (ui->pB_J_LettreChoisie5->isVisible() &&
             ui->pB_J_LettreChoisie5->text().trimmed().isEmpty()) {
        ui->pB_J_LettreChoisie5->setText(ui->pBLettre1->text());
        ui->pBLettre1->setText("");
    }
    else if (ui->pB_J_LettreChoisie6->isVisible() &&
             ui->pB_J_LettreChoisie6->text().trimmed().isEmpty()) {
        ui->pB_J_LettreChoisie6->setText(ui->pBLettre1->text());
        ui->pBLettre1->setText("");
    }
    else if (ui->pB_J_LettreChoisie7->isVisible() &&
             ui->pB_J_LettreChoisie7->text().trimmed().isEmpty()) {
        ui->pB_J_LettreChoisie7->setText(ui->pBLettre1->text());
        ui->pBLettre1->setText("");
    }
    else if (ui->pB_J_LettreChoisie8->isVisible() &&
             ui->pB_J_LettreChoisie8->text().trimmed().isEmpty()) {
        ui->pB_J_LettreChoisie8->setText(ui->pBLettre1->text());
        ui->pBLettre1->setText("");
    }

    reponseTrouvee();
}

void Acceuil::on_pBLettre2_clicked()
{
    if (ui->pB_J_LettreChoisie1->isVisible() &&
            ui->pB_J_LettreChoisie1->text().trimmed().isEmpty()) {
        ui->pB_J_LettreChoisie1->setText(ui->pBLettre2->text());
        ui->pBLettre2->setText("");
    }
    else if (ui->pB_J_LettreChoisie2->isVisible() &&
             ui->pB_J_LettreChoisie2->text().trimmed().isEmpty()) {
        ui->pB_J_LettreChoisie2->setText(ui->pBLettre2->text());
        ui->pBLettre2->setText("");
    }
    else if (ui->pB_J_LettreChoisie3->isVisible() &&
             ui->pB_J_LettreChoisie3->text().trimmed().isEmpty()) {
        ui->pB_J_LettreChoisie3->setText(ui->pBLettre2->text());
        ui->pBLettre2->setText("");
    }
    else if (ui->pB_J_LettreChoisie4->isVisible() &&
             ui->pB_J_LettreChoisie4->text().trimmed().isEmpty()) {
        ui->pB_J_LettreChoisie4->setText(ui->pBLettre2->text());
        ui->pBLettre2->setText("");
    }
    else if (ui->pB_J_LettreChoisie5->isVisible() &&
             ui->pB_J_LettreChoisie5->text().trimmed().isEmpty()) {
        ui->pB_J_LettreChoisie5->setText(ui->pBLettre2->text());
        ui->pBLettre2->setText("");
    }
    else if (ui->pB_J_LettreChoisie6->isVisible() &&
             ui->pB_J_LettreChoisie6->text().trimmed().isEmpty()) {
        ui->pB_J_LettreChoisie6->setText(ui->pBLettre2->text());
        ui->pBLettre2->setText("");
    }
    else if (ui->pB_J_LettreChoisie7->isVisible() &&
             ui->pB_J_LettreChoisie7->text().trimmed().isEmpty()) {
        ui->pB_J_LettreChoisie7->setText(ui->pBLettre2->text());
        ui->pBLettre2->setText("");
    }
    else if (ui->pB_J_LettreChoisie8->isVisible() &&
             ui->pB_J_LettreChoisie8->text().trimmed().isEmpty()) {
        ui->pB_J_LettreChoisie8->setText(ui->pBLettre2->text());
        ui->pBLettre2->setText("");
    }

    reponseTrouvee();
}

void Acceuil::on_pBLettre3_clicked()
{
    if (ui->pB_J_LettreChoisie1->isVisible() &&
            ui->pB_J_LettreChoisie1->text().trimmed().isEmpty()) {
        ui->pB_J_LettreChoisie1->setText(ui->pBLettre3->text());
        ui->pBLettre3->setText("");
    }
    else if (ui->pB_J_LettreChoisie2->isVisible() &&
             ui->pB_J_LettreChoisie2->text().trimmed().isEmpty()) {
        ui->pB_J_LettreChoisie2->setText(ui->pBLettre3->text());
        ui->pBLettre3->setText("");
    }
    else if (ui->pB_J_LettreChoisie3->isVisible() &&
             ui->pB_J_LettreChoisie3->text().trimmed().isEmpty()) {
        ui->pB_J_LettreChoisie3->setText(ui->pBLettre3->text());
        ui->pBLettre3->setText("");
    }
    else if (ui->pB_J_LettreChoisie4->isVisible() &&
             ui->pB_J_LettreChoisie4->text().trimmed().isEmpty()) {
        ui->pB_J_LettreChoisie4->setText(ui->pBLettre3->text());
        ui->pBLettre3->setText("");
    }
    else if (ui->pB_J_LettreChoisie5->isVisible() &&
             ui->pB_J_LettreChoisie5->text().trimmed().isEmpty()) {
        ui->pB_J_LettreChoisie5->setText(ui->pBLettre3->text());
        ui->pBLettre3->setText("");
    }
    else if (ui->pB_J_LettreChoisie6->isVisible() &&
             ui->pB_J_LettreChoisie6->text().trimmed().isEmpty()) {
        ui->pB_J_LettreChoisie6->setText(ui->pBLettre3->text());
        ui->pBLettre3->setText("");
    }
    else if (ui->pB_J_LettreChoisie7->isVisible() &&
             ui->pB_J_LettreChoisie7->text().trimmed().isEmpty()) {
        ui->pB_J_LettreChoisie7->setText(ui->pBLettre3->text());
        ui->pBLettre3->setText("");
    }
    else if (ui->pB_J_LettreChoisie8->isVisible() &&
             ui->pB_J_LettreChoisie8->text().trimmed().isEmpty()) {
        ui->pB_J_LettreChoisie8->setText(ui->pBLettre3->text());
        ui->pBLettre3->setText("");
    }

    reponseTrouvee();
}

void Acceuil::on_pBLettre4_clicked()
{
    if (ui->pB_J_LettreChoisie1->isVisible() &&
            ui->pB_J_LettreChoisie1->text().trimmed().isEmpty()) {
        ui->pB_J_LettreChoisie1->setText(ui->pBLettre4->text());
        ui->pBLettre4->setText("");
    }
    else if (ui->pB_J_LettreChoisie2->isVisible() &&
             ui->pB_J_LettreChoisie2->text().trimmed().isEmpty()) {
        ui->pB_J_LettreChoisie2->setText(ui->pBLettre4->text());
        ui->pBLettre4->setText("");
    }
    else if (ui->pB_J_LettreChoisie3->isVisible() &&
             ui->pB_J_LettreChoisie3->text().trimmed().isEmpty()) {
        ui->pB_J_LettreChoisie3->setText(ui->pBLettre4->text());
        ui->pBLettre4->setText("");
    }
    else if (ui->pB_J_LettreChoisie4->isVisible() &&
             ui->pB_J_LettreChoisie4->text().trimmed().isEmpty()) {
        ui->pB_J_LettreChoisie4->setText(ui->pBLettre4->text());
        ui->pBLettre4->setText("");
    }
    else if (ui->pB_J_LettreChoisie5->isVisible() &&
             ui->pB_J_LettreChoisie5->text().trimmed().isEmpty()) {
        ui->pB_J_LettreChoisie5->setText(ui->pBLettre4->text());
        ui->pBLettre4->setText("");
    }
    else if (ui->pB_J_LettreChoisie6->isVisible() &&
             ui->pB_J_LettreChoisie6->text().trimmed().isEmpty()) {
        ui->pB_J_LettreChoisie6->setText(ui->pBLettre4->text());
        ui->pBLettre4->setText("");
    }
    else if (ui->pB_J_LettreChoisie7->isVisible() &&
             ui->pB_J_LettreChoisie7->text().trimmed().isEmpty()) {
        ui->pB_J_LettreChoisie7->setText(ui->pBLettre4->text());
        ui->pBLettre4->setText("");
    }
    else if (ui->pB_J_LettreChoisie8->isVisible() &&
             ui->pB_J_LettreChoisie8->text().trimmed().isEmpty()) {
        ui->pB_J_LettreChoisie8->setText(ui->pBLettre4->text());
        ui->pBLettre4->setText("");
    }

    reponseTrouvee();
}

void Acceuil::on_pBLettre5_clicked()
{
    if (ui->pB_J_LettreChoisie1->isVisible() &&
            ui->pB_J_LettreChoisie1->text().trimmed().isEmpty()) {
        ui->pB_J_LettreChoisie1->setText(ui->pBLettre5->text());
        ui->pBLettre5->setText("");
    }
    else if (ui->pB_J_LettreChoisie2->isVisible() &&
             ui->pB_J_LettreChoisie2->text().trimmed().isEmpty()) {
        ui->pB_J_LettreChoisie2->setText(ui->pBLettre5->text());
        ui->pBLettre5->setText("");
    }
    else if (ui->pB_J_LettreChoisie3->isVisible() &&
             ui->pB_J_LettreChoisie3->text().trimmed().isEmpty()) {
        ui->pB_J_LettreChoisie3->setText(ui->pBLettre5->text());
        ui->pBLettre5->setText("");
    }
    else if (ui->pB_J_LettreChoisie4->isVisible() &&
             ui->pB_J_LettreChoisie4->text().trimmed().isEmpty()) {
        ui->pB_J_LettreChoisie4->setText(ui->pBLettre5->text());
        ui->pBLettre5->setText("");
    }
    else if (ui->pB_J_LettreChoisie5->isVisible() &&
             ui->pB_J_LettreChoisie5->text().trimmed().isEmpty()) {
        ui->pB_J_LettreChoisie5->setText(ui->pBLettre5->text());
        ui->pBLettre5->setText("");
    }
    else if (ui->pB_J_LettreChoisie6->isVisible() &&
             ui->pB_J_LettreChoisie6->text().trimmed().isEmpty()) {
        ui->pB_J_LettreChoisie6->setText(ui->pBLettre5->text());
        ui->pBLettre5->setText("");
    }
    else if (ui->pB_J_LettreChoisie7->isVisible() &&
             ui->pB_J_LettreChoisie7->text().trimmed().isEmpty()) {
        ui->pB_J_LettreChoisie7->setText(ui->pBLettre5->text());
        ui->pBLettre5->setText("");
    }
    else if (ui->pB_J_LettreChoisie8->isVisible() &&
             ui->pB_J_LettreChoisie8->text().trimmed().isEmpty()) {
        ui->pB_J_LettreChoisie8->setText(ui->pBLettre5->text());
        ui->pBLettre5->setText("");
    }

    reponseTrouvee();
}

void Acceuil::on_pBLettre6_clicked()
{
    if (ui->pB_J_LettreChoisie1->isVisible() &&
            ui->pB_J_LettreChoisie1->text().trimmed().isEmpty()) {
        ui->pB_J_LettreChoisie1->setText(ui->pBLettre6->text());
        ui->pBLettre6->setText("");
    }
    else if (ui->pB_J_LettreChoisie2->isVisible() &&
             ui->pB_J_LettreChoisie2->text().trimmed().isEmpty()) {
        ui->pB_J_LettreChoisie2->setText(ui->pBLettre6->text());
        ui->pBLettre6->setText("");
    }
    else if (ui->pB_J_LettreChoisie3->isVisible() &&
             ui->pB_J_LettreChoisie3->text().trimmed().isEmpty()) {
        ui->pB_J_LettreChoisie3->setText(ui->pBLettre6->text());
        ui->pBLettre6->setText("");
    }
    else if (ui->pB_J_LettreChoisie4->isVisible() &&
             ui->pB_J_LettreChoisie4->text().trimmed().isEmpty()) {
        ui->pB_J_LettreChoisie4->setText(ui->pBLettre6->text());
        ui->pBLettre6->setText("");
    }
    else if (ui->pB_J_LettreChoisie5->isVisible() &&
             ui->pB_J_LettreChoisie5->text().trimmed().isEmpty()) {
        ui->pB_J_LettreChoisie5->setText(ui->pBLettre6->text());
        ui->pBLettre6->setText("");
    }
    else if (ui->pB_J_LettreChoisie6->isVisible() &&
             ui->pB_J_LettreChoisie6->text().trimmed().isEmpty()) {
        ui->pB_J_LettreChoisie6->setText(ui->pBLettre6->text());
        ui->pBLettre6->setText("");
    }
    else if (ui->pB_J_LettreChoisie7->isVisible() &&
             ui->pB_J_LettreChoisie7->text().trimmed().isEmpty()) {
        ui->pB_J_LettreChoisie7->setText(ui->pBLettre6->text());
        ui->pBLettre6->setText("");
    }
    else if (ui->pB_J_LettreChoisie8->isVisible() &&
             ui->pB_J_LettreChoisie8->text().trimmed().isEmpty()) {
        ui->pB_J_LettreChoisie8->setText(ui->pBLettre6->text());
        ui->pBLettre6->setText("");
    }

    reponseTrouvee();
}



void Acceuil::on_pBLettre7_clicked()
{
    if (ui->pB_J_LettreChoisie1->isVisible() &&
            ui->pB_J_LettreChoisie1->text().trimmed().isEmpty()) {
        ui->pB_J_LettreChoisie1->setText(ui->pBLettre7->text());
        ui->pBLettre7->setText("");
    }
    else if (ui->pB_J_LettreChoisie2->isVisible() &&
             ui->pB_J_LettreChoisie2->text().trimmed().isEmpty()) {
        ui->pB_J_LettreChoisie2->setText(ui->pBLettre7->text());
        ui->pBLettre7->setText("");
    }
    else if (ui->pB_J_LettreChoisie3->isVisible() &&
             ui->pB_J_LettreChoisie3->text().trimmed().isEmpty()) {
        ui->pB_J_LettreChoisie3->setText(ui->pBLettre7->text());
        ui->pBLettre7->setText("");
    }
    else if (ui->pB_J_LettreChoisie4->isVisible() &&
             ui->pB_J_LettreChoisie4->text().trimmed().isEmpty()) {
        ui->pB_J_LettreChoisie4->setText(ui->pBLettre7->text());
        ui->pBLettre7->setText("");
    }
    else if (ui->pB_J_LettreChoisie5->isVisible() &&
             ui->pB_J_LettreChoisie5->text().trimmed().isEmpty()) {
        ui->pB_J_LettreChoisie5->setText(ui->pBLettre7->text());
        ui->pBLettre7->setText("");
    }
    else if (ui->pB_J_LettreChoisie6->isVisible() &&
             ui->pB_J_LettreChoisie6->text().trimmed().isEmpty()) {
        ui->pB_J_LettreChoisie6->setText(ui->pBLettre7->text());
        ui->pBLettre7->setText("");
    }
    else if (ui->pB_J_LettreChoisie7->isVisible() &&
             ui->pB_J_LettreChoisie7->text().trimmed().isEmpty()) {
        ui->pB_J_LettreChoisie7->setText(ui->pBLettre7->text());
        ui->pBLettre7->setText("");
    }
    else if (ui->pB_J_LettreChoisie8->isVisible() &&
             ui->pB_J_LettreChoisie8->text().trimmed().isEmpty()) {
        ui->pB_J_LettreChoisie8->setText(ui->pBLettre7->text());
        ui->pBLettre7->setText("");
    }

    reponseTrouvee();
}

void Acceuil::on_pBLettre8_clicked()
{
    if (ui->pB_J_LettreChoisie1->isVisible() &&
            ui->pB_J_LettreChoisie1->text().trimmed().isEmpty()) {
        ui->pB_J_LettreChoisie1->setText(ui->pBLettre8->text());
        ui->pBLettre8->setText("");
    }
    else if (ui->pB_J_LettreChoisie2->isVisible() &&
             ui->pB_J_LettreChoisie2->text().trimmed().isEmpty()) {
        ui->pB_J_LettreChoisie2->setText(ui->pBLettre8->text());
        ui->pBLettre8->setText("");
    }
    else if (ui->pB_J_LettreChoisie3->isVisible() &&
             ui->pB_J_LettreChoisie3->text().trimmed().isEmpty()) {
        ui->pB_J_LettreChoisie3->setText(ui->pBLettre8->text());
        ui->pBLettre8->setText("");
    }
    else if (ui->pB_J_LettreChoisie4->isVisible() &&
             ui->pB_J_LettreChoisie4->text().trimmed().isEmpty()) {
        ui->pB_J_LettreChoisie4->setText(ui->pBLettre8->text());
        ui->pBLettre8->setText("");
    }
    else if (ui->pB_J_LettreChoisie5->isVisible() &&
             ui->pB_J_LettreChoisie5->text().trimmed().isEmpty()) {
        ui->pB_J_LettreChoisie5->setText(ui->pBLettre8->text());
        ui->pBLettre8->setText("");
    }
    else if (ui->pB_J_LettreChoisie6->isVisible() &&
             ui->pB_J_LettreChoisie6->text().trimmed().isEmpty()) {
        ui->pB_J_LettreChoisie6->setText(ui->pBLettre8->text());
        ui->pBLettre8->setText("");
    }
    else if (ui->pB_J_LettreChoisie7->isVisible() &&
             ui->pB_J_LettreChoisie7->text().trimmed().isEmpty()) {
        ui->pB_J_LettreChoisie7->setText(ui->pBLettre8->text());
        ui->pBLettre8->setText("");
    }
    else if (ui->pB_J_LettreChoisie8->isVisible() &&
             ui->pB_J_LettreChoisie8->text().trimmed().isEmpty()) {
        ui->pB_J_LettreChoisie8->setText(ui->pBLettre8->text());
        ui->pBLettre8->setText("");
    }

    reponseTrouvee();
}

void Acceuil::on_pBLettre9_clicked()
{
    if (ui->pB_J_LettreChoisie1->isVisible() &&
            ui->pB_J_LettreChoisie1->text().trimmed().isEmpty()) {
        ui->pB_J_LettreChoisie1->setText(ui->pBLettre9->text());
        ui->pBLettre9->setText("");
    }
    else if (ui->pB_J_LettreChoisie2->isVisible() &&
             ui->pB_J_LettreChoisie2->text().trimmed().isEmpty()) {
        ui->pB_J_LettreChoisie2->setText(ui->pBLettre9->text());
        ui->pBLettre9->setText("");
    }
    else if (ui->pB_J_LettreChoisie3->isVisible() &&
             ui->pB_J_LettreChoisie3->text().trimmed().isEmpty()) {
        ui->pB_J_LettreChoisie3->setText(ui->pBLettre9->text());
        ui->pBLettre9->setText("");
    }
    else if (ui->pB_J_LettreChoisie4->isVisible() &&
             ui->pB_J_LettreChoisie4->text().trimmed().isEmpty()) {
        ui->pB_J_LettreChoisie4->setText(ui->pBLettre9->text());
        ui->pBLettre9->setText("");
    }
    else if (ui->pB_J_LettreChoisie5->isVisible() &&
             ui->pB_J_LettreChoisie5->text().trimmed().isEmpty()) {
        ui->pB_J_LettreChoisie5->setText(ui->pBLettre9->text());
        ui->pBLettre9->setText("");
    }
    else if (ui->pB_J_LettreChoisie6->isVisible() &&
             ui->pB_J_LettreChoisie6->text().trimmed().isEmpty()) {
        ui->pB_J_LettreChoisie6->setText(ui->pBLettre9->text());
        ui->pBLettre9->setText("");
    }
    else if (ui->pB_J_LettreChoisie7->isVisible() &&
             ui->pB_J_LettreChoisie7->text().trimmed().isEmpty()) {
        ui->pB_J_LettreChoisie7->setText(ui->pBLettre9->text());
        ui->pBLettre9->setText("");
    }
    else if (ui->pB_J_LettreChoisie8->isVisible() &&
             ui->pB_J_LettreChoisie8->text().trimmed().isEmpty()) {
        ui->pB_J_LettreChoisie8->setText(ui->pBLettre9->text());
        ui->pBLettre9->setText("");
    }

    reponseTrouvee();
}

void Acceuil::on_pBLettre10_clicked()
{
    if (ui->pB_J_LettreChoisie1->isVisible() &&
            ui->pB_J_LettreChoisie1->text().trimmed().isEmpty()) {
        ui->pB_J_LettreChoisie1->setText(ui->pBLettre10->text());
        ui->pBLettre10->setText("");
    }
    else if (ui->pB_J_LettreChoisie2->isVisible() &&
             ui->pB_J_LettreChoisie2->text().trimmed().isEmpty()) {
        ui->pB_J_LettreChoisie2->setText(ui->pBLettre10->text());
        ui->pBLettre10->setText("");
    }
    else if (ui->pB_J_LettreChoisie3->isVisible() &&
             ui->pB_J_LettreChoisie3->text().trimmed().isEmpty()) {
        ui->pB_J_LettreChoisie3->setText(ui->pBLettre10->text());
        ui->pBLettre10->setText("");
    }
    else if (ui->pB_J_LettreChoisie4->isVisible() &&
             ui->pB_J_LettreChoisie4->text().trimmed().isEmpty()) {
        ui->pB_J_LettreChoisie4->setText(ui->pBLettre10->text());
        ui->pBLettre10->setText("");
    }
    else if (ui->pB_J_LettreChoisie5->isVisible() &&
             ui->pB_J_LettreChoisie5->text().trimmed().isEmpty()) {
        ui->pB_J_LettreChoisie5->setText(ui->pBLettre10->text());
        ui->pBLettre10->setText("");
    }
    else if (ui->pB_J_LettreChoisie6->isVisible() &&
             ui->pB_J_LettreChoisie6->text().trimmed().isEmpty()) {
        ui->pB_J_LettreChoisie6->setText(ui->pBLettre10->text());
        ui->pBLettre10->setText("");
    }
    else if (ui->pB_J_LettreChoisie7->isVisible() &&
             ui->pB_J_LettreChoisie7->text().trimmed().isEmpty()) {
        ui->pB_J_LettreChoisie7->setText(ui->pBLettre10->text());
        ui->pBLettre10->setText("");
    }
    else if (ui->pB_J_LettreChoisie8->isVisible() &&
             ui->pB_J_LettreChoisie8->text().trimmed().isEmpty()) {
        ui->pB_J_LettreChoisie8->setText(ui->pBLettre10->text());
        ui->pBLettre10->setText("");
    }

    reponseTrouvee();
}

void Acceuil::on_pBLettre11_clicked()
{
    if (ui->pB_J_LettreChoisie1->isVisible() &&
            ui->pB_J_LettreChoisie1->text().trimmed().isEmpty()) {
        ui->pB_J_LettreChoisie1->setText(ui->pBLettre11->text());
        ui->pBLettre11->setText("");
    }
    else if (ui->pB_J_LettreChoisie2->isVisible() &&
             ui->pB_J_LettreChoisie2->text().trimmed().isEmpty()) {
        ui->pB_J_LettreChoisie2->setText(ui->pBLettre11->text());
        ui->pBLettre11->setText("");
    }
    else if (ui->pB_J_LettreChoisie3->isVisible() &&
             ui->pB_J_LettreChoisie3->text().trimmed().isEmpty()) {
        ui->pB_J_LettreChoisie3->setText(ui->pBLettre11->text());
        ui->pBLettre11->setText("");
    }
    else if (ui->pB_J_LettreChoisie4->isVisible() &&
             ui->pB_J_LettreChoisie4->text().trimmed().isEmpty()) {
        ui->pB_J_LettreChoisie4->setText(ui->pBLettre11->text());
        ui->pBLettre11->setText("");
    }
    else if (ui->pB_J_LettreChoisie5->isVisible() &&
             ui->pB_J_LettreChoisie5->text().trimmed().isEmpty()) {
        ui->pB_J_LettreChoisie5->setText(ui->pBLettre11->text());
        ui->pBLettre11->setText("");
    }
    else if (ui->pB_J_LettreChoisie6->isVisible() &&
             ui->pB_J_LettreChoisie6->text().trimmed().isEmpty()) {
        ui->pB_J_LettreChoisie6->setText(ui->pBLettre11->text());
        ui->pBLettre11->setText("");
    }
    else if (ui->pB_J_LettreChoisie7->isVisible() &&
             ui->pB_J_LettreChoisie7->text().trimmed().isEmpty()) {
        ui->pB_J_LettreChoisie7->setText(ui->pBLettre11->text());
        ui->pBLettre11->setText("");
    }
    else if (ui->pB_J_LettreChoisie8->isVisible() &&
             ui->pB_J_LettreChoisie8->text().trimmed().isEmpty()) {
        ui->pB_J_LettreChoisie8->setText(ui->pBLettre11->text());
        ui->pBLettre11->setText("");
    }

    reponseTrouvee();
}

void Acceuil::on_pBLettre12_clicked()
{
    if (ui->pB_J_LettreChoisie1->isVisible() &&
            ui->pB_J_LettreChoisie1->text().trimmed().isEmpty()) {
        ui->pB_J_LettreChoisie1->setText(ui->pBLettre12->text());
        ui->pBLettre12->setText("");
    }
    else if (ui->pB_J_LettreChoisie2->isVisible() &&
             ui->pB_J_LettreChoisie2->text().trimmed().isEmpty()) {
        ui->pB_J_LettreChoisie2->setText(ui->pBLettre12->text());
        ui->pBLettre12->setText("");
    }
    else if (ui->pB_J_LettreChoisie3->isVisible() &&
             ui->pB_J_LettreChoisie3->text().trimmed().isEmpty()) {
        ui->pB_J_LettreChoisie3->setText(ui->pBLettre12->text());
        ui->pBLettre12->setText("");
    }
    else if (ui->pB_J_LettreChoisie4->isVisible() &&
             ui->pB_J_LettreChoisie4->text().trimmed().isEmpty()) {
        ui->pB_J_LettreChoisie4->setText(ui->pBLettre12->text());
        ui->pBLettre12->setText("");
    }
    else if (ui->pB_J_LettreChoisie5->isVisible() &&
             ui->pB_J_LettreChoisie5->text().trimmed().isEmpty()) {
        ui->pB_J_LettreChoisie5->setText(ui->pBLettre12->text());
        ui->pBLettre12->setText("");
    }
    else if (ui->pB_J_LettreChoisie6->isVisible() &&
             ui->pB_J_LettreChoisie6->text().trimmed().isEmpty()) {
        ui->pB_J_LettreChoisie6->setText(ui->pBLettre12->text());
        ui->pBLettre12->setText("");
    }
    else if (ui->pB_J_LettreChoisie7->isVisible() &&
             ui->pB_J_LettreChoisie7->text().trimmed().isEmpty()) {
        ui->pB_J_LettreChoisie7->setText(ui->pBLettre12->text());
        ui->pBLettre12->setText("");
    }
    else if (ui->pB_J_LettreChoisie8->isVisible() &&
             ui->pB_J_LettreChoisie8->text().trimmed().isEmpty()) {
        ui->pB_J_LettreChoisie8->setText(ui->pBLettre12->text());
        ui->pBLettre12->setText("");
    }

    reponseTrouvee();
}

void Acceuil::on_pB_J_LettreChoisie1_clicked()
{
    if (ui->pB_J_LettreChoisie1->isVisible() && !ui->pB_J_LettreChoisie1->text().isEmpty()) {

        if (ui->pBLettre1->text().isEmpty()) {
            ui->pBLettre1->setText(ui->pB_J_LettreChoisie1->text());
            ui->pB_J_LettreChoisie1->setText("");
        }
        else if (ui->pBLettre2->text().isEmpty()) {
            ui->pBLettre2->setText(ui->pB_J_LettreChoisie1->text());
            ui->pB_J_LettreChoisie1->setText("");
        }
        else if (ui->pBLettre3->text().isEmpty()) {
            ui->pBLettre3->setText(ui->pB_J_LettreChoisie1->text());
            ui->pB_J_LettreChoisie1->setText("");
        }
        else if (ui->pBLettre4->text().isEmpty()) {
            ui->pBLettre4->setText(ui->pB_J_LettreChoisie1->text());
            ui->pB_J_LettreChoisie1->setText("");
        }
        else if (ui->pBLettre5->text().isEmpty()) {
            ui->pBLettre5->setText(ui->pB_J_LettreChoisie1->text());
            ui->pB_J_LettreChoisie1->setText("");
        }
        else if (ui->pBLettre6->text().isEmpty()) {
            ui->pBLettre6->setText(ui->pB_J_LettreChoisie1->text());
            ui->pB_J_LettreChoisie1->setText("");
        }
        else if (ui->pBLettre7->text().isEmpty()) {
            ui->pBLettre7->setText(ui->pB_J_LettreChoisie1->text());
            ui->pB_J_LettreChoisie1->setText("");
        }
        else if (ui->pBLettre8->text().isEmpty()) {
            ui->pBLettre8->setText(ui->pB_J_LettreChoisie1->text());
            ui->pB_J_LettreChoisie1->setText("");
        }
        else if (ui->pBLettre9->text().isEmpty()) {
            ui->pBLettre9->setText(ui->pB_J_LettreChoisie1->text());
            ui->pB_J_LettreChoisie1->setText("");
        }
        else if (ui->pBLettre10->text().isEmpty()) {
            ui->pBLettre10->setText(ui->pB_J_LettreChoisie1->text());
            ui->pB_J_LettreChoisie1->setText("");
        }
        else if (ui->pBLettre11->text().isEmpty()) {
            ui->pBLettre11->setText(ui->pB_J_LettreChoisie1->text());
            ui->pB_J_LettreChoisie1->setText("");
        }
        else if (ui->pBLettre12->text().isEmpty()) {
            ui->pBLettre12->setText(ui->pB_J_LettreChoisie1->text());
            ui->pB_J_LettreChoisie1->setText("");
        }
    }
}

void Acceuil::on_pB_J_LettreChoisie2_clicked()
{
    if (ui->pB_J_LettreChoisie2->isVisible() && !ui->pB_J_LettreChoisie2->text().isEmpty()) {

        if (ui->pBLettre1->text().isEmpty()) {
            ui->pBLettre1->setText(ui->pB_J_LettreChoisie2->text());
            ui->pB_J_LettreChoisie2->setText("");
        }
        else if (ui->pBLettre2->text().isEmpty()) {
            ui->pBLettre2->setText(ui->pB_J_LettreChoisie2->text());
            ui->pB_J_LettreChoisie2->setText("");
        }
        else if (ui->pBLettre3->text().isEmpty()) {
            ui->pBLettre3->setText(ui->pB_J_LettreChoisie2->text());
            ui->pB_J_LettreChoisie2->setText("");
        }
        else if (ui->pBLettre4->text().isEmpty()) {
            ui->pBLettre4->setText(ui->pB_J_LettreChoisie2->text());
            ui->pB_J_LettreChoisie2->setText("");
        }
        else if (ui->pBLettre5->text().isEmpty()) {
            ui->pBLettre5->setText(ui->pB_J_LettreChoisie2->text());
            ui->pB_J_LettreChoisie2->setText("");
        }
        else if (ui->pBLettre6->text().isEmpty()) {
            ui->pBLettre6->setText(ui->pB_J_LettreChoisie2->text());
            ui->pB_J_LettreChoisie2->setText("");
        }
        else if (ui->pBLettre7->text().isEmpty()) {
            ui->pBLettre7->setText(ui->pB_J_LettreChoisie2->text());
            ui->pB_J_LettreChoisie2->setText("");
        }
        else if (ui->pBLettre8->text().isEmpty()) {
            ui->pBLettre8->setText(ui->pB_J_LettreChoisie2->text());
            ui->pB_J_LettreChoisie2->setText("");
        }
        else if (ui->pBLettre9->text().isEmpty()) {
            ui->pBLettre9->setText(ui->pB_J_LettreChoisie2->text());
            ui->pB_J_LettreChoisie2->setText("");
        }
        else if (ui->pBLettre10->text().isEmpty()) {
            ui->pBLettre10->setText(ui->pB_J_LettreChoisie2->text());
            ui->pB_J_LettreChoisie2->setText("");
        }
        else if (ui->pBLettre11->text().isEmpty()) {
            ui->pBLettre11->setText(ui->pB_J_LettreChoisie2->text());
            ui->pB_J_LettreChoisie2->setText("");
        }
        else if (ui->pBLettre12->text().isEmpty()) {
            ui->pBLettre12->setText(ui->pB_J_LettreChoisie2->text());
            ui->pB_J_LettreChoisie2->setText("");
        }
    }
}

void Acceuil::on_pB_J_LettreChoisie3_clicked()
{
    if (ui->pB_J_LettreChoisie3->isVisible() && !ui->pB_J_LettreChoisie3->text().isEmpty()) {

        if (ui->pBLettre1->text().isEmpty()) {
            ui->pBLettre1->setText(ui->pB_J_LettreChoisie3->text());
            ui->pB_J_LettreChoisie3->setText("");
        }
        else if (ui->pBLettre2->text().isEmpty()) {
            ui->pBLettre2->setText(ui->pB_J_LettreChoisie3->text());
            ui->pB_J_LettreChoisie3->setText("");
        }
        else if (ui->pBLettre3->text().isEmpty()) {
            ui->pBLettre3->setText(ui->pB_J_LettreChoisie3->text());
            ui->pB_J_LettreChoisie3->setText("");
        }
        else if (ui->pBLettre4->text().isEmpty()) {
            ui->pBLettre4->setText(ui->pB_J_LettreChoisie3->text());
            ui->pB_J_LettreChoisie3->setText("");
        }
        else if (ui->pBLettre5->text().isEmpty()) {
            ui->pBLettre5->setText(ui->pB_J_LettreChoisie3->text());
            ui->pB_J_LettreChoisie3->setText("");
        }
        else if (ui->pBLettre6->text().isEmpty()) {
            ui->pBLettre6->setText(ui->pB_J_LettreChoisie3->text());
            ui->pB_J_LettreChoisie3->setText("");
        }
        else if (ui->pBLettre7->text().isEmpty()) {
            ui->pBLettre7->setText(ui->pB_J_LettreChoisie3->text());
            ui->pB_J_LettreChoisie3->setText("");
        }
        else if (ui->pBLettre8->text().isEmpty()) {
            ui->pBLettre8->setText(ui->pB_J_LettreChoisie3->text());
            ui->pB_J_LettreChoisie3->setText("");
        }
        else if (ui->pBLettre9->text().isEmpty()) {
            ui->pBLettre9->setText(ui->pB_J_LettreChoisie3->text());
            ui->pB_J_LettreChoisie3->setText("");
        }
        else if (ui->pBLettre10->text().isEmpty()) {
            ui->pBLettre10->setText(ui->pB_J_LettreChoisie3->text());
            ui->pB_J_LettreChoisie3->setText("");
        }
        else if (ui->pBLettre11->text().isEmpty()) {
            ui->pBLettre11->setText(ui->pB_J_LettreChoisie3->text());
            ui->pB_J_LettreChoisie3->setText("");
        }
        else if (ui->pBLettre12->text().isEmpty()) {
            ui->pBLettre12->setText(ui->pB_J_LettreChoisie3->text());
            ui->pB_J_LettreChoisie3->setText("");
        }
    }
}

void Acceuil::on_pB_J_LettreChoisie4_clicked()
{
    if (ui->pB_J_LettreChoisie4->isVisible() && !ui->pB_J_LettreChoisie4->text().isEmpty()) {

        if (ui->pBLettre1->text().isEmpty()) {
            ui->pBLettre1->setText(ui->pB_J_LettreChoisie4->text());
            ui->pB_J_LettreChoisie4->setText("");
        }
        else if (ui->pBLettre2->text().isEmpty()) {
            ui->pBLettre2->setText(ui->pB_J_LettreChoisie4->text());
            ui->pB_J_LettreChoisie4->setText("");
        }
        else if (ui->pBLettre3->text().isEmpty()) {
            ui->pBLettre3->setText(ui->pB_J_LettreChoisie4->text());
            ui->pB_J_LettreChoisie4->setText("");
        }
        else if (ui->pBLettre4->text().isEmpty()) {
            ui->pBLettre4->setText(ui->pB_J_LettreChoisie4->text());
            ui->pB_J_LettreChoisie4->setText("");
        }
        else if (ui->pBLettre5->text().isEmpty()) {
            ui->pBLettre5->setText(ui->pB_J_LettreChoisie4->text());
            ui->pB_J_LettreChoisie4->setText("");
        }
        else if (ui->pBLettre6->text().isEmpty()) {
            ui->pBLettre6->setText(ui->pB_J_LettreChoisie4->text());
            ui->pB_J_LettreChoisie4->setText("");
        }
        else if (ui->pBLettre7->text().isEmpty()) {
            ui->pBLettre7->setText(ui->pB_J_LettreChoisie4->text());
            ui->pB_J_LettreChoisie4->setText("");
        }
        else if (ui->pBLettre8->text().isEmpty()) {
            ui->pBLettre8->setText(ui->pB_J_LettreChoisie4->text());
            ui->pB_J_LettreChoisie4->setText("");
        }
        else if (ui->pBLettre9->text().isEmpty()) {
            ui->pBLettre9->setText(ui->pB_J_LettreChoisie4->text());
            ui->pB_J_LettreChoisie4->setText("");
        }
        else if (ui->pBLettre10->text().isEmpty()) {
            ui->pBLettre10->setText(ui->pB_J_LettreChoisie4->text());
            ui->pB_J_LettreChoisie4->setText("");
        }
        else if (ui->pBLettre11->text().isEmpty()) {
            ui->pBLettre11->setText(ui->pB_J_LettreChoisie4->text());
            ui->pB_J_LettreChoisie4->setText("");
        }
        else if (ui->pBLettre12->text().isEmpty()) {
            ui->pBLettre12->setText(ui->pB_J_LettreChoisie4->text());
            ui->pB_J_LettreChoisie4->setText("");
        }
    }
}

void Acceuil::on_pB_J_LettreChoisie5_clicked()
{
    if (ui->pB_J_LettreChoisie5->isVisible() && !ui->pB_J_LettreChoisie5->text().isEmpty()) {

        if (ui->pBLettre1->text().isEmpty()) {
            ui->pBLettre1->setText(ui->pB_J_LettreChoisie5->text());
            ui->pB_J_LettreChoisie5->setText("");
        }
        else if (ui->pBLettre2->text().isEmpty()) {
            ui->pBLettre2->setText(ui->pB_J_LettreChoisie5->text());
            ui->pB_J_LettreChoisie5->setText("");
        }
        else if (ui->pBLettre3->text().isEmpty()) {
            ui->pBLettre3->setText(ui->pB_J_LettreChoisie5->text());
            ui->pB_J_LettreChoisie5->setText("");
        }
        else if (ui->pBLettre4->text().isEmpty()) {
            ui->pBLettre4->setText(ui->pB_J_LettreChoisie5->text());
            ui->pB_J_LettreChoisie5->setText("");
        }
        else if (ui->pBLettre5->text().isEmpty()) {
            ui->pBLettre5->setText(ui->pB_J_LettreChoisie5->text());
            ui->pB_J_LettreChoisie5->setText("");
        }
        else if (ui->pBLettre6->text().isEmpty()) {
            ui->pBLettre6->setText(ui->pB_J_LettreChoisie5->text());
            ui->pB_J_LettreChoisie5->setText("");
        }
        else if (ui->pBLettre7->text().isEmpty()) {
            ui->pBLettre7->setText(ui->pB_J_LettreChoisie5->text());
            ui->pB_J_LettreChoisie5->setText("");
        }
        else if (ui->pBLettre8->text().isEmpty()) {
            ui->pBLettre8->setText(ui->pB_J_LettreChoisie5->text());
            ui->pB_J_LettreChoisie5->setText("");
        }
        else if (ui->pBLettre9->text().isEmpty()) {
            ui->pBLettre9->setText(ui->pB_J_LettreChoisie5->text());
            ui->pB_J_LettreChoisie5->setText("");
        }
        else if (ui->pBLettre10->text().isEmpty()) {
            ui->pBLettre10->setText(ui->pB_J_LettreChoisie5->text());
            ui->pB_J_LettreChoisie5->setText("");
        }
        else if (ui->pBLettre11->text().isEmpty()) {
            ui->pBLettre11->setText(ui->pB_J_LettreChoisie5->text());
            ui->pB_J_LettreChoisie5->setText("");
        }
        else if (ui->pBLettre12->text().isEmpty()) {
            ui->pBLettre12->setText(ui->pB_J_LettreChoisie5->text());
            ui->pB_J_LettreChoisie5->setText("");
        }
    }
}

void Acceuil::on_pB_J_LettreChoisie6_clicked()
{
    if (ui->pB_J_LettreChoisie6->isVisible() && !ui->pB_J_LettreChoisie6->text().isEmpty()) {

        if (ui->pBLettre1->text().isEmpty()) {
            ui->pBLettre1->setText(ui->pB_J_LettreChoisie6->text());
            ui->pB_J_LettreChoisie6->setText("");
        }
        else if (ui->pBLettre2->text().isEmpty()) {
            ui->pBLettre2->setText(ui->pB_J_LettreChoisie6->text());
            ui->pB_J_LettreChoisie6->setText("");
        }
        else if (ui->pBLettre3->text().isEmpty()) {
            ui->pBLettre3->setText(ui->pB_J_LettreChoisie6->text());
            ui->pB_J_LettreChoisie6->setText("");
        }
        else if (ui->pBLettre4->text().isEmpty()) {
            ui->pBLettre4->setText(ui->pB_J_LettreChoisie6->text());
            ui->pB_J_LettreChoisie6->setText("");
        }
        else if (ui->pBLettre5->text().isEmpty()) {
            ui->pBLettre5->setText(ui->pB_J_LettreChoisie6->text());
            ui->pB_J_LettreChoisie6->setText("");
        }
        else if (ui->pBLettre6->text().isEmpty()) {
            ui->pBLettre6->setText(ui->pB_J_LettreChoisie6->text());
            ui->pB_J_LettreChoisie6->setText("");
        }
        else if (ui->pBLettre7->text().isEmpty()) {
            ui->pBLettre7->setText(ui->pB_J_LettreChoisie6->text());
            ui->pB_J_LettreChoisie6->setText("");
        }
        else if (ui->pBLettre8->text().isEmpty()) {
            ui->pBLettre8->setText(ui->pB_J_LettreChoisie6->text());
            ui->pB_J_LettreChoisie6->setText("");
        }
        else if (ui->pBLettre9->text().isEmpty()) {
            ui->pBLettre9->setText(ui->pB_J_LettreChoisie6->text());
            ui->pB_J_LettreChoisie6->setText("");
        }
        else if (ui->pBLettre10->text().isEmpty()) {
            ui->pBLettre10->setText(ui->pB_J_LettreChoisie6->text());
            ui->pB_J_LettreChoisie6->setText("");
        }
        else if (ui->pBLettre11->text().isEmpty()) {
            ui->pBLettre11->setText(ui->pB_J_LettreChoisie6->text());
            ui->pB_J_LettreChoisie6->setText("");
        }
        else if (ui->pBLettre12->text().isEmpty()) {
            ui->pBLettre12->setText(ui->pB_J_LettreChoisie6->text());
            ui->pB_J_LettreChoisie6->setText("");
        }
    }
}

void Acceuil::on_pB_J_LettreChoisie7_clicked()
{
    if (ui->pB_J_LettreChoisie7->isVisible() && !ui->pB_J_LettreChoisie7->text().isEmpty()) {

        if (ui->pBLettre1->text().isEmpty()) {
            ui->pBLettre1->setText(ui->pB_J_LettreChoisie7->text());
            ui->pB_J_LettreChoisie7->setText("");
        }
        else if (ui->pBLettre2->text().isEmpty()) {
            ui->pBLettre2->setText(ui->pB_J_LettreChoisie7->text());
            ui->pB_J_LettreChoisie7->setText("");
        }
        else if (ui->pBLettre3->text().isEmpty()) {
            ui->pBLettre3->setText(ui->pB_J_LettreChoisie7->text());
            ui->pB_J_LettreChoisie7->setText("");
        }
        else if (ui->pBLettre4->text().isEmpty()) {
            ui->pBLettre4->setText(ui->pB_J_LettreChoisie7->text());
            ui->pB_J_LettreChoisie7->setText("");
        }
        else if (ui->pBLettre5->text().isEmpty()) {
            ui->pBLettre5->setText(ui->pB_J_LettreChoisie7->text());
            ui->pB_J_LettreChoisie7->setText("");
        }
        else if (ui->pBLettre6->text().isEmpty()) {
            ui->pBLettre6->setText(ui->pB_J_LettreChoisie7->text());
            ui->pB_J_LettreChoisie7->setText("");
        }
        else if (ui->pBLettre7->text().isEmpty()) {
            ui->pBLettre7->setText(ui->pB_J_LettreChoisie7->text());
            ui->pB_J_LettreChoisie7->setText("");
        }
        else if (ui->pBLettre8->text().isEmpty()) {
            ui->pBLettre8->setText(ui->pB_J_LettreChoisie7->text());
            ui->pB_J_LettreChoisie7->setText("");
        }
        else if (ui->pBLettre9->text().isEmpty()) {
            ui->pBLettre9->setText(ui->pB_J_LettreChoisie7->text());
            ui->pB_J_LettreChoisie7->setText("");
        }
        else if (ui->pBLettre10->text().isEmpty()) {
            ui->pBLettre10->setText(ui->pB_J_LettreChoisie7->text());
            ui->pB_J_LettreChoisie7->setText("");
        }
        else if (ui->pBLettre11->text().isEmpty()) {
            ui->pBLettre11->setText(ui->pB_J_LettreChoisie7->text());
            ui->pB_J_LettreChoisie7->setText("");
        }
        else if (ui->pBLettre12->text().isEmpty()) {
            ui->pBLettre12->setText(ui->pB_J_LettreChoisie7->text());
            ui->pB_J_LettreChoisie7->setText("");
        }
    }
}

void Acceuil::on_pB_J_LettreChoisie8_clicked()
{
    if (ui->pB_J_LettreChoisie8->isVisible() && !ui->pB_J_LettreChoisie8->text().isEmpty()) {

        if (ui->pBLettre1->text().isEmpty()) {
            ui->pBLettre1->setText(ui->pB_J_LettreChoisie8->text());
            ui->pB_J_LettreChoisie8->setText("");
        }
        else if (ui->pBLettre2->text().isEmpty()) {
            ui->pBLettre2->setText(ui->pB_J_LettreChoisie8->text());
            ui->pB_J_LettreChoisie8->setText("");
        }
        else if (ui->pBLettre3->text().isEmpty()) {
            ui->pBLettre3->setText(ui->pB_J_LettreChoisie8->text());
            ui->pB_J_LettreChoisie8->setText("");
        }
        else if (ui->pBLettre4->text().isEmpty()) {
            ui->pBLettre4->setText(ui->pB_J_LettreChoisie8->text());
            ui->pB_J_LettreChoisie8->setText("");
        }
        else if (ui->pBLettre5->text().isEmpty()) {
            ui->pBLettre5->setText(ui->pB_J_LettreChoisie8->text());
            ui->pB_J_LettreChoisie8->setText("");
        }
        else if (ui->pBLettre6->text().isEmpty()) {
            ui->pBLettre6->setText(ui->pB_J_LettreChoisie8->text());
            ui->pB_J_LettreChoisie8->setText("");
        }
        else if (ui->pBLettre7->text().isEmpty()) {
            ui->pBLettre7->setText(ui->pB_J_LettreChoisie8->text());
            ui->pB_J_LettreChoisie8->setText("");
        }
        else if (ui->pBLettre8->text().isEmpty()) {
            ui->pBLettre8->setText(ui->pB_J_LettreChoisie8->text());
            ui->pB_J_LettreChoisie8->setText("");
        }
        else if (ui->pBLettre9->text().isEmpty()) {
            ui->pBLettre9->setText(ui->pB_J_LettreChoisie8->text());
            ui->pB_J_LettreChoisie8->setText("");
        }
        else if (ui->pBLettre10->text().isEmpty()) {
            ui->pBLettre10->setText(ui->pB_J_LettreChoisie8->text());
            ui->pB_J_LettreChoisie8->setText("");
        }
        else if (ui->pBLettre11->text().isEmpty()) {
            ui->pBLettre11->setText(ui->pB_J_LettreChoisie8->text());
            ui->pB_J_LettreChoisie8->setText("");
        }
        else if (ui->pBLettre12->text().isEmpty()) {
            ui->pBLettre12->setText(ui->pB_J_LettreChoisie8->text());
            ui->pB_J_LettreChoisie8->setText("");
        }
    }
}

void Acceuil::actualiserTemps()
{
    tempsJeu = tempsJeu.addSecs(1);
    ui->l_J_Temps->setText("<html><head/><body><p align=\"center\"><span style=\" font-size:12pt; font-weight:600; color:#f5c70e;\">"+tempsJeu.toString("hh:mm:ss")+"</span></p></body></html>");
}

void Acceuil::on_pBContinuerPartie_clicked()
{
    QFile fichierSave("partie.save");
    if(fichierSave.exists()) {
        partie = new PartieJeu();

        QString instruction = "SELECT COUNT() AS nbr FROM Mots;";
        QSqlQuery requete;
        requete.prepare(instruction);
        requete.exec();
        requete.next();

        if (requete.value("nbr").toInt() >= 10) {

        }
        else QMessageBox::information(this, "Nombre de mot Insuffisant", "Il n'y a pas assez de Mot pour continuer la partie ! Ajouter-en dans les paramètres");

        if (fichierSave.open(QIODevice::ReadOnly | QIODevice::Text)) {

            QTextStream in(&fichierSave);
            QString line = in.readLine();
            partie->setScore(line.section('\n', 0).toInt());

            line = in.readLine();
            line = line.section('\n', 0);

            partie->setDuree(QTime(line.section(':', 0).toInt(),
                                   line.section(':', 1).toInt(),
                                   line.section(':', 2).toInt()));

            while (!in.atEnd()) {
                line = in.readLine();
                partie->seTnumMots(line.section('\n', 0).toInt());
            }

            this->afficherMot(partie->numMots(partie->score()));
            fichierSave.remove();
            this->ui->stackedWidget->setCurrentWidget(ui->pageJeu);
        }
    }
}

void Acceuil::on_stackedWidget_currentChanged(int arg1)
{
    if (ui->stackedWidget->indexOf(ui->pageAcceuil) == arg1)
    {
        this->ui->pBScores->setDisabled(false);
        this->ui->pBParametres->setDisabled(false);
        this->ui->pBAcceuil->setDisabled(false);

        QFile fichierSauvegarde("partie.save");
        if (!fichierSauvegarde.exists()) this->ui->pBContinuerPartie->setDisabled(true);
        else this->ui->pBContinuerPartie->setDisabled(false);
    }
    if (ui->stackedWidget->indexOf(ui->pageJeu) == arg1) {
        ui->pBScores->setDisabled(true);
        ui->pBParametres->setDisabled(true);
    }
}
