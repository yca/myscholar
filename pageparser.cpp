#include "pageparser.h"
#include "mynetworkaccessmanager.h"

#include <QFile>
#include <QDebug>
#include <QWebFrame>
#include <QWebElement>
#include <QDataStream>
#include <QNetworkReply>
#include <QAuthenticator>

#include <errno.h>

static QStringList dictWords;
static QString titleSearch(const QStringList &flds)
{
	QString title;
	foreach (const QString fld, flds) {
		QStringList words = fld.split(" ", QString::SkipEmptyParts);
		int cnt = 0;
		foreach (const QString word, words) {
			if (word.size() <= 1 || word.contains("."))
				continue;
			if (word.compare("at", Qt::CaseInsensitive) == 0)
				continue;
			if (word.compare("et", Qt::CaseInsensitive) == 0)
				continue;
			if (dictWords.contains(word, Qt::CaseInsensitive))
				cnt++;
		}
		if (cnt >= 3) {
			title = fld;
			break;
		}
	}
	return title.trimmed().toLower();
}

static QString parseIEEEStyle(const QString &desc)
{
	int in = desc.indexOf(" and ");
	in = desc.indexOf(QRegExp("\\w{2,}\\.{1} +"), in);
	if (in < 0)
		in = desc.indexOf(".", 2);
	else
		in = desc.indexOf(".", in + 4);
	QString title = desc.mid(in + 1);
	return title.split(".").first().trimmed().toLower();
}

PageParser::PageParser(QObject *parent) :
	QObject(parent)
{
	page.setNetworkAccessManager(new MyNetworkAccessManager(this));
	connect(&page, SIGNAL(loadFinished(bool)), SLOT(pageLoadFinished(bool)));
	connect(&page, SIGNAL(loadStarted()), SLOT(pageLoadStarted()));
	connect(&page, SIGNAL(frameCreated(QWebFrame*)), SLOT(pageFrameCreated(QWebFrame*)));
	connect(page.networkAccessManager(), SIGNAL(proxyAuthenticationRequired(QNetworkProxy,QAuthenticator*)), SLOT(proxyAuthenticationRequired(QNetworkProxy,QAuthenticator*)));
	parseResult = NULL;
	loading = false;
	parseDone = false;
	qDebug() << "******************" << "parser created";
}

int PageParser::parseUrl(QUrl url)
{
	parseDone = false;
	page.mainFrame()->load(url);
	return 0;
}

void PageParser::pageLoadStarted()
{
	qDebug() << "load started";
	loading = true;
}

void PageParser::pageLoadFinished(bool ok)
{
	if (!ok) {
		qDebug() << "parse error";
		emit parseError(1);
		return;
	}

	if (parseDone)
		return;

	if (dictWords.size() == 0) {
		QFile f("/usr/share/dict/words");
		if (f.open(QIODevice::ReadOnly)) {
			dictWords = QString::fromUtf8(f.readAll()).split("\n");
			f.close();
		}
	}

	qDebug() << "load finished";
	int err = reparse();
	if (err == 0)
		parseDone = true;
	if (!err)
		emit parseFinished();
	else
		emit parseError(err);

	loading = false;

	if (!err)
		parseNext();
}

void PageParser::nmFinished(QNetworkReply *reply)
{
	Q_UNUSED(reply);
	int err = reparse();
	if (!err) {
		disconnect(page.networkAccessManager(), SIGNAL(finished(QNetworkReply*)), this, SLOT(nmFinished(QNetworkReply*)));
		parseNext();
	}
}

void PageParser::pageFrameCreated(QWebFrame *frame)
{
	Q_UNUSED(frame);
	//qDebug() << "new frame added" << frame->frameName();
}

void PageParser::proxyAuthenticationRequired(QNetworkProxy, QAuthenticator *a)
{
	a->setUser("hiler");
	a->setPassword("hil68er");
	qDebug() << "proxy authentication required";
}

int GoogleScholarParser::reparse()
{
	if (parseResult)
		delete parseResult;

	qDebug() << "done" << page.mainFrame()->url();

	ScholarResultList *r = new ScholarResultList;
	parseResult = r;

	QWebElementCollection coll = page.mainFrame()->findAllElements("a");
	foreach (QWebElement el, coll) {
		QString inner = el.toInnerXml();
		/*
		 * links shown in page have data-clk attr, so do pdfs on the right,
		 * but pdf links have span elements so we check it to filter them out
		 */
		if (el.hasAttribute("data-clk") && el.findFirst("span").isNull()) {
			/* this is the link to external page and starts a new scholar item */
			Scholar *s = new Scholar;
			s->setTitle(inner.trimmed().toLower());
			s->externalLink = el.attribute("href");
			if (inner == page.mainFrame()->url().queryItemValue("q"))
				s->queryMatch = true;
			r->list2 << s;
		} else if (inner.contains("Cited by") && r->list2.size()) {
			r->list2.last()->citationsLink = el.attribute("href");
			r->list2.last()->citedBy = inner.remove("Cited by").trimmed().toInt();
		}
	}

	QWebElement el = page.mainFrame()->findFirstElement(".gs_ico_nav_next");
	if (!el.isNull())
		r->nextLink = el.parent().attribute("href");
	if (r->nextLink.isEmpty()) {
		/* check if this is a robot detection */
		const QString pstr = page.mainFrame()->toPlainText();
		if (pstr.contains("Please type the words below so that we know you're not a robot.")
			|| pstr.contains("To continue, please type the characters below:"))
			return 0x1525;
	}

	return 0;
}

