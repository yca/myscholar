#include "scholar.h"

#include <errno.h>

#include <QDebug>
#include <QFileInfo>

Scholar::Scholar()
{
	citedBy = -1;
	queryMatch = true; //to prevent google robot breaking
}

void Scholar::setTitle(QString title)
{
	this->title = title;
	uniqueHash = QString::fromLatin1(QCryptographicHash::hash(title.toUtf8(), QCryptographicHash::Md5).toHex());
}

void Scholar::write(QIODevice *dev) const
{
	QStringList lines;
	lines << QString("title=%1").arg(title);
	lines << QString("ccount=%1").arg(citedBy);
	lines << QString("externalLink=%1").arg(externalLink);
	lines << QString("citationsLink=%1").arg(citationsLink);
	for (int i = 0; i < citingTitles.size(); i++)
		lines << QString("ct=%1").arg(citingTitles[i]);
	for (int i = 0; i < references.size(); i++)
		lines << QString("ref=%1").arg(references[i]);
	dev->write(lines.join("\n").toUtf8());
}

void Scholar::read(QIODevice *dev)
{
	citingTitles.clear();
	references.clear();
	QStringList lines = QString::fromUtf8(dev->readAll()).split("\n");
	for (int i = 0; i < lines.size(); i++) {
		if (!lines[i].contains("="))
			continue;
		QStringList val = lines[i].split("=");
		QString key = val[0];
		val.removeFirst();
		QString rest = val.join("=");
		if (key == "title")
			title = rest;
		else if (key == "ccount")
			citedBy = val[0].trimmed().toInt();
		else if (key == "externalLink")
			externalLink = rest;
		else if (key == "citationsLink")
			citationsLink = rest;
		else if (key == "ct")
			citingTitles << rest;
		else if (key == "ref")
			references << rest;
	}
}

int Scholar::readScholar(const QString &path)
{
	QFileInfo fi(QString("%2/%1.sxt").arg(uniqueHash).arg(path));
	QFile f(fi.absoluteFilePath());
	if (f.open(QIODevice::ReadOnly)) {
		read(&f);
		f.close();
		return 0;
	}

	return -ENOENT;
}

int Scholar::saveScholar(const QString &path)
{
	QFile f(QString("%2/%1.sxt").arg(uniqueHash).arg(path));
	if (f.open(QIODevice::WriteOnly)) {
		write(&f);
		f.close();
		return 0;
	}
	return -EPERM;
}

QList<Scholar *> Scholar::readScholars(const QStringList &titles, const QString &path)
{
	QList<Scholar *> list;
	for (int i = 0; i < titles.size(); i++) {
		//qDebug() << "reading" << i << s.citedBy << s.citingTitles.size();
		QString t = titles[i];
		Scholar *c = new Scholar;
		c->setTitle(t);
		c->readScholar(path);
		list << c;
	}
	return list;
}
