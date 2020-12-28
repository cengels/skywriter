#ifndef GLOBALMOUSEAREA_H
#define GLOBALMOUSEAREA_H

#include <QObject>
#include <QCursor>
#include <QPointF>
#include <QMouseEvent>
#include <QQuickItem>

class Mouse : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QPointF globalPosition MEMBER m_globalPosition NOTIFY globalPositionChanged)
    Q_PROPERTY(QPointF windowPosition MEMBER m_windowPosition NOTIFY windowPositionChanged)
    Q_PROPERTY(Qt::MouseButtons buttons MEMBER m_buttons NOTIFY buttonsChanged)

    public:
        explicit Mouse(QObject *parent = nullptr);

        static bool isLongPress(QMouseEvent* mouseUpEvent, QMouseEvent* lastMouseDownEvent);
        static bool isDoubleClick(QMouseEvent* mouseDownEvent, QMouseEvent* lastMouseUpEvent);
        static bool isDoubleClick(QMouseEvent* mouseDownEvent, QMouseEvent* lastMouseUpEvent, QMouseEvent* lastMouseDownEvent);

    public Q_SLOTS:
        bool isInside(QQuickItem* item) const;
        void setCursor(Qt::CursorShape cursorShape);
        void resetCursor();

    Q_SIGNALS:
        void globalPositionChanged();
        void windowPositionChanged();
        void move();
        void pressed(int button);
        void released(int button, bool longPress);
        void buttonsChanged();

    protected:
        bool eventFilter(QObject *obj, QEvent *event) override;

    private:
        QPointF m_globalPosition;
        QPointF m_windowPosition;
        Qt::MouseButtons m_buttons;
        QMouseEvent* m_lastPressMouseEvent;

};

#endif // GLOBALMOUSEAREA_H
