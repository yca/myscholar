#ifndef MYBROWSER_H
#define MYBROWSER_H

#include <QUrl>
#include <QHash>
#include <QWidget>

class QListWidgetItem;

namespace Ui {
class MyBrowser;
}

class MyBrowser : public QWidget
{
	Q_OBJECT

public:
	explicit MyBrowser(QWidget *parent = 0);
	void addTab(const QUrl &url);
	~MyBrowser();
protected:
	void closeEvent(QCloseEvent *ev);
private slots:
	void on_listTabs_itemDoubleClicked(QListWidgetItem *item);

	void on_tabWidget_currentChanged(int index);

private:
	Ui::MyBrowser *ui;
	QHash<QUrl, int> urls;
};

#endif // MYBROWSER_H
