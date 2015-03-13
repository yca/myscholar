#include "mynetworkaccessmanager.h"

#include <QDebug>
#include <QNetworkRequest>

MyNetworkAccessManager::MyNetworkAccessManager(QObject *parent) :
	QNetworkAccessManager(parent)
{
}

QNetworkReply *MyNetworkAccessManager::createRequest(QNetworkAccessManager::Operation op, const QNetworkRequest &request, QIODevice *outgoingData)
{
	//qDebug() << "new request" << op << request.url();
	return QNetworkAccessManager::createRequest(op, request, outgoingData);
}
