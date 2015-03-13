#ifndef SCHOLARSEARCHER_H
#define SCHOLARSEARCHER_H

#include "longoperation.h"

class Scholar;
class PageParser;

class ScholarSearcher : public LongOperation
{
	Q_OBJECT
public:
	explicit ScholarSearcher(QObject *parent = 0);
	int findReferences(Scholar *s);
	int findCitations(Scholar *s);
	int findScholar(const QString &title);

	PageParser *parser;
	QList<Scholar *> scholars;

signals:
	void searchError(int);
public slots:
	void parseError(int err);
	void scholarSearchFinished();
	void citationSearchFinished();
	void referenceSearchFinished();
protected:
	int startSearch(int total);

};

#endif // SCHOLARSEARCHER_H
