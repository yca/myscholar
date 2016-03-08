#include "querywidget.h"
#include "ui_querywidget.h"
#include "mybrowser.h"
#include "scholar.h"
#include "mybookmarks.h"

#include <QDir>
#include <QMenu>
#include <QDebug>
#include <QComboBox>
#include <QClipboard>
#include <QInputDialog>
#include <QPlainTextEdit>

#define COLOR_READ Qt::darkGreen
#define COLOR_SEEN Qt::darkBlue

static bool lessThan2(const QPair<QString, int> &p1, const QPair<QString, int> &p2)
{
	if (p1.second < p2.second)
		return false;
	return true;
}

static bool greaterThan(const QPair<QString, int> &p1, const QPair<QString, int> &p2)
{
	if (p1.second > p2.second)
		return false;
	return true;
}

static QString getNote(const QStringList &cats, QString &cat, const QString &init = "")
{
	QFrame f;
	f.setLayout(new QVBoxLayout());
	QComboBox combo(&f);
	combo.addItems(cats);
	combo.setEditable(true);
	f.layout()->addWidget(&combo);
	QPlainTextEdit plain(&f);
	plain.setPlainText(init);
	f.layout()->addWidget(&plain);
	f.show();
	f.setWindowModality(Qt::ApplicationModal);
	while (f.isVisible())
		QApplication::processEvents();
	cat = combo.currentText();
	return plain.toPlainText();
}

static QString getNote(const QString &init = "")
{
	QString cat;
	return getNote(QStringList(), cat, init);
}

QueryWidget::QueryWidget(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::QueryWidget),
	browser(NULL)
{
	ui->setupUi(this);
	ui->listWidget->setEditTriggers(QAbstractItemView::EditKeyPressed | QAbstractItemView::DoubleClicked);
	ui->listWidget->setContextMenuPolicy(Qt::CustomContextMenu);

	loadSorted(1);
#if 0
	QDir d("scholars");
	QStringList files = d.entryList(QStringList() << "*.sxt", QDir::Files | QDir::NoDotAndDotDot);

	foreach(QString f, files) {
		Scholar *s = new Scholar;
		s->uniqueHash = f.split(".").first();
		s->readScholar();
		papers << QPair<QString, int>(s->title, s->citedBy);
		//papers << QPair<QString, int>(s->title, s->publicationDate.toInt());
		scholars.insert(s->title, s);
		scholarsByHash.insert(s->uniqueHash, s);
	}
	qSort(papers.begin(), papers.end(), lessThan2);
	for (int i = 0; i < papers.size(); i++)
		allTitles << papers[i].first;
	ui->labelStat_1->setText(QString("Total of %1 scholars").arg(scholars.size()));
	filter("");
#endif
}

QueryWidget::~QueryWidget()
{
	delete ui;
}

void QueryWidget::on_lineQuery_textChanged(const QString &arg1)
{
	filter(arg1.trimmed());
}

int QueryWidget::filter(QString text)
{
	if (text.startsWith("!"))
		return filterByHash(text.mid(1));
	else if (text.startsWith("@@"))
		return filterByRead(true, true);
	else if (text.startsWith("@"))
		return filterByRead(true, false);
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

	showItems(list);

	return 0;
}

int QueryWidget::filterByHash(const QString &hash)
{
	QStringList list;
	QHashIterator<QString, Scholar *> i(scholars);
	while (i.hasNext()) {
		i.next();
		Scholar *s = i.value();
		if (s->uniqueHash == hash)
			list << s->title;
	}
	showItems(list);
	return 0;
}

int QueryWidget::filterByRead(bool read, bool seen)
{
	QStringList list;
	QHashIterator<QString, Scholar *> i(scholars);
	while (i.hasNext()) {
		i.next();
		Scholar *s = i.value();
		if (read && MyBookmarks::contains("readby", s->uniqueHash, "caglar"))
			list << s->title;
		if (seen && MyBookmarks::contains("seenby", s->uniqueHash, "caglar"))
			list << s->title;
	}
	showItems(list);
	return 0;
}

void QueryWidget::showItems(const QStringList &list)
{
	ui->listWidget->clear();
	ui->listWidget->addItems(list);
	for (int i = 0; i < ui->listWidget->count(); i++) {
		Scholar *s = scholars[ui->listWidget->item(i)->text()];
		if (MyBookmarks::contains("readby", s->uniqueHash, "caglar"))
			ui->listWidget->item(i)->setForeground(COLOR_READ);
		else if (MyBookmarks::contains("seenby", s->uniqueHash, "caglar"))
			ui->listWidget->item(i)->setForeground(COLOR_SEEN);
	}
	ui->labelStat_2->setText(QString("Showing %1 scholars").arg(list.size()));
}

