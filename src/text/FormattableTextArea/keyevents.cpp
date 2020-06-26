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

    switch (event->key()) {
        case Qt::Key_Right:
            moveCursor(ctrl ? QTextCursor::NextWord : QTextCursor::Right, moveMode);
            event->accept();
            break;
        case Qt::Key_Left:
            moveCursor(ctrl ? QTextCursor::PreviousWord : QTextCursor::Left, moveMode);
            event->accept();
            break;
        case Qt::Key_Up:
            moveCursor(QTextCursor::Up, moveMode);
            event->accept();
            break;
        case Qt::Key_Down:
            moveCursor(QTextCursor::Down, moveMode);
            event->accept();
            break;
        case Qt::Key_Back:
        case Qt::Key_Backspace:
            if (!m_textCursor.atStart() && !m_textCursor.hasSelection()) {
                m_textCursor.movePosition(ctrl ? QTextCursor::PreviousWord : QTextCursor::PreviousCharacter, QTextCursor::KeepAnchor);
            }

            remove();

            updateActive();
            event->accept();
            break;
        case Qt::Key_Delete:
            if (!m_textCursor.atEnd() && !m_textCursor.hasSelection()) {
                m_textCursor.movePosition(ctrl ? QTextCursor::NextWord : QTextCursor::NextCharacter, QTextCursor::KeepAnchor);
            }

            remove();

            updateActive();
            event->accept();
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

                return;
            }
            event->accept();
            // intended fallthrough
        default:
            bool hadSelection = m_textCursor.hasSelection();
            const QString text = symbols::sanitize(event->text());

            if (!text.isEmpty()) {
                const int selectionStart = m_textCursor.selectionStart();
                const int selectionEnd = m_textCursor.selectionEnd();
                const QChar& previousCharacter = m_document->characterAt(selectionStart - 1);
                const QChar& nextCharacter = m_document->characterAt(selectionEnd);

                if (hadSelection && m_replacer.isSmartReplacement(text) && symbols::isWordSeparator(previousCharacter) && symbols::isWordSeparator(nextCharacter)) {
                    const QString start = m_replacer.replace(text, previousCharacter);
                    const QString end = m_replacer.replace(text);

                    m_textCursor.beginEditBlock();
                    m_textCursor.setPosition(selectionStart);
                    m_textCursor.insertText(start);
                    m_textCursor.setPosition(selectionEnd + start.size());
                    m_textCursor.insertText(end);
                    m_textCursor.setPosition(selectionStart + start.size());
                    m_textCursor.setPosition(selectionEnd + start.size(), QTextCursor::KeepAnchor);
                    m_textCursor.endEditBlock();
                } else {
                    const QString replacedText = m_replacer.replace(text, previousCharacter);
                    m_textCursor.insertText(replacedText);
                    moveCursor(QTextCursor::NextCharacter, QTextCursor::MoveAnchor, replacedText.length() - 1);
                }

                if (m_textCursor.block().text().isEmpty()) {
                    m_textCursor.joinPreviousEditBlock();
                    m_textCursor.setBlockFormat(ThemeManager::instance()->activeTheme()->blockFormat());
                    m_textCursor.endEditBlock();
                }
                event->accept();
            } else {
                event->ignore();
            }

            if (hadSelection) {
                emit selectedTextChanged();
            }
            break;
    }
}
