#include "acceuil.h"
#include <QApplication>



int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Acceuil w;
    w.show();

    return a.exec();
}
