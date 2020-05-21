#ifndef GLOBALMOUSEAREA_H
#define GLOBALMOUSEAREA_H

#include <QObject>
#include <QCursor>
#include <QPointF>

class Mouse : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QPointF globalPosition MEMBER m_globalPosition NOTIFY globalPositionChanged)
    Q_PROPERTY(QPointF windowPosition MEMBER m_windowPosition NOTIFY windowPositionChanged)

    public:
        explicit Mouse(QObject *parent = nullptr);

    Q_SIGNALS:
        void globalPositionChanged();
        void windowPositionChanged();

    protected:
        bool eventFilter(QObject *obj, QEvent *event) override;

    private:
        QPointF m_globalPosition;
        QPointF m_windowPosition;

};

#endif // GLOBALMOUSEAREA_H
