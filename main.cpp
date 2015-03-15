#include "mainwindow.h"
#include "singlescholarwidget.h"
#include "batchreferencewidget.h"
#include "querywidget.h"

#include <QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	//MainWindow w;
	//SingleScholarWidget w;
	//BatchReferenceWidget w;
	QueryWidget w;
	w.show();

	return a.exec();
}
