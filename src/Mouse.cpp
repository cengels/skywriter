#include <QCursor>
#include <QEvent>
#include <QMouseEvent>
#include <QQuickItem>
#include <QGuiApplication>
#include <QStyleHints>

#include "Mouse.h"

Mouse::Mouse(QObject *parent) : QObject(parent),
    m_globalPosition(),
    m_windowPosition(),
    m_lastPressMouseEvent(QMouseEvent(QMouseEvent::None, QPointF(), Qt::NoButton, 0, 0))
{
}

bool Mouse::isLongPress(QMouseEvent* mouseUpEvent, QMouseEvent* lastMouseDownEvent)
{
    return lastMouseDownEvent && mouseUpEvent && (qAbs(lastMouseDownEvent->timestamp() - mouseUpEvent->timestamp())
            > static_cast<unsigned long>(QGuiApplication::styleHints()->mousePressAndHoldInterval())
            || lastMouseDownEvent->x() != mouseUpEvent->x()
            || lastMouseDownEvent->y() != mouseUpEvent->y());
}

bool Mouse::isDoubleClick(QMouseEvent* mouseDownEvent, QMouseEvent* lastMouseUpEvent)
{
    return lastMouseUpEvent && mouseDownEvent
            && mouseDownEvent->button() == lastMouseUpEvent->button()
            && (qAbs(lastMouseUpEvent->timestamp() - mouseDownEvent->timestamp()) <= static_cast<unsigned long>(QGuiApplication::styleHints()->mouseDoubleClickInterval())
            && qAbs(lastMouseUpEvent->x() - mouseDownEvent->x()) < QGuiApplication::styleHints()->mouseDoubleClickDistance()
            && qAbs(lastMouseUpEvent->y() - mouseDownEvent->y()) < QGuiApplication::styleHints()->mouseDoubleClickDistance());
}

bool Mouse::isDoubleClick(QMouseEvent* mouseDownEvent, QMouseEvent* lastMouseUpEvent, QMouseEvent* lastMouseDownEvent)
{
    return isDoubleClick(lastMouseUpEvent, mouseDownEvent)
        && isDoubleClick(lastMouseDownEvent, lastMouseUpEvent);
}

bool Mouse::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::MouseMove) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
        this->m_globalPosition = mouseEvent->globalPos();
        this->m_windowPosition = mouseEvent->windowPos();
        emit globalPositionChanged();
        emit windowPositionChanged();
        emit move();
    } else if (event->type() == QEvent::MouseButtonPress) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);

        m_buttons = mouseEvent->buttons();
        m_lastPressMouseEvent = QMouseEvent(*mouseEvent);
        emit pressed(static_cast<int>(mouseEvent->button()));
        emit buttonsChanged();
    } else if (event->type() == QEvent::MouseButtonRelease) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);

        m_buttons = mouseEvent->buttons();
        bool longPress = isLongPress(mouseEvent, &m_lastPressMouseEvent);
        emit released(static_cast<int>(mouseEvent->button()), longPress);
        emit buttonsChanged();
    }

    return QObject::eventFilter(obj, event);
}

bool Mouse::isInside(const QQuickItem* item) const
{
    return item->contains(item->mapFromGlobal(m_globalPosition));
}

void Mouse::setCursor(Qt::CursorShape cursorShape)
{
    QGuiApplication::setOverrideCursor(cursorShape);
}

void Mouse::resetCursor()
{
    QGuiApplication::restoreOverrideCursor();
}
