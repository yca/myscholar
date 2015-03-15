#include "querywidget.h"
#include "ui_querywidget.h"
#include "mybrowser.h"
#include "scholar.h"
#include "mybookmarks.h"

#include <QDir>
#include <QMenu>
#include <QDebug>
#include <QInputDialog>

static bool lessThan2(const QPair<QString, int> &p1, const QPair<QString, int> &p2)
{
	if (p1.second < p2.second)
		return false;
	return true;
}

QueryWidget::QueryWidget(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::QueryWidget),
	browser(NULL)
{
	ui->setupUi(this);
	ui->listWidget->setEditTriggers(QAbstractItemView::EditKeyPressed | QAbstractItemView::DoubleClicked);
	ui->listWidget->setContextMenuPolicy(Qt::CustomContextMenu);

	QDir d("scholars");
	QStringList files = d.entryList(QStringList() << "*.sxt", QDir::Files | QDir::NoDotAndDotDot);

	foreach(QString f, files) {
		Scholar *s = new Scholar;
		s->uniqueHash = f.split(".").first();
		s->readScholar();
		papers << QPair<QString, int>(s->title, s->citedBy);
		scholars.insert(s->title, s);
	}
	qSort(papers.begin(), papers.end(), lessThan2);
	for (int i = 0; i < papers.size(); i++)
		allTitles << papers[i].first;
	ui->labelStat_1->setText(QString("Total of %1 scholars").arg(scholars.size()));
	filter("");
}

QueryWidget::~QueryWidget()
{
	delete ui;
}

void QueryWidget::on_lineQuery_textChanged(const QString &arg1)
{
	filter(arg1.trimmed());
}

void QueryWidget::filter(QString text)
{
	QStringList list = allTitles;
	if (!text.isEmpty()) {
		if (text.contains("||")) {
			QStringList qs = text.split("||", QString::SkipEmptyParts);
			list.clear();
			foreach (const QString &q, qs)
				list << allTitles.filter(q.trimmed(), Qt::CaseInsensitive);
		} else if (text.contains("&&")) {
			QStringList qs = text.split("&&", QString::SkipEmptyParts);
			foreach (const QString &q, qs)
				list = list.filter(q.trimmed(), Qt::CaseInsensitive);
		} else
			list = allTitles.filter(text, Qt::CaseInsensitive);
	}

	ui->listWidget->clear();
	ui->listWidget->addItems(list);
	ui->labelStat_2->setText(QString("Showing %1 scholars").arg(list.size()));
}

void QueryWidget::on_listWidget_currentRowChanged(int currentRow)
{
	if (currentRow < 0)
		return;
	QString title = ui->listWidget->item(currentRow)->text();
	Scholar *s = scholars[title];
	ui->labelStat_3->setText(QString("Cited by: %1").arg(s->citedBy));
}

void QueryWidget::on_listWidget_itemActivated(QListWidgetItem *item)
{
	if (QApplication::keyboardModifiers() & Qt::ControlModifier) {
		if (!browser)
			browser = new MyBrowser;
		Scholar *s = scholars[item->text()];
		browser->addTab(s->externalLink);
		browser->show();
	}
}

void QueryWidget::on_listWidget_customContextMenuRequested(const QPoint &pos)
{
	QListWidgetItem *item = ui->listWidget->itemAt(pos);
	if (!item)
		return;
	QMenu m;
	m.addAction("Open Browser(Ctrl + Enter)");
	m.addAction("Bookmark");
	m.addAction("Quote");
	QAction *act = m.exec(pos);
	if (!act)
		return;
	if (act->text().contains("Browser"))
		on_listWidget_itemActivated(item);
	else if (act->text().contains("Bookmark"))
		MyBookmarks::add(item->text());
	else if (act->text().contains("Quote")) {
		QString quote = QInputDialog::getText(this, trUtf8("Quote"), trUtf8("Please enter quote text:"));
		if (!quote.isEmpty()) {
			Scholar *s = scholars[item->text()];
			MyBookmarks::addQuote(s->uniqueHash, quote);
		}
	}
}

void QueryWidget::on_pushBookmarks_clicked()
{

}

void QueryWidget::on_pushNote_clicked()
{
	QString t = QInputDialog::getText(this, trUtf8("Note input"), trUtf8("Please enter your note text:"));
	if (t.isEmpty())
		return;
	MyBookmarks::add("notes", t);
}
