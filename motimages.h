#ifndef MOTIMAGES_H
#define MOTIMAGES_H

#include <QString>

class MotImages
{
public:
    MotImages();

    QString tag () const;
    QString image(unsigned int numImage) const;

    void setTag(QString tag);
    void setImage(unsigned int numImage, QString image);
    bool complet();
    void aZero();


private:
    QString tagMot;
    QString image1;
    QString image2;
    QString image3;
    QString image4;
};


#endif // MOTIMAGES_H
