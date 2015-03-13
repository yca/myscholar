#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "pageparser.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();

private slots:
	void singleSearchFinished();
	void citationsNextFinished();
	void on_pushSingleSearch_clicked();

	void on_pushCitations_clicked();

	void on_pushShowPage_clicked();

	void on_pushSaveCurrent_clicked();

	void on_pushScholarInfo_clicked();

	void on_comboSource_activated(int index);

	void on_pushReferences_clicked();

private:
	void atext(const QString &s);
	Ui::MainWindow *ui;
	GoogleScholarParser *curr;
	Scholar currScholar;
	QList<Scholar> scholarList;
};

#endif // MAINWINDOW_H
