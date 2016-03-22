#include "hq.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    HQ w;
    w.show();

    return a.exec();
}
