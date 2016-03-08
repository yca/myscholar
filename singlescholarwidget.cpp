#include "singlescholarwidget.h"
#include "ui_singlescholarwidget.h"

#include "scholar.h"
#include "pageparser.h"
#include "scholarsearcher.h"

#include <QDir>
#include <QDebug>
#include <QWebView>
#include <QMessageBox>
#include <QWebSettings>
#include <QWebInspector>

SingleScholarWidget::SingleScholarWidget(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::SingleScholarWidget),
	w(NULL)
{
	ui->setupUi(this);
	searcher = new ScholarSearcher;
	connect(searcher, SIGNAL(operationFinished()), SLOT(searchFinished()));
	connect(searcher, SIGNAL(operationProgress(int,int)), SLOT(searchProgress(int, int)));
	connect(searcher, SIGNAL(searchError(int)), SLOT(searchError(int)));
	ui->progressBar->setVisible(false);
}

SingleScholarWidget::~SingleScholarWidget()
{
	delete ui;
}

void SingleScholarWidget::searchError(int err)
{
	if (err == 0x1525) {
		showBrowser(&searcher->parser->page);
	} else {
		QMessageBox::information(this, trUtf8("Unhandled parse error"), trUtf8("Unhandled parse error %1").arg(err));
	}
}

void SingleScholarWidget::searchProgress(int current, int total)
{
	ui->progressBar->setVisible(true);
	ui->progressBar->setMaximum(total);
	ui->progressBar->setValue(current);
}

void SingleScholarWidget::searchFinished()
{
	Scholar *s = NULL;
	ScholarResultList *r = (ScholarResultList *)searcher->parser->parseResult;
	if (action == 1) {
		s = scholars.last();
		for (int i = 0; i < r->list2.size(); i++) {
			s->references << r->list2[i]->title;
			scholars << r->list2[i];
		}
		s->saveScholar();
	} else if (action == 2) {
		if (r->list2.size() > 1) {
			QMessageBox::warning(this, trUtf8("Multiple search results"), trUtf8("Found more than one result for this paper, using first"));
		} else if (!r->list2.size()) {
			QMessageBox::warning(this, trUtf8("No search result"), trUtf8("Found no result for this paper!"));
			return;
		}
		s = r->list2.first();
		/* we may have returned reference in our database */
		s->readScholar();
		scholars << s;
		s->saveScholar();
	} else if (action == 3) {
		s = scholars.last();
		for (int i = 0; i < searcher->scholars.size(); i++) {
			Scholar *s2 = searcher->scholars[i];
			s->citingTitles << s2->title;
			scholars << s2;

			/* check our local database before saving */
			Scholar s3;
			s3.setTitle(s2->title);
			int err = s3.readScholar();
			if (err == 0) {
				if (s2->externalLink.isEmpty())
					s2->externalLink = s3.externalLink;
				if (s2->citationsLink.isEmpty())
					s2->citationsLink = s3.citationsLink;
				if (s2->citedBy <= 0)
					s2->citedBy = s3.citedBy;
				if (s2->citingTitles.isEmpty())
					s2->citingTitles = s3.citingTitles;
				if (s2->references.isEmpty())
					s2->references = s3.references;
			}

			/* save */
			s2->saveScholar();
		}
		s->saveScholar();
	}
	if (s)
		showScholar(s);

	/* hide progressbar */
	ui->progressBar->setVisible(false);
}

void SingleScholarWidget::on_lineTitle_returnPressed()
{
	QString title = ui->lineTitle->text().trimmed();
	title = title.toLower();
	title.replace("\n", " ");
	title.replace("\r", " ");
	/* we first check our cache */
	Scholar *s = new Scholar;
	s->setTitle(title);
	int err = s->readScholar();
	if (err) {
		/* we don't have this in cache, let's search */
		action = 2;
		searcher->findScholar(title);
		delete s;
		return;
	}

	scholars << s;
	showScholar(scholars.last());
}

