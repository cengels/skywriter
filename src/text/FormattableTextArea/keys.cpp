//////////////////////////////////////////////////////////////////////////////
///                                                                        ///
///         Contains all code responsible for handling key events.         ///
///                                                                        ///
//////////////////////////////////////////////////////////////////////////////

#include "FormattableTextArea.h"

void FormattableTextArea::keyPressEvent(QKeyEvent* event)
{
    const QTextCursor::MoveMode moveMode = event->modifiers().testFlag(Qt::ShiftModifier)
                                           ? QTextCursor::KeepAnchor
                                           : QTextCursor::MoveAnchor;

    switch (event->key()) {
        case Qt::Key_Right:
            moveCursor(QTextCursor::Right, moveMode);
            break;
        case Qt::Key_Left:
            moveCursor(QTextCursor::Left, moveMode);
            break;
        case Qt::Key_Up:
            moveCursor(QTextCursor::Up, moveMode);
            break;
        case Qt::Key_Down:
            moveCursor(QTextCursor::Down, moveMode);
            break;
        case Qt::Key_Back:
        case Qt::Key_Backspace:
            m_textCursor.deletePreviousChar();
            update();
            emit caretPositionChanged();
            break;
        case Qt::Key_Delete:
            m_textCursor.deleteChar();
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
