//////////////////////////////////////////////////////////////////////////////
///                                                                        ///
///        Contains all code responsible for handling mouse events.        ///
///                                                                        ///
//////////////////////////////////////////////////////////////////////////////

#include <QAbstractTextDocumentLayout>
#include <QGuiApplication>
#include <QStyleHints>

#include "FormattableTextArea.h"
#include "../../Mouse.h"

void FormattableTextArea::mousePressEvent(QMouseEvent* event)
{
    const bool shift = event->modifiers() & Qt::ShiftModifier;

    switch (event->button()) {
        case Qt::LeftButton:
        case Qt::RightButton:
        {
            const int position = hitTest(event->localPos());
            const int previousPosition = m_textCursor.position();
            m_textCursor.setPosition(position, event->button() == Qt::LeftButton && shift ? QTextCursor::KeepAnchor : QTextCursor::MoveAnchor);

            if (event->button() == Qt::LeftButton
                    && Mouse::isDoubleClick(event, &m_lastMouseUpEvent, &m_lastMouseDownEvent))
            {
                expandSelection();
            } else {
                m_selectionMode = SelectionMode::NoSelection;
            }

            if (previousPosition != m_textCursor.position() || m_textCursor.hasSelection()) {
                updateActive();
                emit caretPositionChanged();
            }
            break;
        }
        default: break;
    }

    m_lastMouseDownEvent = QMouseEvent(*event);
}

void FormattableTextArea::expandSelection()
{
    switch (m_selectionMode) {
        case SelectionMode::NoSelection:
            m_selectionMode = SelectionMode::WordSelection;
            m_textCursor.select(QTextCursor::SelectionType::WordUnderCursor);
            break;
        case SelectionMode::WordSelection: {
            m_selectionMode = SelectionMode::BlockSelection;
            const QTextBlock& block = m_textCursor.block();
            m_textCursor.setPosition(block.position());
            m_textCursor.setPosition(block.position() + block.length() - 1, QTextCursor::KeepAnchor);
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
            switch (m_selectionMode) {
                case SelectionMode::NoSelection:
                    m_textCursor.setPosition(position, QTextCursor::KeepAnchor);
                    break;
                case SelectionMode::WordSelection: {
                        QTextCursor tempCursor(m_document);
                        tempCursor.setPosition(position);
                        tempCursor.select(QTextCursor::WordUnderCursor);

                        if (tempCursor.anchor() == m_textCursor.anchor()) {
                            m_textCursor.setPosition(tempCursor.selectionStart());
                            m_textCursor.setPosition(tempCursor.selectionEnd(), QTextCursor::KeepAnchor);
                        } else if (tempCursor.anchor() < m_textCursor.anchor()) {
                            m_textCursor.setPosition(m_textCursor.anchor());
                            m_textCursor.movePosition(QTextCursor::EndOfWord);
                            m_textCursor.setPosition(tempCursor.selectionStart(), QTextCursor::KeepAnchor);
                        } else {
                            m_textCursor.setPosition(m_textCursor.anchor());
                            m_textCursor.movePosition(QTextCursor::StartOfWord);
                            m_textCursor.setPosition(tempCursor.selectionEnd(), QTextCursor::KeepAnchor);
                        }
                    }
                    break;
                case SelectionMode::BlockSelection: {
                    const QTextBlock& sourceBlock = m_document->findBlock(m_textCursor.anchor());
                    const QTextBlock& targetBlock = m_document->findBlock(position);
                    // The -1 causes the final line break to not be captured
                    // which is necessary to ensure the anchor stays in
                    // the right block.

                    if (targetBlock == sourceBlock) {
                        m_textCursor.setPosition(sourceBlock.position());
                        m_textCursor.setPosition(sourceBlock.position() + sourceBlock.length()- 1, QTextCursor::KeepAnchor);
                    } else if (targetBlock < sourceBlock) {
                        m_textCursor.setPosition(sourceBlock.position() + sourceBlock.length() - 1);
                        m_textCursor.setPosition(targetBlock.position(), QTextCursor::KeepAnchor);
                    } else {
                        m_textCursor.setPosition(sourceBlock.position());
                        m_textCursor.setPosition(targetBlock.position() + targetBlock.length() - 1, QTextCursor::KeepAnchor);
                    }
                }
            }

            update();
            emit caretPositionChanged();
        }
    }
}

void FormattableTextArea::mouseReleaseEvent(QMouseEvent* event)
{
    m_lastMouseUpEvent = QMouseEvent(*event);
}

int FormattableTextArea::hitTest(const QPointF& point) const
{
    QPointF actualPoint(point.x(), point.y() + m_contentY - m_overflowArea);

    return m_document->documentLayout()->hitTest(actualPoint, Qt::FuzzyHit);
}