void SingleScholarWidget::showScholar(const Scholar *s)
{
	ui->lineSource->setText(s->externalLink);
	ui->plainCitedBy->setPlainText(s->citingTitles.join("\n"));
	ui->plainReferences->setPlainText(s->references.join("\n"));
	ui->labelCitedBy->setText(trUtf8("Cited by: (%1)").arg(s->citedBy));
	ui->labelReferences->setText(trUtf8("References: (%1)").arg(s->references.size()));
	ui->lineHash->setText(s->uniqueHash);
}

void SingleScholarWidget::on_pushFindReferences_clicked()
{
	action = 1;
	searcher->findReferences(scholars.last());
}

void SingleScholarWidget::on_pushDebug_clicked()
{
	QWebSettings::globalSettings()->setAttribute(QWebSettings::DeveloperExtrasEnabled, true);
	QWebInspector ins;
	ins.setPage(&searcher->parser->page);
	ins.show();
	showBrowser(&searcher->parser->page);
}

void SingleScholarWidget::on_pushFindCitedBy_clicked()
{
	action = 3;
	searcher->findCitations(scholars.last());
}

static QHash<QString, int> _hosts;
static bool lessThan(const QString &s1, const QString &s2)
{
	if (_hosts[s1] < _hosts[s2])
		return false;
	return true;
}

static bool lessThan2(const QPair<QString, int> &p1, const QPair<QString, int> &p2)
{
	if (p1.second < p2.second)
		return false;
	return true;
}

void SingleScholarWidget::on_pushDbStats_clicked()
{
	QDir d("scholars");
	QStringList files = d.entryList(QStringList() << "*.sxt", QDir::Files | QDir::NoDotAndDotDot);

	QStringList allTitles;
	QStringList complete;
	QStringList partialComplete;
	QHash<QString, int> hosts;
	QList<QPair<QString, int> > papers;
	foreach(QString f, files) {
		Scholar s;
		s.uniqueHash = f.split(".").first();
		s.readScholar();
		if (s.citingTitles.size() && s.references.size())
			complete << s.title;
		else if (s.citingTitles.size() || s.references.size())
			partialComplete << s.title;
		QUrl url(s.externalLink);
		hosts[url.host()]++;
		allTitles << s.title;
		papers << QPair<QString, int>(s.title, s.citedBy);
	}

	QStringList lines;
	lines << QString("%1 scholars").arg(files.size());
	lines << QString("%1 scholars complete").arg(complete.size());
	lines << QString("%1 scholars partial complete").arg(partialComplete.size());
	lines << QString("%1 different scholar databases:").arg(hosts.size());
	QStringList hostsSorted = hosts.keys();
	_hosts = hosts;
	qSort(hostsSorted.begin(), hostsSorted.end(), lessThan);
	foreach(QString h, hostsSorted) {
		lines << QString("\t%1: %2").arg(h).arg(hosts[h]);
		if (hosts[h] < 10)
			break;
	}

	/* most cited papers */
	qSort(papers.begin(), papers.end(), lessThan2);
	lines << QString("Most cited 100 papers:");
	for (int i = 0; i < 100; i++)
		lines << QString("\t%1: %2").arg(papers[i].first).arg(papers[i].second);

	/* check test file */
	if (QFile::exists("test.txt")) {
		lines << QString("analyzing test.txt:");
		QFile f("test.txt");
		if (f.open(QIODevice::ReadOnly)) {
			QStringList items = QString::fromUtf8(f.readAll()).split("\n");
			f.close();
			foreach (const QString &item, items) {
				if (allTitles.contains(item.toLower(), Qt::CaseInsensitive))
					lines << QString("\t%1 : 1").arg(item);
				else
					lines << QString("\t%1 : 0").arg(item);
			}
		}
	}

	/* visualize */
	QPlainTextEdit edit;
	edit.resize(500, 400);
	edit.setPlainText(lines.join("\n"));
	edit.show();
	while (edit.isVisible())
		QApplication::processEvents();
}

void SingleScholarWidget::showBrowser(QWebPage *page)
{
	if (w == NULL) {
		w = new QWebView;
		//w->setWindowModality(Qt::ApplicationModal);
	}
	w->setPage(page);
	w->show();
}
