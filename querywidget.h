#ifndef QUERYWIDGET_H
#define QUERYWIDGET_H

#include <QHash>
#include <QWidget>
#include <QStringList>

class Scholar;
class MyBrowser;
class QListWidgetItem;

namespace Ui {
class QueryWidget;
}

class QueryWidget : public QWidget
{
	Q_OBJECT

public:
	explicit QueryWidget(QWidget *parent = 0);
	~QueryWidget();

private slots:
	void on_lineQuery_textChanged(const QString &arg1);

	void on_listWidget_currentRowChanged(int currentRow);

	void on_listWidget_itemActivated(QListWidgetItem *item);

	void on_listWidget_customContextMenuRequested(const QPoint &pos);

	void on_pushBookmarks_clicked();

	void on_pushNote_clicked();

	void on_comboSort_activated(int index);

private:
	void openBrowser(const QString &text);
	int filter(QString text);
	int filterByHash(const QString &hash);
	int filterByRead(bool read, bool seen);
	void showItems(const QStringList &list);
	void loadSorted(int flags);

	Ui::QueryWidget *ui;
	QStringList allTitles;
	QHash<QString, Scholar *> scholars;
	QList<QPair<QString, int> > papers;
	QHash<QString, Scholar *> scholarsByHash;
	MyBrowser *browser;
};

#endif // QUERYWIDGET_H
