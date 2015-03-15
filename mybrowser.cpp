#include "mybrowser.h"
#include "ui_mybrowser.h"

#include <QDir>
#include <QDebug>
#include <QWebView>
#include <QCloseEvent>
#include <QMessageBox>
#include <QWebSettings>
#include <QWebInspector>
#include <QNetworkProxy>

MyBrowser::MyBrowser(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::MyBrowser)
{
	ui->setupUi(this);
}

void MyBrowser::addTab(const QUrl &url)
{
	if (!urls.contains(url)) {
		QWebView *view = new QWebView(ui->tabWidget);
		view->page()->networkAccessManager()->setProxy(QNetworkProxy(QNetworkProxy::HttpProxy, "proxy.baskent.edu.tr", 8080, "hiler", "hil68er"));
		view->load(url);
		ui->tabWidget->addTab(view, QString("%1").arg(urls.size() + 1));
		urls.insert(url, ui->tabWidget->count() - 1);
		ui->listTabs->addItem(QString("%1").arg(url.toString()));
	}
	ui->tabWidget->setCurrentIndex(urls[url]);
	ui->lineAddressBar->setText(url.toString());
}

MyBrowser::~MyBrowser()
{
	delete ui;
}

void MyBrowser::closeEvent(QCloseEvent *ev)
{
	ev->ignore();
	hide();
}

void MyBrowser::on_listTabs_itemDoubleClicked(QListWidgetItem *item)
{
	ui->tabWidget->setCurrentIndex(urls[QUrl(item->text())]);
	ui->lineAddressBar->setText(item->text());
}
