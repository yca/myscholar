#include "mybrowser.h"
#include "ui_mybrowser.h"

#include <QDir>
#include <QDebug>
#include <QWebView>
#include <QCloseEvent>
#include <QMessageBox>
#include <QInputDialog>
#include <QWebSettings>
#include <QWebInspector>
#include <QNetworkProxy>

MyBrowser::MyBrowser(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::MyBrowser)
{
	ui->setupUi(this);
	ui->frameSearch->hide();
	ui->tabWidget->setTabsClosable(true);
}

void MyBrowser::addTab(const QUrl &url)
{
	if (!urls.contains(url)) {
		QWebView *view = new QWebView(ui->tabWidget);
		MyWebPage *page = new MyWebPage(view);
		connect(page, SIGNAL(newTabRequest(QUrl)), SLOT(addTab(QUrl)));
		view->setPage(page);
		connect(view, SIGNAL(linkClicked(QUrl)), SLOT(addTab(QUrl)));
		connect(page, SIGNAL(downloadRequested(const QNetworkRequest &)), SLOT(downloadRequested(const QNetworkRequest &)));
		connect(page, SIGNAL(unsupportedContent(QNetworkReply*)), SLOT(unsupportedContent(QNetworkReply *)));
		connect(page, SIGNAL(frameCreated(QWebFrame*)), SLOT(frameCreated(QWebFrame *)));
		view->installEventFilter(this);
		view->page()->networkAccessManager()->setProxy(QNetworkProxy(QNetworkProxy::HttpProxy, "proxy.baskent.edu.tr", 8080, "hiler", "hil68er"));
		view->load(url);
		ui->listTabs->addItem(QString("%1").arg(url.toString()));
		ui->tabWidget->addTab(view, QString("%1").arg(urls.size() + 1));
		urls.insert(url, ui->tabWidget->count() - 1);
	}
	ui->tabWidget->setCurrentIndex(urls[url]);
	ui->lineAddressBar->setText(url.toString());
}

MyBrowser::~MyBrowser()
{
	delete ui;
}

bool MyBrowser::eventFilter(QObject *obj, QEvent *ev)
{
	if (ev->type() != QEvent::KeyPress)
		return false;
	QKeyEvent *kev = (QKeyEvent *)ev;
	if (kev->modifiers() != Qt::ControlModifier)
		return false;
	if (kev->key() != Qt::Key_F)
		return false;
	QWebView *view = qobject_cast<QWebView *>(obj);
	if (!view)
		return false;
	initSearch(view);
	return true;
}

void MyBrowser::closeEvent(QCloseEvent *ev)
{
	QWidget::closeEvent(ev);
}

void MyBrowser::frameCreated(QWebFrame *)
{
	qDebug() << "frame added";
}

void MyBrowser::unsupportedContent(QNetworkReply *reply)
{
	qDebug() << reply->url();
}

void MyBrowser::downloadRequested(const QNetworkRequest &req)
{
	qDebug() << req.url();
}

void MyBrowser::on_listTabs_itemDoubleClicked(QListWidgetItem *item)
{
	ui->tabWidget->setCurrentIndex(urls[QUrl(item->text())]);
	ui->lineAddressBar->setText(item->text());
}

void MyBrowser::on_tabWidget_currentChanged(int index)
{
	if (index == 0) {
		/* add page action */
		QString text = QInputDialog::getText(this, trUtf8("URL Entry"), trUtf8("Please enter page address:"));
		if (!text.isEmpty())
			addTab(QUrl(text));
	} else
		ui->lineAddressBar->setText(ui->listTabs->item(index - 1)->text());
}

void MyBrowser::initSearch(QWebView *view)
{
	ui->frameSearch->show();
	ui->lineSearch->setText(view->selectedText());
	ui->lineSearch->setSelection(0, ui->lineSearch->text().size());
	ui->lineSearch->setFocus();
	on_lineSearch_textEdited(ui->lineSearch->text());
}

void MyBrowser::on_toolSearchClose_clicked()
{
	ui->frameSearch->hide();
}

void MyBrowser::on_lineSearch_textEdited(const QString &arg1)
{
	QWebView *view = (QWebView *)ui->tabWidget->currentWidget();
	view->findText("", QWebPage::HighlightAllOccurrences); /* clear old selection */
	if (!view->findText(arg1, QWebPage::HighlightAllOccurrences))
		ui->lineSearch->setStyleSheet("color: red");
	else
		ui->lineSearch->setStyleSheet("");
}

void MyBrowser::on_pushSearchNext_clicked()
{
	QWebView *view = (QWebView *)ui->tabWidget->currentWidget();
	view->findText(ui->lineSearch->text(), QWebPage::FindWrapsAroundDocument);
}

void MyBrowser::on_pushSearchPrev_clicked()
{
	QWebView *view = (QWebView *)ui->tabWidget->currentWidget();
	view->findText(ui->lineSearch->text(), QWebPage::FindWrapsAroundDocument | QWebPage::FindBackward);
}

void MyBrowser::on_lineSearch_returnPressed()
{
	on_pushSearchNext_clicked();
}

void MyBrowser::on_tabWidget_tabCloseRequested(int index)
{
	if (!index)
		return;
	if (ui->tabWidget->count() < 3)
		return;
	QWidget *w = ui->tabWidget->widget(index);
	ui->tabWidget->removeTab(index);
	QListWidgetItem *item = ui->listTabs->takeItem(index - 1);
	urls.remove(item->text());
	ui->tabWidget->setCurrentIndex(1);
	w->deleteLater();
	delete item;
}


MyWebPage::MyWebPage(QObject *parent)
	: QWebPage(parent)
{
	connect(this, SIGNAL(linkHovered(QString,QString,QString)), SLOT(linkHoveredS(QString,QString,QString)));
}

void MyWebPage::triggerAction(QWebPage::WebAction act, bool checked)
{
	if (act == QWebPage::OpenLinkInNewWindow)
		emit newTabRequest(QUrl(lastLink));
	else
		QWebPage::triggerAction(act, checked);
}

void MyWebPage::linkHoveredS(const QString &link, const QString &title, const QString &textContent)
{
	Q_UNUSED(title);
	Q_UNUSED(textContent);
	if (!link.isEmpty())
		lastLink = link;
}
