#include "partiejeu.h"

PartieJeu::PartieJeu()
{
    scorePartie = 0;
    dureePartie.setHMS(0, 0, 0);
    listeNumMots.clear();
}

void PartieJeu::aZero()
{
    scorePartie = 0;
    dureePartie.setHMS(0, 0, 0);
    listeNumMots.clear();
}

int PartieJeu::score() const { return scorePartie; }

QTime PartieJeu::duree() const { return dureePartie; }

int PartieJeu::numMots(int position) const {
    if (scorePartie >= position) return listeNumMots.value(position);
    return -1;
}

void PartieJeu::setScore(int score) {
    scorePartie = score;
}

void PartieJeu::setDuree(QTime duree)
{
    dureePartie = duree;
}

void PartieJeu::seTnumMots(int numMot)
{
    listeNumMots.append(numMot);
}
