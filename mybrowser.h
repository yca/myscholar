#ifndef MYBROWSER_H
#define MYBROWSER_H

#include <QUrl>
#include <QHash>
#include <QWidget>
#include <QWebPage>

class QWebView;
class QListWidgetItem;

namespace Ui {
class MyBrowser;
}

class MyWebPage : public QWebPage
{
	Q_OBJECT
public:
	MyWebPage(QObject *parent = NULL);
	void triggerAction(WebAction act, bool checked = false);
signals:
	void newTabRequest(QUrl);
protected slots:
	void linkHoveredS(const QString &link, const QString &title, const QString &textContent);
protected:
	QString lastLink;
};

class MyBrowser : public QWidget
{
	Q_OBJECT

public:
	explicit MyBrowser(QWidget *parent = 0);
	~MyBrowser();
public slots:
	void addTab(const QUrl &url);
protected:
	bool eventFilter(QObject *obj, QEvent *ev);
	void closeEvent(QCloseEvent *ev);
private slots:
	void on_listTabs_itemDoubleClicked(QListWidgetItem *item);

	void on_tabWidget_currentChanged(int index);

	void on_toolSearchClose_clicked();

	void on_lineSearch_textEdited(const QString &arg1);

	void on_pushSearchNext_clicked();

	void on_pushSearchPrev_clicked();

	void on_lineSearch_returnPressed();

	void on_tabWidget_tabCloseRequested(int index);

private:
	void initSearch(QWebView *view);
	Ui::MyBrowser *ui;
	QHash<QUrl, int> urls;
};

#endif // MYBROWSER_H
