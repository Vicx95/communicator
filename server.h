#pragma once

#include "logger.h"
#include <QString>
#include <QVector>
#include <QtCore/QObject>
#include <QtCore/QList>
#include <QtCore/QByteArray>
#include <QtNetwork>
#include <QMap>

QT_FORWARD_DECLARE_CLASS(QWebSocketServer)
QT_FORWARD_DECLARE_CLASS(QWebSocket)


class Server : public QObject
{ 
    Q_OBJECT
public:
    explicit Server(quint16 port, QObject *parent = nullptr);
    ~Server();

    Q_SIGNALS:
        void closed();
        void messageReceived(const QJsonObject &message, Server *transport);

private Q_SLOTS:
        void onNewConnection();
        void processTxtMsg(QString msg);
        void processPrivTxtMsg(const QString &message) ;
        void socketDisconnected();
        void jsonMessageReceived(const QString& message);
        void nicknameListAdd(const QString& text);
        void nicknameListUpdateSend();

private:
        QWebSocketServer *WebSocketServer;
        QList<QWebSocket *> clients;
        Logger logs;
        QVector<QString> nicknameList;
};

