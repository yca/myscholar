#ifndef IMAGESEARCHWIDGET_H
#define IMAGESEARCHWIDGET_H

#include <QWidget>

namespace Ui {
class ImageSearchWidget;
}

class ImageSearchWidget : public QWidget
{
	Q_OBJECT

public:
	explicit ImageSearchWidget(QWidget *parent = 0);
	~ImageSearchWidget();

private slots:
	void on_pushSearch_clicked();

private:
	Ui::ImageSearchWidget *ui;
};

#endif // IMAGESEARCHWIDGET_H
