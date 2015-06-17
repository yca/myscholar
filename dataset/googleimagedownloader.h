#ifndef GOOGLEIMAGEDOWNLOADER_H
#define GOOGLEIMAGEDOWNLOADER_H

#include "pageparser.h"

class GoogleImageDownloader : public PageParser
{
	Q_OBJECT
public:
	explicit GoogleImageDownloader(QObject *parent = 0);
	int search(const QString &query);
signals:

public slots:
protected:
	virtual int reparse();
};

#endif // GOOGLEIMAGEDOWNLOADER_H
