#include "scholarsearcher.h"
#include "pageparser.h"
#include "scholar.h"

#include <QDebug>

ScholarSearcher::ScholarSearcher(QObject *parent) :
	LongOperation(parent)
{
	parser = NULL;
}

int ScholarSearcher::findReferences(Scholar *s)
{
	if (parser)
		delete parser;
	QUrl url(s->externalLink);
	if (s->externalLink.contains("ieeexplore.ieee.org")) {
		parser = new IEEExploreParser();
		QString arnumber = s->externalLink.split("arnumber=").last();
		url = QString("http://ieeexplore.ieee.org/xpl/abstractReferences.jsp?arnumber=%1").arg(arnumber);
	} else if (s->externalLink.contains("springer.com")) {
		parser = new SpringerParser();
	}
	int err = parser->parseUrl(url);
	if (err)
		return err;
	connect(parser, SIGNAL(parseFinished()), SLOT(referenceSearchFinished()));
	return startSearch(1);
}

int ScholarSearcher::findCitations(Scholar *s)
{
	//if (parser)
		//delete parser;
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
	url.addQueryItem("q", title);
	int err = parser->parseUrl(url);
	if (err)
		return err;

	connect(parser, SIGNAL(parseFinished()), SLOT(scholarSearchFinished()));
	return startSearch(1);
}

void ScholarSearcher::parseError(int err)
{
	emit searchError(err);
}

void ScholarSearcher::scholarSearchFinished()
{
	operationStep(1);
}

void ScholarSearcher::citationSearchFinished()
{
	ScholarResultList *r = (ScholarResultList *)parser->parseResult;
	scholars << r->list2;
	operationStep(scholars.size());
	if (r->nextLink.isEmpty()) {
		operationStep(-1);
	} else
		parser->parseUrl(QUrl(QString("http://scholar.google.com%1").arg(r->nextLink)));

}

void ScholarSearcher::referenceSearchFinished()
{
	operationStep(1);
}

int ScholarSearcher::startSearch(int total)
{
	connect(parser, SIGNAL(parseError(int)), SLOT(parseError(int)));
	operationStart(total);
	return 0;
}
