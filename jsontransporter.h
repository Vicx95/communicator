#pragma once
#include <QtCore/QObject>
#include <QtWebChannel/qwebchannelglobal.h>

QT_BEGIN_NAMESPACE
class QJsonObject;
class QWebSocket ;
QT_END_NAMESPACE
class JsonTransporter : public QObject
{
    Q_OBJECT
public:
    JsonTransporter() ;
    explicit JsonTransporter(QWebSocket *socket);
    ~JsonTransporter() ;

    void sendJsonMessage(const QJsonObject &message)  ;

Q_SIGNALS:
    void messageReceived(const QJsonObject &message, JsonTransporter *transport) ;

public slots:
    void jsonMessageReceived(const QString& message) ;

private:
    QWebSocket *js_socket ;
};

