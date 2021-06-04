//////////////////////////////////////////////////////////////////////////////
///                                                                        ///
///         Contains all code responsible for handling key events.         ///
///                                                                        ///
//////////////////////////////////////////////////////////////////////////////

#include "../symbols.h"
#include "../selection.h"
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
        case Qt::Key_Home:
            moveCursor(ctrl ? QTextCursor::Start : QTextCursor::StartOfBlock, moveMode);
            event->accept();
            break;
        case Qt::Key_End:
            moveCursor(ctrl ? QTextCursor::End : QTextCursor::EndOfBlock, moveMode);
            event->accept();
            break;
        case Qt::Key_Back:
        case Qt::Key_Backspace: {
            bool moveBack = false;
            if (!m_textCursor.atStart() && !m_textCursor.hasSelection()) {
                if (ctrl) {
                    selection::moveToPreviousWord(m_textCursor, QTextCursor::KeepAnchor);
                } else if (m_textCursor.block().text().isEmpty() && !m_textCursor.atEnd()) {
                    // Workaround for scene breaks. Backspacing on an empty block after a scene break
                    // for some reason doesn't just delete the empty block but destroys the scene
                    // break format as well (even though the same doesn't happen for headings),
                    // so we basically simulate a Key_Delete deletion here and move the cursor
                    // backwards by one.
                    m_textCursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor);
                    moveBack = true;
                } else {
                    m_textCursor.movePosition(QTextCursor::PreviousCharacter, QTextCursor::KeepAnchor);
                }
            }

            remove();

            if (moveBack) {
                moveCursor(QTextCursor::MoveOperation::PreviousCharacter);
            }

            updateActive();
            event->accept();
            break;
        }
        case Qt::Key_Delete:
            if (!m_textCursor.atEnd() && !m_textCursor.hasSelection()) {
                if (ctrl) {
                    selection::moveToNextWord(m_textCursor, QTextCursor::KeepAnchor);
                } else {
                    m_textCursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor);
                }
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
                    if (m_textCursor.selectionEnd() - m_textCursor.selectionStart() == 1
                            && symbols::isWordSeparator(m_document->characterAt(m_textCursor.position()))) {
                        this->selectParagraph();
                    } else if (symbols::isWordSeparator(m_document->characterAt(m_textCursor.selectionStart() - 1))
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
            bool changes = false;
            const QString text = symbols::sanitize(event->text());

            if (!text.isEmpty() && (m_textCursor.blockFormat() != format::sceneBreakFormat || symbols::isNewLine(text[0]))) {
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
                    changes = true;
                } else {
                    const QString replacedText = m_replacer.replace(text, previousCharacter);
                    m_textCursor.insertText(replacedText);
                    moveCursor(QTextCursor::NextCharacter, QTextCursor::MoveAnchor, replacedText.length() - 1);
                    changes = true;
                }

                if (m_textCursor.block().text().isEmpty()) {
                    // Ensures that new blocks always go back to the default
                    // block format instead of continuing in e.g. heading format.
                    m_textCursor.joinPreviousEditBlock();
                    m_textCursor.setBlockFormat(ThemeManager::instance()->activeTheme()->blockFormat());
                    m_textCursor.endEditBlock();
                }
                event->accept();
            } else {
                event->ignore();
            }

            if (hadSelection && changes) {
                emit selectedTextChanged();
            }
            break;
    }
}
