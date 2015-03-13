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
			QMessageBox::warning(this, trUtf8("Multiple search results"), trUtf8("Found more than one result for this paper!"));
			return;
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
			s->citingTitles << searcher->scholars[i]->title;
			scholars << searcher->scholars[i];
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

void SingleScholarWidget::on_pushDbStats_clicked()
{
	QDir d("scholars");
	QStringList files = d.entryList(QStringList() << "*.sxt", QDir::Files | QDir::NoDotAndDotDot);

	QStringList complete;
	QStringList partialComplete;
	QHash<QString, int> hosts;
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
	}
	QPlainTextEdit edit;
	edit.setPlainText(lines.join("\n"));
	edit.show();
	while (edit.isVisible())
		QApplication::processEvents();
}

void SingleScholarWidget::showBrowser(QWebPage *page)
{
	if (w == NULL) {
		w = new QWebView;
		w->setWindowModality(Qt::ApplicationModal);
	}
	w->setPage(page);
	w->show();
}
