#include "server.h"

#include <QtWebSockets/qwebsocketserver.h>
#include <QtWebSockets/qwebsocket.h>
#include <QtCore/QDebug>
#include <QNetworkReply>


static QString getIdentifier(QWebSocket *p)
{
    return QStringLiteral("%1:%2").arg(p->peerAddress().toString(),
                                   QString::number(p->peerPort()));
}


Server::Server(quint16 port,  QObject *parent ) :
    QObject(parent),
    WebSocketServer(new QWebSocketServer(QStringLiteral("serwer"),
    QWebSocketServer::NonSecureMode, this))
{
    if(WebSocketServer->listen(QHostAddress::Any,port))
    {
       QTextStream(stdout) << "Server nasluchuje na porcie: " << port << '\n';
       connect(WebSocketServer,&QWebSocketServer::newConnection,this, &Server::onNewConnection);
    }
}

Server::~Server()
{
    WebSocketServer->close();
    qDeleteAll(clients.begin(),clients.end());
}

void Server::onNewConnection()
{
    QWebSocket *socket = WebSocketServer->nextPendingConnection();
    QTextStream(stdout) << getIdentifier(socket) << " polaczony!\n";
    socket->setParent(this);
    connect(socket, &QWebSocket::textMessageReceived,this, &Server::jsonMessageReceived);

    connect(socket, &QWebSocket::textMessageReceived, this, &Server::processTxtMsg);
    connect(socket, &QWebSocket::textMessageReceived, this, &Server::processPrivTxtMsg);

    connect(socket, &QWebSocket::textMessageReceived, this, &Server::nicknameListAdd);

    connect(socket, &QWebSocket::disconnected, this, &Server::socketDisconnected);

    clients << socket;

    logs.saveConnectionLog(socket);
}

void  Server::processTxtMsg(const QString& message)
{
    QWebSocket *ptr_sender = qobject_cast<QWebSocket * >(sender());

    QJsonDocument msg = QJsonDocument::fromJson(message.toUtf8());

    QJsonObject object(msg.object());

    if (object.contains("event") && object["event"] == "chat") {
        for(QWebSocket *ptr_client : qAsConst(clients))
        {
            if(ptr_client != ptr_sender) {
                ptr_client->sendTextMessage(message);
            }
        }
    }
}

void Server::socketDisconnected()
{
    QWebSocket *client = qobject_cast<QWebSocket *> (sender());

    QTextStream(stdout) << getIdentifier(client) << "rozlaczony!\n";

    int pos;
    for (pos = 0; pos < clients.size(); ++pos) {
        if (clients.at(pos) == client) {
            break;
        }
    }

    nicknameList.erase(nicknameList.begin() + pos);

    nicknameListUpdateSend();

    if(client)
    {
        clients.removeAll(client);

        client->deleteLater();
    }
    logs.saveDisconnectLog(client);
}

void Server::jsonMessageReceived(const QString &message)
{
    QJsonParseError err;
    QJsonDocument msg = QJsonDocument::fromJson(message.toUtf8(),&err);
    if(err.error)
    {
        qWarning() << "Failed to parse text message as JSON object: " << message
                    << "error is: " << err.errorString();
    }
    else if (!msg.isObject())
    {
        qWarning()<< "Received JSON message that is not an object: " << message;
    }

    qDebug() << msg;
    emit messageReceived(msg.object(),this);
}

void Server::nicknameListAdd(const QString& text)
{
    QWebSocket *client = qobject_cast<QWebSocket *> (sender());

    QJsonDocument msg = QJsonDocument::fromJson(text.toUtf8());

    QJsonObject object(msg.object());

    if (object.contains("event") && object["event"] == "response" &&
        object.contains("type") && object["type"] == "nickname" &&
        object.contains("data")) {

        object = object["data"].toObject();

        QString nickname = object.value("nickname").toString();

        int pos;
        for (pos = 0; pos < clients.size(); ++pos) {
            if (clients.at(pos) == client) {
                break;
            }
        }

        qDebug() << nickname; qDebug() << pos;

        nicknameList.push_back(nickname);

        nicknameListUpdateSend();
    }
}

void Server::nicknameListUpdateSend()
{
    QJsonArray array;

    for (const QString& nickname : nicknameList) {
        array.push_back(nickname);
    }

    QJsonObject object;
    object.insert("event", QJsonValue::fromVariant("users"));
    object.insert("users", array);

    qDebug() << object;

    QJsonDocument doc(object);
    qDebug() << doc.toJson(QJsonDocument::Compact);

    for(QWebSocket *ptr_client : qAsConst(clients))
    {
        ptr_client->sendTextMessage(doc.toJson(QJsonDocument::Compact));
    }
}

void Server::processPrivTxtMsg(const QString &message)
{
    QJsonDocument msg = QJsonDocument::fromJson(message.toUtf8());

    QJsonObject object(msg.object());

    if(object.contains("event") && object["event"] == "private" &&
       object.contains("userName") && object.contains("toUserName") &&
       object.contains("message") )
    {
        QString toUser = object.value("toUserName").toString();

        for (int i = 0 ; i < nicknameList.size(); i++)
        {
            if (nicknameList[i].compare(toUser) == 0)
            {
                clients[i]->sendTextMessage(message);
                break;
            }

        }
    }
}

