#ifndef SINGLESCHOLARWIDGET_H
#define SINGLESCHOLARWIDGET_H

#include <QWidget>

class Scholar;
class QWebPage;
class QWebView;
class ScholarSearcher;

namespace Ui {
class SingleScholarWidget;
}

class SingleScholarWidget : public QWidget
{
	Q_OBJECT

public:
	explicit SingleScholarWidget(QWidget *parent = 0);
	~SingleScholarWidget();

private slots:
	void searchError(int err);
	void searchProgress(int current, int total);
	void searchFinished();
	void on_lineTitle_returnPressed();

	void on_pushFindReferences_clicked();

	void on_pushDebug_clicked();

	void on_pushFindCitedBy_clicked();

	void on_pushDbStats_clicked();

private:
	void showBrowser(QWebPage *page);
	void showScholar(const Scholar *s);

	Ui::SingleScholarWidget *ui;
	QList<Scholar *> scholars;
	ScholarSearcher *searcher;
	int action;
	QWebView *w;
};

#endif // SINGLESCHOLARWIDGET_H
