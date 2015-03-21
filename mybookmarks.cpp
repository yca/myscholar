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

void MyBookmarks::addToGroup(const QString &group, const QString &text, const QString &key)
{
	inst.s.beginGroup(group);
	QStringList list = inst.s.value(key).toStringList();
	list << text;
	inst.s.setValue(key, list);
	inst.s.endGroup();
}

bool MyBookmarks::contains(const QString &group, const QString &text, const QString &key)
{
	inst.s.beginGroup(group);
	QStringList list = inst.s.value(key).toStringList();
	inst.s.endGroup();
	return list.contains(text);
}

void MyBookmarks::addQuote(const QString &scholar, const QString &quote)
{
	inst.s.beginGroup("quotes");
	QStringList list = inst.s.value(scholar).toStringList();
	list << quote;
	inst.s.setValue(scholar, list);
	inst.s.endGroup();
}

QHash<QString, QStringList> MyBookmarks::getQuotes()
{
	inst.s.beginGroup("quotes");
	QStringList hashes = inst.s.allKeys();
	QHash<QString, QStringList> quotes;
	foreach (QString h, hashes)
		quotes.insert(h, inst.s.value(h).toStringList());
	inst.s.endGroup();
	return quotes;
}

QStringList MyBookmarks::getGroups()
{
	return inst.s.childGroups();
}

QStringList MyBookmarks::all()
{
	return inst.s.value("bookmarks").toStringList();
}

MyBookmarks::MyBookmarks()
	: s("bookmarks.ini", QSettings::IniFormat)
{
}