void QueryWidget::loadSorted(int flags)
{
	QDir d("scholars");
	QStringList files = d.entryList(QStringList() << "*.sxt", QDir::Files | QDir::NoDotAndDotDot);
	scholars.clear();
	scholarsByHash.clear();
	allTitles.clear();

	foreach(QString f, files) {
		Scholar *s = new Scholar;
		s->uniqueHash = f.split(".").first();
		s->readScholar();
		if (flags == 1)
			papers << QPair<QString, int>(s->title, s->citedBy);
		else if (flags == 2)
			papers << QPair<QString, int>(s->title, s->publicationDate.toInt());
		scholars.insert(s->title, s);
		scholarsByHash.insert(s->uniqueHash, s);
	}
	if (flags == 1)
		qSort(papers.begin(), papers.end(), lessThan2);
	else if (flags == 2)
		qSort(papers.begin(), papers.end(), greaterThan);
	for (int i = 0; i < papers.size(); i++)
		allTitles << papers[i].first;
	ui->labelStat_1->setText(QString("Total of %1 scholars").arg(scholars.size()));
	filter("");
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
		openBrowser(item->text());
	}
}

void QueryWidget::on_listWidget_customContextMenuRequested(const QPoint &pos)
{
	QListWidgetItem *item = ui->listWidget->itemAt(pos);
	if (!item)
		return;
	Scholar *s = scholars[item->text()];
	QMenu m;
	m.addAction("Open Browser(Ctrl + Enter)");
	m.addAction("Bookmark");
	m.addAction("Quote");
	m.addAction("Read");
	m.addAction("Tag");
	m.addAction("Mark as read");
	m.addAction("Mark as seen");
	m.addAction("Copy link address");
	QAction *act = m.exec(ui->listWidget->mapToGlobal(pos));
	if (!act)
		return;
	if (act->text().contains("Browser"))
		openBrowser(item->text());
	else if (act->text().contains("Bookmark"))
		MyBookmarks::add(item->text());
	else if (act->text().contains("Quote")) {
		QString quote = getNote();
		if (!quote.isEmpty()) {
			MyBookmarks::addQuote(s->uniqueHash, quote);
		}
	} else if (act->text().contains("Mark as read")) {
		MyBookmarks::addToGroup("readby", s->uniqueHash, "caglar");
		item->setForeground(COLOR_READ);
	}else if (act->text().contains("Mark as seen")) {
		MyBookmarks::addToGroup("seenby", s->uniqueHash, "caglar");
		item->setForeground(COLOR_SEEN);
	} else if (act->text().contains("Read")) {
		MyBookmarks::addToGroup("read", s->title);
	} else if (act->text().contains("Tag")) {
		QString cat;
		QString t = getNote(MyBookmarks::getGroups(), cat, s->uniqueHash);
		if (!t.isEmpty())
			MyBookmarks::addToGroup(cat, t);
	} else if (act->text().contains("Copy link address")) {
		QClipboard *c = QApplication::clipboard();
		c->setText(s->externalLink);
	}
}

void QueryWidget::on_pushBookmarks_clicked()
{
	QString text;
	QStringList keys;
	QHash<QString, QStringList> quotes = MyBookmarks::getQuotes();
	keys = quotes.keys();
	QHashIterator<QString, QStringList> i(quotes);
	while (i.hasNext()) {
		i.next();
		QStringList l = i.value();
		foreach (QString q, l) {
			text.append((q.replace("\n", " ").append(QString("[%1]\n\n").arg(keys.indexOf(i.key()) + 1))));
		}
	}
	for (int i = 0; i < keys.size(); i++)
		text.append(QString("[%1] %2\n").arg(i + 1).arg(scholarsByHash[keys[i]]->title));
	getNote(text);
}

void QueryWidget::on_pushNote_clicked()
{
	QString cat;
	QString t = getNote(MyBookmarks::getGroups(), cat);
	if (t.isEmpty())
		return;
	MyBookmarks::addToGroup(cat, t);
}

void QueryWidget::openBrowser(const QString &text)
{
	if (!browser)
		browser = new MyBrowser;
	Scholar *s = scholars[text];
	browser->addTab(s->externalLink);
	browser->show();
}

void QueryWidget::on_comboSort_activated(int index)
{
	loadSorted(index + 1);
}
