#ifndef LONGOPERATION_H
#define LONGOPERATION_H

#include <QObject>

class LongOperation : public QObject
{
	Q_OBJECT
public:
	explicit LongOperation(QObject *parent = 0);

	int stepsTotal;
	int stepsCompleted;

signals:
	void operationFinished();
	void operationProgress(int completed, int total);
public slots:
protected:
	void operationStep(int completed);
	void operationStart(int total);
};

#endif // LONGOPERATION_H
