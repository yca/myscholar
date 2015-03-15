#include "mybookmarks.h"

#include <QStringList>

MyBookmarks MyBookmarks::inst;

void MyBookmarks::add(const QString &bookmark)
{
	QStringList list = inst.s.value("bookmarks").toStringList();
	if (!list.contains(bookmark))
		list << bookmark;
	inst.s.setValue("bookmarks", list);
	inst.s.sync();
}

void MyBookmarks::add(const QString &section, const QString &bookmark)
{
	QStringList list = inst.s.value(section).toStringList();
	if (!list.contains(bookmark))
		list << bookmark;
	inst.s.setValue(section, list);
	inst.s.sync();
}

void MyBookmarks::addQuote(const QString &scholar, const QString &quote)
{
	inst.s.beginGroup("quotes");
	QStringList list = inst.s.value(scholar).toStringList();
	list << quote;
	inst.s.setValue(scholar, list);
	inst.s.endGroup();
}

QStringList MyBookmarks::all()
{
	return inst.s.value("bookmarks").toStringList();
}

MyBookmarks::MyBookmarks()
	: s("bookmarks.ini", QSettings::IniFormat)
{
}
