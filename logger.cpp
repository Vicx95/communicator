#include "logger.h"

Logger::Logger()
{

}

static QString getIdentifier(QWebSocket *p)
{
    return QStringLiteral("%1:%2").arg(p->peerAddress().toString(),
                                   QString::number(p->peerPort()))  ;
}
void Logger::saveConnectionLog(QWebSocket *connect)
{
    QFile file(QString("%1%2.txt").arg(QDate::currentDate().toString("ddMMyyyy")).arg("_connect"));
    QTime time_current ;
    QString time = time_current.currentTime().toString("HH:mm:ss") ;
    file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append);
    QTextStream out(&file);
    out << "IP: " << getIdentifier(connect) << " czas: " << time << '\n' ;
    file.close();
}

void Logger::saveDisconnectLog(QWebSocket *disconnect)
{
    QFile file(QString("%1%2.txt").arg(QDate::currentDate().toString("ddMMyyyy")).arg("_disconnect"));
    QTime time_current ;
    QString time = time_current.currentTime().toString("HH:mm:ss") ;
    file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append);
    QTextStream out(&file);
    out << "IP: " << getIdentifier(disconnect) << " czas: " << time << '\n' ;
    file.close();
}
