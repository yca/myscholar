#include "bibliowidget.h"
#include "ui_bibliowidget.h"

#include <QFile>
#include <QDebug>
#include <QFileDialog>

static QString importText(const QString &filename)
{
	QFile file(filename);
	if (!file.open(QIODevice::ReadOnly))
		return QString();
	QString text = QString::fromUtf8(file.readAll());
	file.close();
	return text;
}

static QString getBibId(const QString &bib)
{
	QStringList lines = bib.split("\n");
	return lines.first().split("{").last().remove(",");
}

static QStringList parseBibFile(const QString &filename)
{
	QString text = importText(filename);
	QStringList lines = text.split("\n");
	QString bib;
	QStringList bibs;
	foreach (const QString &line, lines) {
		if (line.trimmed().isEmpty()) {
			bibs << bib;
			bib.clear();
		} else
			bib.append(line + "\n");
	}
	if (!bib.isEmpty())
		bibs << bib;
	return bibs;
}

static void exportBibFile(const QString &filename, const QHash<QString, QString> &bibs)
{
	QFile file(filename);
	if (!file.open(QIODevice::WriteOnly))
		return;
	QHashIterator<QString, QString> hi(bibs);
	while (hi.hasNext()) {
		hi.next();
		file.write(hi.value().toUtf8());
		file.write("\n");
	}
	file.close();
}

BiblioWidget::BiblioWidget(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::BiblioWidget)
{
	ui->setupUi(this);
}

BiblioWidget::~BiblioWidget()
{
	delete ui;
}

void BiblioWidget::on_pushAdd_clicked()
{
	QStringList bibs;
	QStringList filenames = QFileDialog::getOpenFileNames(this, trUtf8("Please select your bibliography files"));
	foreach (const QString &filename, filenames) {
		bibs += parseBibFile(filename);
	}

	foreach (const QString bib, bibs)
		allbibs.insert(getBibId(bib), bib);

	ui->listBibs->clear();
	ui->listBibs->addItems(allbibs.keys());
	ui->labelCount->setText(trUtf8("%1 papers").arg(allbibs.size()));
}

void BiblioWidget::on_pushExport_clicked()
{
	QString filename = QFileDialog::getSaveFileName(this, trUtf8("Please select your export file name"));
	if (!filename.isEmpty())
		exportBibFile(filename, allbibs);
}
