#ifndef BIBLIOWIDGET_H
#define BIBLIOWIDGET_H

#include <QHash>
#include <QWidget>

namespace Ui {
class BiblioWidget;
}

class BiblioWidget : public QWidget
{
	Q_OBJECT

public:
	explicit BiblioWidget(QWidget *parent = 0);
	~BiblioWidget();

private slots:
	void on_pushAdd_clicked();

	void on_pushExport_clicked();

private:
	Ui::BiblioWidget *ui;
	QHash<QString, QString> allbibs;
};

#endif // BIBLIOWIDGET_H
