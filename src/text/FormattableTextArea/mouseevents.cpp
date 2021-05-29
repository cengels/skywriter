//////////////////////////////////////////////////////////////////////////////
///                                                                        ///
///        Contains all code responsible for handling mouse events.        ///
///                                                                        ///
//////////////////////////////////////////////////////////////////////////////

#include <QAbstractTextDocumentLayout>
#include <QGuiApplication>
#include <QStyleHints>

#include "FormattableTextArea.h"
#include "../selection.h"
#include "../../Mouse.h"

void FormattableTextArea::mousePressEvent(QMouseEvent* event)
{
    const bool shift = event->modifiers() & Qt::ShiftModifier;

    switch (event->button()) {
        case Qt::LeftButton:
        {
            const bool hadSelection = m_textCursor.hasSelection();
            const int position = hitTest(event->localPos());
            m_textCursor.setPosition(position, event->button() == Qt::LeftButton && shift ? QTextCursor::KeepAnchor : QTextCursor::MoveAnchor);

            if (event->button() == Qt::LeftButton
                    && Mouse::isDoubleClick(event, m_lastMouseUpEvent, m_lastMouseDownEvent))
            {
                expandSelection();
                break;
            } else {
                m_selectionMode = SelectionMode::NoSelection;
                emit caretPositionChanged();
                updateActive();
            }

            if (hadSelection || m_textCursor.hasSelection()) {
                emit selectedTextChanged();
            }

            break;
        }
        case Qt::RightButton:
        {
            const bool hadSelection = m_textCursor.hasSelection();
            const int position = this->hitTest(event->localPos());

            if (!(hadSelection
                && m_textCursor.selectionStart() <= position
                && position <= m_textCursor.selectionEnd()))
            {
                // Mouse cursor is not on selection.
                m_textCursor.setPosition(position, QTextCursor::MoveAnchor);
                m_selectionMode = SelectionMode::NoSelection;
                updateActive();
                emit caretPositionChanged();

                if (hadSelection) {
                    emit selectedTextChanged();
                }
            }

            break;
        }
        default: event->ignore(); break;
    }

    m_lastMouseDownEvent = event->clone();
}

void FormattableTextArea::expandSelection()
{
    switch (m_selectionMode) {
        case SelectionMode::NoSelection:
            m_selectionMode = SelectionMode::WordSelection;
            this->selectWord();
            break;
        case SelectionMode::WordSelection: {
            m_selectionMode = SelectionMode::BlockSelection;
            this->selectParagraph();
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
                    if (!this->m_lastMouseDownEvent) {
                        return;
                    }

                    const int sourcePosition = this->hitTest(this->m_lastMouseDownEvent->localPos());
                    QTextCursor tempCursor(m_document);
                    tempCursor.setPosition(position);
                    selection::selectWord(tempCursor);

                    m_textCursor.setPosition(sourcePosition);
                    selection::selectWord(m_textCursor);

                    const int start = qMin(tempCursor.selectionStart(), m_textCursor.selectionStart());
                    const int end = qMax(tempCursor.selectionEnd(), m_textCursor.selectionEnd());

                    m_textCursor.setPosition(start);
                    m_textCursor.setPosition(end, QTextCursor::KeepAnchor);
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
                        m_textCursor.setPosition(sourceBlock.position() + sourceBlock.length() - 1, QTextCursor::KeepAnchor);
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
            emit selectedTextChanged();
        }
    }
}

void FormattableTextArea::mouseReleaseEvent(QMouseEvent* event)
{
    m_lastMouseUpEvent = event->clone();

    if (event->button() == Qt::RightButton) {
        emit contextMenuRequested();
    }
}

int FormattableTextArea::hitTest(const QPointF& point) const
{
    QPointF actualPoint(point.x(), point.y() + m_contentY - m_overflowArea);

    return m_document->documentLayout()->hitTest(actualPoint, Qt::FuzzyHit);
}
