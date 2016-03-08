#include "mynetworkaccessmanager.h"

#include <QDebug>
#include <QNetworkRequest>

MyNetworkAccessManager::MyNetworkAccessManager(QObject *parent) :
	QNetworkAccessManager(parent)
{
}

QNetworkReply *MyNetworkAccessManager::createRequest(QNetworkAccessManager::Operation op, const QNetworkRequest &request, QIODevice *outgoingData)
{
	qDebug() << "new request" << op << request.url();
	QNetworkRequest req(request);
	//req.setRawHeader("User-Agent", "981273912837");
	return QNetworkAccessManager::createRequest(op, req, outgoingData);
}
