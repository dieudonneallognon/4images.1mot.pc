#include "motimages.h"

MotImages::MotImages() { tagMot = image1 = image2 = image3 = image4 = ""; }

QString MotImages::tag() const { return this->tagMot; }

QString MotImages::image(unsigned int numImage) const
{
    switch (numImage) {
        case 1: return image1;
        case 2: return image2;
        case 3: return image3;
        case 4: return image4;
    }
}

void MotImages::setTag(QString tagMot) { this->tagMot = tagMot.trimmed().toUpper(); }

void MotImages::setImage(unsigned int numImage, QString image) {
    switch (numImage) {
        case 1:
            this->image1 = image.trimmed();
            break;
        case 2:
            this->image2 = image.trimmed();
            break;
        case 3:
            this->image3 = image.trimmed();
            break;
        case 4:
            this->image4 = image.trimmed();
            break;
    }
}

bool MotImages::complet()
{
    if (!image1.trimmed().isEmpty() && !image2.trimmed().isEmpty() && !image3.trimmed().isEmpty()
            && !image4.trimmed().isEmpty() && !tagMot.trimmed().isEmpty())
        return true;
    return false;
}

void MotImages::aZero()
{
    image1 = image2 = image3 = image4 = tagMot = "";
}
