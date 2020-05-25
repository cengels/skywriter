/////////////////////////////////////////////////////////////////////////////
///                                                                       ///
///   Contains all code responsible for interaction with the underlying   ///
///   QTextEdit component.                                                ///
///                                                                       ///
/////////////////////////////////////////////////////////////////////////////

#include "FormattableTextArea.h"
#include "../../theming/ThemeManager.h"

void FormattableTextArea::paint(QPainter *painter)
{
    if (m_document) {
//        const QTime start = QTime::currentTime();
        m_textEdit->render(painter);
//        qDebug() << "Finished painting the text area in" << start.msecsTo(QTime::currentTime()) << "ms";
    }
}

bool FormattableTextArea::eventFilter(QObject* object, QEvent* event)
{
    switch (event->type())
    {
        case QEvent::Paint:
        case QEvent::UpdateRequest:
            this->update();
        break;
        default:
            break;
    }

    return QQuickPaintedItem::eventFilter(object, event);
}

bool FormattableTextArea::event(QEvent* event)
{
    if (event->type() == QEvent::MouseButtonPress) {
        forceActiveFocus();
    } else if (event->type() == QEvent::Wheel) {
        QWheelEvent* ev = static_cast<QWheelEvent*>(event);

        m_textEdit->scroll(0, ev->pixelDelta().y());
    }

    QCoreApplication::sendEvent(m_textEdit, event);

    return QQuickPaintedItem::event(event);
}

void FormattableTextArea::updateStyling()
{
    m_textEdit->setFont(ThemeManager::instance()->activeTheme()->font());
    m_textEdit->setTextColor(ThemeManager::instance()->activeTheme()->fontColor());
    update();
}
