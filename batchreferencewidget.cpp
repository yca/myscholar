#include "batchreferencewidget.h"
#include "ui_batchreferencewidget.h"

#include "scholar.h"
#include "pageparser.h"
#include "scholarsearcher.h"

#include <QDir>
#include <QDebug>
#include <QTimer>
#include <QWebView>
#include <QMessageBox>
#include <QWebSettings>
#include <QWebInspector>

BatchReferenceWidget::BatchReferenceWidget(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::BatchReferenceWidget),
	w(NULL)
{
	ui->setupUi(this);
	searcher = new ScholarSearcher;
	connect(searcher, SIGNAL(operationFinished()), SLOT(searchFinished()));
	connect(searcher, SIGNAL(operationProgress(int,int)), SLOT(searchProgress(int, int)));
	connect(searcher, SIGNAL(searchError(int)), SLOT(searchError(int)));
	ui->progressBar->setVisible(false);
}

BatchReferenceWidget::~BatchReferenceWidget()
{
	delete ui;
}

void BatchReferenceWidget::searchNext()
{
	if (++current < scholars.size()) {
		int err = searcher->findReferences(scholars[current]);
		if (err) {
			ui->plainUnsupported->appendPlainText(QString("%1").arg(scholars[current]->uniqueHash));
			searchNext();
			return;
		} else {
			ui->labelCurrent->setText(scholars[current]->uniqueHash);
			ui->progressBar->setValue(current);
		}
	} else
		QMessageBox::information(this, trUtf8("Operation completed"), trUtf8("All references finished"));
}

void BatchReferenceWidget::searchError(int err)
{
	if (err == 0x1525) {
		showBrowser(&searcher->parser->page);
	} else {
		QMessageBox::information(this, trUtf8("Unhandled parse error"), trUtf8("Unhandled parse error %1").arg(err));
		ui->plainError->appendPlainText(QString("%1: %2").arg(scholars[current]->uniqueHash).arg(err));
		handleNext();
	}
}

void BatchReferenceWidget::searchProgress(int, int)
{
}

void BatchReferenceWidget::searchFinished()
{
	ScholarResultList *r = (ScholarResultList *)searcher->parser->parseResult;
	Scholar *s = scholars.first();
	for (int i = 0; i < r->list2.size(); i++) {
		s->references << r->list2[i]->title;
		scholars << r->list2[i];
	}
	if (r->list2.size()) {
		ui->plainCompleted->appendPlainText(QString("%1: %2").arg(scholars[current]->uniqueHash).arg(r->list2.size()));
		s->saveScholar();
	} else
		ui->plainEmpty->appendPlainText(QString("%1").arg(scholars[current]->uniqueHash));
	handleNext();
}

void BatchReferenceWidget::on_pushSearchMissing_clicked()
{
	QDir d("scholars");
	QStringList files = d.entryList(QStringList() << "*.sxt", QDir::Files | QDir::NoDotAndDotDot);
	foreach(QString f, files) {
		Scholar *s = new Scholar;
		s->uniqueHash = f.split(".").first();
		s->readScholar();
		if (s->references.size()) {
			delete s;
			continue;
		}
		//searchTitles << s.title;
		scholars << s;
	}
	ui->progressBar->setMaximum(scholars.size());
	ui->progressBar->setVisible(true);
	current = -1;
	searchNext();
}

void BatchReferenceWidget::showBrowser(QWebPage *page)
{
	if (w == NULL) {
		w = new QWebView;
		w->setWindowModality(Qt::ApplicationModal);
	}
	w->setPage(page);
	w->show();
}

void BatchReferenceWidget::handleNext()
{
	QTimer::singleShot(0, this, SLOT(searchNext()));
}
