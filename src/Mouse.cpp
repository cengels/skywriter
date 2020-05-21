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
    m_lastPressTimestamp(0)
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
        m_lastPressTimestamp = mouseEvent->timestamp();
        emit pressed(static_cast<int>(mouseEvent->button()));
        emit buttonsChanged();
    } else if (event->type() == QEvent::MouseButtonRelease) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);

        m_buttons = mouseEvent->buttons();
        bool longPress = mouseEvent->timestamp() - m_lastPressTimestamp > static_cast<unsigned long>(QGuiApplication::styleHints()->mousePressAndHoldInterval());
        emit released(static_cast<int>(mouseEvent->button()), longPress);
        emit buttonsChanged();
    }

    return QObject::eventFilter(obj, event);
}

bool Mouse::isInside(const QQuickItem* item) const {
    return item->contains(item->mapFromGlobal(m_globalPosition));
}

void Mouse::setCursor(Qt::CursorShape cursorShape) {
    QGuiApplication::styleHints()->mousePressAndHoldInterval();
    QGuiApplication::setOverrideCursor(cursorShape);
}

void Mouse::resetCursor() {
    QGuiApplication::restoreOverrideCursor();
}
