#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "pageparser.h"

#include <QDebug>
#include <QWebView>

#if 1

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	ui->progressBar->setVisible(false);
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::singleSearchFinished()
{
	GoogleScholarParser *p = (GoogleScholarParser *)sender();

	if (!p->parseResult) {
		atext(QString("No parse result"));
		return;
	}

	ScholarResultList *r  = (ScholarResultList *)p->parseResult;
	ui->plainSingleSearch->appendPlainText(QString("Found %1 results").arg(r->list.size()));
	for (int i = 0; i < r->list.size(); i++) {
		const Scholar &s = r->list[i];
		atext(QString("Result %1").arg(i + 1));
		atext(QString("\tExternal link: %1").arg(s.externalLink));
		atext(QString("\tCitations link: %1").arg(s.citationsLink));
		atext(QString("\tCited by: %1").arg(s.citedBy));
		atext(QString("\tQuery match: %1").arg(s.queryMatch));
		if (s.queryMatch)
			currScholar = s;
	}

	ui->pushCitations->setEnabled(true);
	curr = p;
}

void MainWindow::citationsNextFinished()
{
	ScholarResultList *r  = (ScholarResultList *)curr->parseResult;
	for (int i = 0; i < r->list.size(); i++) {
		const Scholar &s = r->list[i];
		currScholar.citingTitles << s.title;
		scholarList << s;

		/* save result */
		QFile f(QString("scholars/%1.sxt").arg(s.uniqueHash));
		if (f.open(QIODevice::WriteOnly)) {
			s.write(&f);
			f.close();
		}
	}

	if (r->list.size() == 0) {
		qDebug() << "error reading results";
		ui->plainSingleSearch->appendPlainText(QString("Citations fetch error"));
		return;
	}
	ui->progressBar->setMaximum(currScholar.citedBy);
	ui->progressBar->setValue(currScholar.citingTitles.size());
	ui->progressBar->setVisible(true);

	curr->parseUrl(QUrl(QString("http://scholar.google.com%1").arg(r->nextLink)));
}

void MainWindow::on_pushSingleSearch_clicked()
{
	/* we first check our cache */
	Scholar s;
	s.setTitle(ui->lineQuery->text());
	if (QFile::exists(QString("scholars/%1.sxt").arg(s.uniqueHash))) {
		QFile f(QString("scholars/%1.sxt").arg(s.uniqueHash));
		f.open(QIODevice::ReadOnly);
		s.read(&f);
		f.close();

		for (int i = 0; i < s.citingTitles.size(); i++) {
			//qDebug() << "reading" << i << s.citedBy << s.citingTitles.size();
			QString t = s.citingTitles[i];
			Scholar c;
			c.setTitle(t);
			c.readScholar();
			scholarList << c;
		}
		currScholar = s;
		return;
	}

	if (ui->comboSource->currentIndex() == 0) {
		GoogleScholarParser *p = new GoogleScholarParser();
		QUrl url("http://scholar.google.com/scholar");
		url.addQueryItem("q", ui->lineQuery->text());
		ui->plainSingleSearch->clear();
		int err = p->parseUrl(url);
		if (err) {
			atext(QString("Parse error %1").arg(err));
		} else {
			atext(QString("Started parsing..."));
			connect(p, SIGNAL(parseFinished()), SLOT(singleSearchFinished()));
		}
	} else if (ui->comboSource->currentIndex() == 1) {

	}
}

void MainWindow::on_pushCitations_clicked()
{
	ScholarResultList *r  = (ScholarResultList *)curr->parseResult;
	for (int i = 0; i < r->list.size(); i++) {
		if (!r->list[i].queryMatch)
			continue;
		QUrl url(QString("http://scholar.google.com%1").arg(r->list[i].citationsLink));
		disconnect(curr, SIGNAL(parseFinished()), this, SLOT(singleSearchFinished()));
		connect(curr, SIGNAL(parseFinished()), SLOT(citationsNextFinished()));
		curr->parseUrl(url);
	}
}

void MainWindow::on_pushShowPage_clicked()
{
	QWebView w;
	w.setPage(&curr->page);
	w.show();
	while (w.isVisible())
		QApplication::processEvents();
}

void MainWindow::on_pushSaveCurrent_clicked()
{
	QFile f(QString("scholars/%1.sxt").arg(currScholar.uniqueHash));
	if (f.open(QIODevice::WriteOnly)) {
		currScholar.write(&f);
		f.close();
	}
	qDebug() << "current scholar is" << f.fileName();
}

void MainWindow::on_pushScholarInfo_clicked()
{
	QMap<QString, int> hosts;
	for (int i = 0; i < scholarList.size(); i++) {
		const Scholar &s = scholarList[i];
		QUrl url(s.externalLink);
		hosts[url.host()]++;
	}
	atext(QString("Found %1 scholar hosts").arg(hosts.size()));
	QMapIterator<QString, int> i(hosts);
	while(i.hasNext()) {
		i.next();
		atext(QString("\t%1: %2").arg(i.key()).arg(i.value()));
	}
}

void MainWindow::atext(const QString &s)
{
	ui->plainSingleSearch->appendPlainText(s);
}

void MainWindow::on_comboSource_activated(int index)
{
	if (index == 0) {
		ui->pushCitations->setEnabled(true);
		ui->pushSaveCurrent->setEnabled(true);
		ui->pushScholarInfo->setEnabled(true);
		ui->pushReferences->setEnabled(false);
	} else {
		ui->pushCitations->setEnabled(false);
		ui->pushSaveCurrent->setEnabled(false);
		ui->pushScholarInfo->setEnabled(false);
		ui->pushReferences->setEnabled(true);
	}
}

void MainWindow::on_pushReferences_clicked()
{

}

#endif