int IEEExploreParser::reparse()
{
	QString urlstr = page.mainFrame()->url().toString();
	qDebug() << "done" << urlstr;

	QWebElement el = page.mainFrame()->findFirstElement("ol[class=\"docs\"]");

	//if (!urlstr.contains("abstractReferences")) {
	/*if (el.isNull()) {
		el = page.mainFrame()->findFirstElement("a[id=\"abstract-references-tab\"]");
		if (!el.isNull())
			el = el.firstChild();
		qDebug() << "click" << el.isNull();
		if (!el.isNull()) {
			el.evaluateJavaScript("var __ev = document.createEvent('MouseEvents'); __ev.initEvent('click', true, true); this.dispatchEvent(__ev);");
			return -EAGAIN;
		}
		return -ENOENT;
	}*/

	if (parseResult)
		delete parseResult;

	ScholarResultList *r = new ScholarResultList;
	parseResult = r;
	QWebElementCollection coll = el.findAll("li");
	foreach (QWebElement ref, coll) {
		QString desc = ref.toPlainText().split("\n").first();
		QString title = parseIEEEStyle(desc);
		//QString title = titleSearch(desc.split("."));
		if (title.isEmpty())
			continue;
		Scholar *s = new Scholar;
		s->setTitle(title.trimmed().toLower());
		r->list2 << s;
	}

	return 0;
}

int SpringerParser::reparse()
{
	QString urlstr = page.mainFrame()->url().toString();
	qDebug() << "done" << urlstr;

	if (parseResult)
		delete parseResult;

	QWebElement el = page.mainFrame()->findFirstElement("div[class=\"formatted\"]");
	ScholarResultList *r = new ScholarResultList;
	parseResult = r;
	QWebElementCollection coll = el.findAll("li");
	foreach (QWebElement ref, coll) {
		QString desc = ref.toPlainText().trimmed().split("\n").first();
		int in = desc.indexOf(":");
		int in2 = desc.indexOf(".", in + 1);
		QString title = desc.mid(in + 1, in2 - in - 1).trimmed();
		if (title.isEmpty())
			continue;
		Scholar *s = new Scholar;
		s->setTitle(title.toLower());
		r->list2 << s;
	}

	return 0;
}

int AcmParser::reparse()
{
	/* we support ACM's flat layout */
	QString urlstr = page.mainFrame()->url().toString();
	qDebug() << "done" << urlstr;

	if (parseResult)
		delete parseResult;

	ScholarResultList *r = new ScholarResultList;
	parseResult = r;
	QWebElementCollection coll = page.mainFrame()->findAllElements("span[class=\"heading\"]");
	foreach (QWebElement span, coll) {
		QString section = span.toPlainText().trimmed();
		/*
		 *	"AUTHORS"
			"REFERENCES"
			"CITED BY"
			"INDEX TERMS"
			"The ACM Computing Classification System (CCS rev.2012)"
			"The ACM Computing Classification System"
			"Primary Classification:"
			"Additional Classification:"
			"PUBLICATION"
			"REVIEWS"
			"COMMENTS"
		*/
		if (section != "REFERENCES")
			continue;
		QWebElement el = span.parent().parent().nextSibling().findFirst("table");
		QWebElementCollection coll2 = el.findAll("tr");
		foreach (QWebElement tr, coll2) {
			QString text = tr.lastChild().firstChild().toPlainText();
			QString place = tr.lastChild().findFirst("i").toPlainText();
			QString title;
			if (place.isEmpty()) {
				QStringList flds = text.split(",");
				title = titleSearch(flds);
			} else {
				//title = parseIEEEStyle(text.split(place).first());
				title = titleSearch(text.split(place).first().split("."));
			}
			if (!title.isEmpty()) {
				Scholar *s = new Scholar;
				s->setTitle(title.toLower());
				r->list2 << s;
			}
		}
		break;
	}

	return 0;
}


SciencedirectParser::SciencedirectParser(QObject *parent)
	: PageParser(parent)
{
	page.networkAccessManager()->setProxy(QNetworkProxy(QNetworkProxy::HttpProxy, "proxy.baskent.edu.tr", 8080, "hiler", "hil68er"));
}

int SciencedirectParser::reparse()
{
	QString urlstr = page.mainFrame()->url().toString();
	qDebug() << "done" << urlstr;

	if (parseResult)
		delete parseResult;

	ScholarResultList *r = new ScholarResultList;
	parseResult = r;

	QWebElement el = page.mainFrame()->findFirstElement("ol[class=\"references\"]");
	QWebElementCollection coll = el.findAll("li[class=\"author\"]");
	foreach (QWebElement li, coll) {
		QString desc = li.toPlainText().trimmed();
		desc.replace("\n", " ");
		desc.replace("\r", " ");
		QString title = titleSearch(desc.split(","));
		if (title.contains("."))
			title = title.split(".").last().trimmed();
		if (title.isEmpty())
			continue;
		Scholar *s = new Scholar;
		s->setTitle(title);
		r->list2 << s;
	}
	coll = el.findAll("li[class=\"title\"]");
	foreach (QWebElement li, coll) {
		QString title = li.toPlainText().trimmed().toLower();
		if (title.isEmpty())
			continue;
		Scholar *s = new Scholar;
		s->setTitle(title);
		r->list2 << s;
	}

	return 0;
}
