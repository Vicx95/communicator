#include "jsontransporter.h"

#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QWebSocket>

JsonTransporter::JsonTransporter(QWebSocket *socket) : js_socket(socket)
{
    connect(socket, &QWebSocket::textMessageReceived,this,&JsonTransporter::jsonMessageReceived) ;
    connect(socket, &QWebSocket::disconnected,this,&JsonTransporter::deleteLater) ;
}


JsonTransporter::~JsonTransporter()
{
    js_socket->deleteLater() ;
}

void JsonTransporter::sendJsonMessage(const QJsonObject &message)
{
    QJsonDocument document(message);
    js_socket ->sendTextMessage(QString::fromUtf8(document.toJson(QJsonDocument::Compact))) ;
}

void JsonTransporter::jsonMessageReceived(const QString &message)
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
    emit messageReceived(msg.object(),this);
}

