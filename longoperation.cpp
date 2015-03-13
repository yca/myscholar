#include "longoperation.h"

LongOperation::LongOperation(QObject *parent) :
	QObject(parent)
{
	stepsCompleted = stepsTotal = 0;
}

void LongOperation::operationStart(int total)
{
	stepsCompleted = 0;
	stepsTotal = total;
}

void LongOperation::operationStep(int completed)
{
	if (completed >= 0)
		stepsCompleted = completed;
	else
		stepsCompleted = stepsTotal;

	if (stepsCompleted >= stepsTotal)
		emit operationFinished();
	else
		emit operationProgress(stepsCompleted, stepsTotal);
}
