#include "hovereventfilter.h"

HoverEventFilter::HoverEventFilter(QObject *parent) : QObject(parent)
{

}

bool HoverEventFilter::eventFilter(QObject *watched, QEvent *event)
{

	QEvent::Type t = event->type();

	switch (t) {
	case QEvent::Enter:
		emit HoverIn(watched);
		break;

	case QEvent::Leave:
		emit HoverOut(watched);
		break;
	default:
		return false;
	}

	return true;
}