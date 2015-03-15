#ifndef BATCHREFERENCEWIDGET_H
#define BATCHREFERENCEWIDGET_H

#include <QWidget>

class Scholar;
class QWebPage;
class QWebView;
class ScholarSearcher;

namespace Ui {
class BatchReferenceWidget;
}

class BatchReferenceWidget : public QWidget
{
	Q_OBJECT

public:
	explicit BatchReferenceWidget(QWidget *parent = 0);
	~BatchReferenceWidget();

private slots:
	void searchNext();
	void searchError(int err);
	void searchProgress(int current, int total);
	void searchFinished();
	void on_pushSearchMissing_clicked();

private:
	void showBrowser(QWebPage *page);
	void handleNext();

	Ui::BatchReferenceWidget *ui;
	QWebView *w;
	QList<Scholar *> scholars;
	ScholarSearcher *searcher;
	int current;
};

#endif // BATCHREFERENCEWIDGET_H
