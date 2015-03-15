#ifndef MYBOOKMARKS_H
#define MYBOOKMARKS_H

#include <QString>
#include <QSettings>

class MyBookmarks
{
public:
	static void add(const QString &bookmark);
	static void add(const QString &section, const QString &bookmark);
	static void addQuote(const QString &scholar, const QString &quote);
	static QStringList all();
protected:
	MyBookmarks();

	static MyBookmarks inst;
	QSettings s;
};

#endif // MYBOOKMARKS_H
