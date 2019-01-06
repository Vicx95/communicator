#pragma once

#include <QFile>
#include <QDir>
#include <QDate>
#include <QTextStream>
#include <QWebSocket>
#include <QTime>


class Logger
{
public:
    Logger();
    void saveConnectionLog(QWebSocket *connect);
    void saveDisconnectLog(QWebSocket *disconnect);
};


