#ifndef SCHOLAR_H
#define SCHOLAR_H

#include <QFile>
#include <QString>
#include <QStringList>
#include <QCryptographicHash>

class Scholar
{
public:
	Scholar();
	void setTitle(QString title);
	void write(QIODevice *dev) const;
	void read(QIODevice *dev);
	int readScholar(const QString &path = "scholars");
	int saveScholar(const QString &path = "scholars");
	static QList<Scholar *> readScholars(const QStringList &titles, const QString &path = "scholars");

	QString title;
	int citedBy;
	QString externalLink;
	QString citationsLink;
	bool queryMatch;
	QString uniqueHash;
	QStringList citingTitles;
	QStringList references;
	QString publicationDate;
};

#endif // SCHOLAR_H
