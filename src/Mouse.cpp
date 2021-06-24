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
    m_lastPressMouseEvent(nullptr)
{
}

bool Mouse::isLongPress(QMouseEvent* mouseUpEvent, QMouseEvent* lastMouseDownEvent)
{
    return lastMouseDownEvent && mouseUpEvent && (qAbs(lastMouseDownEvent->timestamp() - mouseUpEvent->timestamp())
            > static_cast<unsigned long>(QGuiApplication::styleHints()->mousePressAndHoldInterval())
            || lastMouseDownEvent->globalPosition().x() != mouseUpEvent->globalPosition().x()
            || lastMouseDownEvent->globalPosition().y() != mouseUpEvent->globalPosition().y());
}

bool Mouse::isDoubleClick(QMouseEvent* mouseDownEvent, QMouseEvent* lastMouseUpEvent)
{
    return lastMouseUpEvent && mouseDownEvent
            && mouseDownEvent->button() == lastMouseUpEvent->button()
            && (qAbs(lastMouseUpEvent->timestamp() - mouseDownEvent->timestamp()) <= static_cast<unsigned long>(QGuiApplication::styleHints()->mouseDoubleClickInterval())
            && qAbs(lastMouseUpEvent->globalPosition().x() - mouseDownEvent->globalPosition().x()) < QGuiApplication::styleHints()->mouseDoubleClickDistance()
            && qAbs(lastMouseUpEvent->globalPosition().y() - mouseDownEvent->globalPosition().y()) < QGuiApplication::styleHints()->mouseDoubleClickDistance());
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

        this->m_globalPosition = mouseEvent->globalPosition();
        this->m_windowPosition = mouseEvent->scenePosition();
        emit globalPositionChanged();
        emit windowPositionChanged();
        emit move();
    } else if (event->type() == QEvent::MouseButtonPress) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);

        m_buttons = mouseEvent->buttons();
        m_lastPressMouseEvent = mouseEvent->clone();
        emit pressed(static_cast<int>(mouseEvent->button()));
        emit buttonsChanged();
    } else if (event->type() == QEvent::MouseButtonRelease) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);

        m_buttons = mouseEvent->buttons();
        bool longPress = isLongPress(mouseEvent, m_lastPressMouseEvent);
        emit released(static_cast<int>(mouseEvent->button()), longPress);
        emit buttonsChanged();
    }

    return QObject::eventFilter(obj, event);
}

bool Mouse::isInside(QQuickItem* item) const
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
