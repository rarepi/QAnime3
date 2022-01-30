#include "QAnime3.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QAnime3 w;
    w.show();
    return a.exec();
}
