#include "mainwindow.h"
#include "singlescholarwidget.h"

#include <QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	//MainWindow w;
	SingleScholarWidget w;
	w.show();

	return a.exec();
}
