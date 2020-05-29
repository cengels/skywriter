//////////////////////////////////////////////////////////////////////////////
///                                                                        ///
///        Contains all code responsible for handling mouse events.        ///
///                                                                        ///
//////////////////////////////////////////////////////////////////////////////

#include <QAbstractTextDocumentLayout>

#include "FormattableTextArea.h"

void FormattableTextArea::mousePressEvent(QMouseEvent* event)
{
    const bool shift = event->modifiers() & Qt::ShiftModifier;

    switch (event->button()) {
        case Qt::LeftButton:
        case Qt::RightButton:
        {
            const int position = hitTest(event->localPos());
            if (position != m_textCursor.position() || m_textCursor.hasSelection()) {
                m_textCursor.setPosition(position, event->button() == Qt::LeftButton && shift ? QTextCursor::KeepAnchor : QTextCursor::MoveAnchor);
                update();
                emit caretPositionChanged();
            }
            break;
        }
        default: break;
    }
}

void FormattableTextArea::mouseMoveEvent(QMouseEvent* event)
{
    // TODO: text dragging

    if (event->buttons() & Qt::LeftButton) {
        const int position = hitTest(event->localPos());
        if (position != m_textCursor.position()) {
            m_textCursor.setPosition(position, QTextCursor::KeepAnchor);
            update();
            emit caretPositionChanged();
        }
    }
}

void FormattableTextArea::mouseReleaseEvent(QMouseEvent* event)
{

}

int FormattableTextArea::hitTest(const QPointF& point) const
{
    QPointF actualPoint(point.x(), point.y() + m_contentY);

    return m_document->documentLayout()->hitTest(actualPoint, Qt::FuzzyHit);
}
