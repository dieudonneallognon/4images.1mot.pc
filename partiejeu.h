#ifndef PARTIEJEU_H
#define PARTIEJEU_H

#include <QVector>
#include <QString>
#include <QTime>


class PartieJeu
{
public:
    PartieJeu();

    int score() const;
    QTime duree() const;
    int numMots(int position) const;

    void setScore(int score);
    void setDuree(QTime duree);
    void seTnumMots(int numMot);
    void aZero();

private:
    int scorePartie;
    QTime dureePartie;
    QVector <int> listeNumMots;
};

#endif // PARTIEJEU_H
