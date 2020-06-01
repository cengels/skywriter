//////////////////////////////////////////////////////////////////////////////
///                                                                        ///
///         Contains all code responsible for handling key events.         ///
///                                                                        ///
//////////////////////////////////////////////////////////////////////////////

#include "../symbols.h"
#include "FormattableTextArea.h"

void FormattableTextArea::keyPressEvent(QKeyEvent* event)
{
    const bool shift = event->modifiers().testFlag(Qt::ShiftModifier);
    const bool ctrl = event->modifiers().testFlag(Qt::ControlModifier);
    const QTextCursor::MoveMode moveMode = shift
                                           ? QTextCursor::KeepAnchor
                                           : QTextCursor::MoveAnchor;

    if (event->matches(QKeySequence::SelectAll))
    {
        m_textCursor.select(QTextCursor::SelectionType::Document);
    }
    else if (event->matches(QKeySequence::Copy))
    {
        copy();
    }
    else if (event->matches(QKeySequence::Paste))
    {
        paste();
    }
    else if (event->matches(QKeySequence::Undo))
    {
        undo();
    }
    else if (event->matches(QKeySequence::Redo))
    {
        redo();
    }
    else {
        switch (event->key()) {
            case Qt::Key_Right:
                moveCursor(ctrl ? QTextCursor::NextWord : QTextCursor::Right, moveMode);
                break;
            case Qt::Key_Left:
                moveCursor(ctrl ? QTextCursor::PreviousWord : QTextCursor::Left, moveMode);
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
                updateWordCount();
                updateActive();
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
                updateWordCount();
                updateActive();
                break;
            case Qt::Key_W:
                if (ctrl) {
                    if (!m_textCursor.hasSelection()) {
                        this->selectWord();
                    } else {
                        if (symbols::isWordSeparator(m_document->characterAt(m_textCursor.selectionStart() - 1))
                                   && symbols::isWordSeparator(m_document->characterAt(m_textCursor.selectionEnd()))) {
                            this->selectParagraph();
                        } else if (m_document->characterAt(m_textCursor.selectionStart()) != QChar::ParagraphSeparator
                                   && m_document->characterAt(m_textCursor.selectionEnd()) != QChar::ParagraphSeparator) {
                            this->selectWord();
                        }
                    }
                }
                break;
            default:
                const QString text = symbols::sanitize(event->text());

                if (!text.isEmpty()) {
                    const QChar& previousCharacter = m_document->characterAt(m_textCursor.position() - 1);
                    m_textCursor.insertText(text);
                    moveCursor(QTextCursor::NextCharacter, QTextCursor::MoveAnchor, text.length() - 1);

                    if (symbols::isWordSeparator(previousCharacter)) {
                        updateWordCount();
                    }
                }

                if (m_textCursor.block().text().isEmpty()) {
                    m_textCursor.setBlockFormat(ThemeManager::instance()->activeTheme()->blockFormat());
                }
                break;
        }
    }
}
