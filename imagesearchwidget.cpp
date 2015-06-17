#include "imagesearchwidget.h"
#include "ui_imagesearchwidget.h"

#include "dataset/googleimagedownloader.h"

#include <QDir>
#include <QDebug>
#include <QWebView>
#include <QMessageBox>
#include <QWebSettings>
#include <QWebInspector>

ImageSearchWidget::ImageSearchWidget(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::ImageSearchWidget)
{
	ui->setupUi(this);
}

ImageSearchWidget::~ImageSearchWidget()
{
	delete ui;
}

void ImageSearchWidget::on_pushSearch_clicked()
{
	GoogleImageDownloader down;
	down.search("cloud");

	QWebSettings::globalSettings()->setAttribute(QWebSettings::DeveloperExtrasEnabled, true);
	QWebInspector ins;
	ins.setPage(&down.page);
	ins.show();

	QWebView *w = NULL;
	if (w == NULL) {
		w = new QWebView;
		//w->setWindowModality(Qt::ApplicationModal);
	}
	w->setPage(&down.page);
	w->show();
	while (w->isVisible())
		QApplication::processEvents();
}
