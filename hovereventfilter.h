#ifndef HOVEREVENTFILTER_H
#define HOVEREVENTFILTER_H

#include <QObject>
#include <QEvent>

class HoverEventFilter : public QObject
{
	Q_OBJECT
public:
	explicit HoverEventFilter(QObject *parent = 0);

signals:
	void HoverIn(QObject *);
	void HoverOut(QObject *);

	public slots:

protected:
	bool eventFilter(QObject *watched, QEvent *event);
};

#endif // HOVEREVENTFILTER_H
