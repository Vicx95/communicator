#include <QCoreApplication>
#include "server.h"
#include <QDebug>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    Server s(8999) ;

    return a.exec();
}
