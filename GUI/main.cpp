#include "hhcc.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    HHCC w;
    w.show();

    return a.exec();
}
