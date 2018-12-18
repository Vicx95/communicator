#include "server.h"
#include <QtWebSockets/qwebsocketserver.h>
#include <QtWebSockets/qwebsocket.h>
#include <QtCore/QDebug>
#include <QNetworkReply>



static QString getIdentifier(QWebSocket *p)
{
    return QStringLiteral("%1:%2").arg(p->peerAddress().toString(),
                                   QString::number(p->peerPort()))  ;
}



Server::Server(quint16 port,  QObject *parent ) :
    QObject(parent),
    WebSocketServer(new QWebSocketServer(QStringLiteral("serwer"),
    QWebSocketServer::NonSecureMode, this))
{
    if(WebSocketServer->listen(QHostAddress::Any,port))
    {
       QTextStream(stdout) << "Server nasluchuje na porcie: " << port << '\n' ;
       connect(WebSocketServer,&QWebSocketServer::newConnection,this, &Server::onNewConnection) ;
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
    connect(socket, &QWebSocket::textMessageReceived,this,&Server::jsonMessageReceived);
    connect(socket, &QWebSocket::textMessageReceived, this, &Server::processTxtMsg);
    connect(socket, &QWebSocket::disconnected, this, &Server::socketDisconnected);
    clients << socket ;

    logs.saveConnectionLog(socket);

}

void  Server::processTxtMsg(QString msg)
{
    QWebSocket *ptr_sender = qobject_cast<QWebSocket * >(sender()) ;
    for(QWebSocket *ptr_client : qAsConst(clients))
    {
        if(ptr_client != ptr_sender)
            ptr_client->sendTextMessage(msg) ;
    }
}


void Server::socketDisconnected()
{
    QWebSocket *client = qobject_cast<QWebSocket *> (sender());

    QTextStream(stdout) << getIdentifier(client) << "rozlaczony!\n" ;

    if(client)
    {
        clients.removeAll(client);

        client->deleteLater();
    }
    logs.saveDisconnectLog(client) ;

}

void Server::jsonMessageReceived(const QString &message)
{
    QJsonParseError err ;
    QJsonDocument msg = QJsonDocument::fromJson(message.toUtf8(),&err) ;
    if(err.error)
    {
        qWarning() << "Failed to parse text message as JSON object: " << message
                    << "error is: " << err.errorString() ;
    }
    else if (!msg.isObject())
    {
        qWarning()<< "Received JSON message that is not an object: " << message;
    }


    qDebug() << msg  ;
    emit messageReceived(msg.object(),this);
}

