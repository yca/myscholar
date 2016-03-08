#ifndef PAGEPARSER_H
#define PAGEPARSER_H

#include <QUrl>
#include <QFile>
#include <QObject>
#include <QWebPage>
#include <QVariant>
#include <QNetworkProxy>

#include "scholar.h"

class QNetworkReply;
class QAuthenticator;

class ParseResult
{
public:
	virtual QVariant get(const QString &str) = 0;
	virtual ~ParseResult() {}
};

class ScholarResultList : public ParseResult
{
public:
	virtual QVariant get(const QString &str)
	{
		Q_UNUSED(str);
		return QVariant();
	}

	QList<Scholar> list;
	QList<Scholar *> list2;
	QString nextLink;
};

class HashResult : public ParseResult
{
public:
	virtual QVariant get(const QString &str)
	{
		return r[str];
	}

	QHash<QString, QVariant> r;
};

class PageParser : public QObject
{
	Q_OBJECT
public:
	explicit PageParser(QObject *parent = 0);
	int parseUrl(QUrl url);
	virtual ~PageParser() {}

	ParseResult *parseResult;
	QWebPage page;
signals:
	void parseFinished();
	void parseError(int);
public slots:
	void pageLoadStarted();
	void pageLoadFinished(bool ok);
	void nmFinished(QNetworkReply *reply);
	void pageFrameCreated(QWebFrame*frame);
	void proxyAuthenticationRequired(QNetworkProxy, QAuthenticator *);
protected:
	virtual int reparse() = 0;
	virtual void parseNext() {}

	bool loading;
	bool parseDone;
};

class GoogleScholarParser : public PageParser
{
	Q_OBJECT
public:
	explicit GoogleScholarParser(QObject *parent = 0);
	virtual int reparse();
};

class IEEExploreParser : public PageParser
{
	Q_OBJECT
public:
	virtual int reparse();
};

class SpringerParser : public PageParser
{
	Q_OBJECT
public:
	virtual int reparse();
};

class AcmParser : public PageParser
{
	Q_OBJECT
public:
	virtual int reparse();
};

class SciencedirectParser : public PageParser
{
	Q_OBJECT
public:
	explicit SciencedirectParser(QObject *parent = 0);
	virtual int reparse();
};

#endif // PAGEPARSER_H
