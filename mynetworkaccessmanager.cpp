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
	QNetworkRequest req(request);
	req.setRawHeader("User-Agent", "Let me in Google, this is for research purposes only!");
	return QNetworkAccessManager::createRequest(op, req, outgoingData);
}
