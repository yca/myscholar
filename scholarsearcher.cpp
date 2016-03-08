#include "scholarsearcher.h"
#include "pageparser.h"
#include "scholar.h"

#include <QDebug>
#include <QApplication>
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
#   include <QUrlQuery>
#endif

#include <errno.h>

ScholarSearcher::ScholarSearcher(QObject *parent) :
	LongOperation(parent)
{
	parser = NULL;
}

int ScholarSearcher::findReferences(Scholar *s)
{
	if (parser) {
		parser->deleteLater();
		parser = NULL;
	}
	QUrl url(s->externalLink);
	if (s->externalLink.contains("ieeexplore.ieee.org")) {
		parser = new IEEExploreParser();
		QString arnumber = s->externalLink.split("arnumber=").last();
		url = QString("http://ieeexplore.ieee.org/xpl/abstractReferences.jsp?arnumber=%1").arg(arnumber);
	} else if (s->externalLink.contains("springer.com")) {
		parser = new SpringerParser();
	} else if (s->externalLink.contains("dl.acm.org")) {
		parser = new AcmParser();
		QString id = s->externalLink.split("id=").last();
		url = QString("http://dl.acm.org/citation.cfm?id=%1&preflayout=flat").arg(id);
	} else if (s->externalLink.contains("www.sciencedirect.com")) {
		parser = new SciencedirectParser;
	} else
		return -EINVAL;
	int err = parser->parseUrl(url);
	if (err)
		return err;
	connect(parser, SIGNAL(parseFinished()), SLOT(referenceSearchFinished()));
	return startSearch(1);
}

int ScholarSearcher::findCitations(Scholar *s)
{
	if (parser)
		delete parser;
	parser = new GoogleScholarParser();
	QUrl url(QString("http://scholar.google.com%1").arg(s->citationsLink));
	int err = parser->parseUrl(url);
	if (err)
		return err;

	connect(parser, SIGNAL(parseFinished()), SLOT(citationSearchFinished()));
	return startSearch(s->citedBy);
}

int ScholarSearcher::findScholar(const QString &title)
{
	if (parser)
		delete parser;
	parser = new GoogleScholarParser();
	QUrl url("http://scholar.google.com/scholar");
	#if QT_VERSION < QT_VERSION_CHECK(5,0,0)
	url.addQueryItem("q", title);
	#else
	QUrlQuery q;
	q.addQueryItem("q", title);
	url.setQuery(q);
	#endif
	int err = parser->parseUrl(url);
	if (err)
		return err;

	connect(parser, SIGNAL(parseFinished()), SLOT(scholarSearchFinished()));
	return startSearch(1);
}

void ScholarSearcher::parseError(int err)
{
	QApplication::setOverrideCursor(QCursor(Qt::ArrowCursor));
	emit searchError(err);
}

void ScholarSearcher::scholarSearchFinished()
{
	QApplication::setOverrideCursor(QCursor(Qt::ArrowCursor));
	operationStep(1);
}

void ScholarSearcher::citationSearchFinished()
{
	ScholarResultList *r = (ScholarResultList *)parser->parseResult;
	scholars << r->list2;
	operationStep(scholars.size());
	if (1 || r->nextLink.isEmpty()) {
		operationStep(-1);
		QApplication::setOverrideCursor(QCursor(Qt::ArrowCursor));
	} else
		parser->parseUrl(QUrl(QString("http://scholar.google.com%1").arg(r->nextLink)));
}

void ScholarSearcher::referenceSearchFinished()
{
	QApplication::setOverrideCursor(QCursor(Qt::ArrowCursor));
	operationStep(1);
}

int ScholarSearcher::startSearch(int total)
{
	connect(parser, SIGNAL(parseError(int)), SLOT(parseError(int)));
	operationStart(total);
	QApplication::setOverrideCursor(QCursor(Qt::BusyCursor));
	return 0;
}
