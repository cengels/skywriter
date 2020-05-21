#include <QCursor>
#include <QEvent>
#include <QMouseEvent>

#include "Mouse.h"

Mouse::Mouse(QObject *parent) : QObject(parent),
    m_globalPosition(),
    m_windowPosition()
{
}

bool Mouse::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::MouseMove) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
        this->m_globalPosition = mouseEvent->globalPos();
        this->m_windowPosition = mouseEvent->windowPos();
        emit globalPositionChanged();
        emit windowPositionChanged();
    }

    return QObject::eventFilter(obj, event);
}
