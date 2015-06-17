#include "googleimagedownloader.h"

#include <QDebug>
#include <QWebFrame>
#include <QWebElementCollection>

GoogleImageDownloader::GoogleImageDownloader(QObject *parent) :
	PageParser(parent)
{
}

int GoogleImageDownloader::search(const QString &query)
{
	//return parseUrl(QUrl(QString("https://www.google.com.tr/search?tbm=isch&q=%1").arg(query)));
	return parseUrl(QUrl(QString("http://www.flickr.com/search/?text=%1").arg(query)));
}

int GoogleImageDownloader::reparse()
{
	qDebug() << "done" << page.mainFrame()->url();

	/* This is different than what you see in firefox/chrome */
	QWebElement table = page.mainFrame()->findFirstElement("table.images_table");
	QWebElementCollection images = table.findAll("a");
	foreach (QWebElement im, images)
		qDebug() << im.attribute("href");

	return 0;
}
