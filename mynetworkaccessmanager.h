#ifndef MYNETWORKACCESSMANAGER_H
#define MYNETWORKACCESSMANAGER_H

#include <QNetworkAccessManager>

class MyNetworkAccessManager : public QNetworkAccessManager
{
	Q_OBJECT
public:
	explicit MyNetworkAccessManager(QObject *parent = 0);

signals:

public slots:
protected:
	QNetworkReply  * createRequest(Operation op, const QNetworkRequest &request, QIODevice *outgoingData);
};

#endif // MYNETWORKACCESSMANAGER_H
