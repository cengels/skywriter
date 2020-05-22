#include <QCursor>
#include <QEvent>
#include <QMouseEvent>
#include <QQuickItem>
#include <QGuiApplication>
#include <QStyleHints>

#include "Mouse.h"

namespace {
    bool isLongPress(QMouseEvent& event1, QMouseEvent& event2) {
        return event1.timestamp() - event2.timestamp()
                > static_cast<unsigned long>(QGuiApplication::styleHints()->mousePressAndHoldInterval())
                || event1.x() != event2.x()
                || event1.y() != event2.y();
    }
}

Mouse::Mouse(QObject *parent) : QObject(parent),
    m_globalPosition(),
    m_windowPosition(),
    m_lastPressMouseEvent(QMouseEvent(QEvent::None, QPointF(), Qt::MouseButton::NoButton, 0, 0))
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
        emit move();
    } else if (event->type() == QEvent::MouseButtonPress) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);

        m_buttons = mouseEvent->buttons();
        m_lastPressMouseEvent = *mouseEvent;
        emit pressed(static_cast<int>(mouseEvent->button()));
        emit buttonsChanged();
    } else if (event->type() == QEvent::MouseButtonRelease) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);

        m_buttons = mouseEvent->buttons();
        bool longPress = isLongPress(*mouseEvent, m_lastPressMouseEvent);
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
