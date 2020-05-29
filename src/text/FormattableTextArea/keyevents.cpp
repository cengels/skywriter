//////////////////////////////////////////////////////////////////////////////
///                                                                        ///
///         Contains all code responsible for handling key events.         ///
///                                                                        ///
//////////////////////////////////////////////////////////////////////////////

#include "FormattableTextArea.h"

void FormattableTextArea::keyPressEvent(QKeyEvent* event)
{
    const bool shift = event->modifiers().testFlag(Qt::ShiftModifier);
    const bool ctrl = event->modifiers().testFlag(Qt::ControlModifier);
    const QTextCursor::MoveMode moveMode = shift
                                           ? QTextCursor::KeepAnchor
                                           : QTextCursor::MoveAnchor;

    switch (event->key()) {
        case Qt::Key_Right:
            moveCursor(ctrl && !shift ? QTextCursor::NextWord : QTextCursor::Right, moveMode);
            break;
        case Qt::Key_Left:
            moveCursor(ctrl && !shift ? QTextCursor::PreviousWord : QTextCursor::Left, moveMode);
            break;
        case Qt::Key_Up:
            moveCursor(QTextCursor::Up, moveMode);
            break;
        case Qt::Key_Down:
            moveCursor(QTextCursor::Down, moveMode);
            break;
        case Qt::Key_Back:
        case Qt::Key_Backspace:
            if (m_textCursor.hasSelection()) {
                m_textCursor.removeSelectedText();
            } else if (!ctrl) {
                m_textCursor.deletePreviousChar();
            } else {
                m_textCursor.movePosition(QTextCursor::PreviousWord, QTextCursor::KeepAnchor);
                m_textCursor.removeSelectedText();
            }
            update();
            emit caretPositionChanged();
            break;
        case Qt::Key_Delete:
            if (m_textCursor.hasSelection()) {
                m_textCursor.removeSelectedText();
            } else if (!ctrl) {
                m_textCursor.deleteChar();
            } else {
                m_textCursor.movePosition(QTextCursor::NextWord, QTextCursor::KeepAnchor);
                m_textCursor.removeSelectedText();
            }
            update();
            break;
        default:
            const QString text = event->text();

            if (!text.isEmpty()) {
                m_textCursor.insertText(text);
                moveCursor(QTextCursor::NextCharacter, QTextCursor::MoveAnchor, text.length() - 1);
            }
            break;
    }
}
