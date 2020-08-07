/////////////////////////////////////////////////////////////////////////////
///                                                                       ///
///         Contains simple actions common to most text editors.          ///
///                                                                       ///
/////////////////////////////////////////////////////////////////////////////

#include <QGuiApplication>
#include <QMimeData>
#include <QClipboard>
#include <QTextDocumentFragment>

#include "FormattableTextArea.h"
#include "../../ErrorManager.h"

void FormattableTextArea::toggleBold()
{
    QTextCharFormat format;
    format.setFontWeight(getSelectionFormat().fontWeight() == QFont::Bold ? QFont::Normal : QFont::Bold);
    mergeFormat(format);
}

void FormattableTextArea::toggleItalics()
{
    QTextCharFormat format;
    format.setFontItalic(!getSelectionFormat().fontItalic());
    mergeFormat(format);
}

void FormattableTextArea::toggleStrikethrough()
{
    QTextCharFormat format;
    format.setFontStrikeOut(!getSelectionFormat().fontStrikeOut());
    mergeFormat(format);
}

void FormattableTextArea::applyHeading(int level)
{
    if (level < 0 || level > 6) {
        emit ErrorManager::instance()->warning(tr("Only headings between 0 and 6 are permitted."));
        return;
    }

    if (level == 0) {
        const Theme* theme = ThemeManager::instance()->activeTheme();
        m_textCursor.setBlockFormat(theme->blockFormat());
        format::mergeBlockCharFormat(m_textCursor, theme->charFormat());
    } else {
        const HeadingFormat& format = ThemeManager::instance()->activeTheme()->headingFormat(level);
        m_textCursor.setBlockFormat(format.blockFormat());
    }

    refreshDocumentStructure();

    updateActive();
}

void FormattableTextArea::insertSceneBreak()
{
    if (m_sceneBreak.isEmpty()) {
        return;
    }

    m_textCursor.beginEditBlock();

    format::insertSceneBreak(m_textCursor, m_sceneBreak);

    m_textCursor.endEditBlock();
}

void FormattableTextArea::copy()
{
    if (m_textCursor.hasSelection()) {
        bool couldPaste = canPaste();

        QClipboard* clipboard = QGuiApplication::clipboard();
        QMimeData* mimeData = new QMimeData();
        mimeData->setText(m_textCursor.selection().toPlainText());
        mimeData->setHtml(m_textCursor.selection().toHtml("utf-8"));
        clipboard->setMimeData(mimeData);

        if (!couldPaste) {
            emit canPasteChanged();
        }
    }
}

void FormattableTextArea::paste()
{
    if (!canPaste()) {
        return;
    }

    bool hadSelection = m_textCursor.hasSelection();
    int previousPosition = m_textCursor.selectionStart();
    QString insertedString;
    const QMimeData* mimeData = QGuiApplication::clipboard()->mimeData();

    if (mimeData->hasHtml()) {
        m_textCursor.insertHtml(mimeData->html());
        int newPosition = m_textCursor.position();
        m_textCursor.setPosition(previousPosition);
        m_textCursor.setPosition(newPosition, QTextCursor::KeepAnchor);
        insertedString = m_textCursor.selectedText();
        format::normalize(m_textCursor, ThemeManager::instance()->activeTheme(), m_sceneBreak);
        m_textCursor.clearSelection();
    } else if (mimeData->hasText()) {
        insertedString = mimeData->text();
        m_textCursor.insertText(mimeData->text());
    } else {
        return;
    }

    updateActive();
    emit caretPositionChanged();

    if (hadSelection) {
        emit selectedTextChanged();
    }
}

void FormattableTextArea::pasteUnformatted()
{
    if (!canPaste()) {
        return;
    }

    bool hadSelection = m_textCursor.hasSelection();
    QString insertedString;
    const QMimeData* mimeData = QGuiApplication::clipboard()->mimeData();

    if (mimeData->hasText()) {
        insertedString = mimeData->text();
        m_textCursor.insertText(mimeData->text());
    } else {
        return;
    }

    updateActive();
    emit caretPositionChanged();

    if (hadSelection) {
        emit selectedTextChanged();
    }
}

void FormattableTextArea::remove()
{
    if (m_textCursor.hasSelection()) {
        int position = m_textCursor.selectionStart();
        QString text = m_textCursor.selectedText();

        if (text.size() == 1) {
            // This distinction is made because removeSelectedText() immediately
            // adds a new undo state, which is undesired for small changes
            // that add or remove only one character.
            m_textCursor.clearSelection();
            m_textCursor.setPosition(position);
            m_textCursor.deleteChar();
        } else {
            m_textCursor.removeSelectedText();
        }

        emit selectedTextChanged();
        emit caretPositionChanged();
    }
}

void FormattableTextArea::cut()
{
    copy();
    remove();
}

void FormattableTextArea::undo()
{
    bool hadSelection = m_textCursor.hasSelection();
    m_document->undo(&m_textCursor);
    updateActive();
    emit caretPositionChanged();

    if (hadSelection) {
        emit selectedTextChanged();
    }

    refreshDocumentStructure();

    if (!m_searchResults.isEmpty()) {
        find(m_searchString, m_searchFlags);
    }
}

void FormattableTextArea::redo()
{
    bool hadSelection = m_textCursor.hasSelection();
    m_document->redo(&m_textCursor);
    updateActive();
    emit caretPositionChanged();

    if (hadSelection) {
        emit selectedTextChanged();
    }

    refreshDocumentStructure();

    if (!m_searchResults.isEmpty()) {
        find(m_searchString, m_searchFlags);
    }
}

void FormattableTextArea::selectWord()
{
    m_textCursor.select(QTextCursor::SelectionType::WordUnderCursor);
    emit caretPositionChanged();
    emit selectedTextChanged();
    update();
}

void FormattableTextArea::selectParagraph()
{
    m_textCursor.select(QTextCursor::SelectionType::BlockUnderCursor);
    emit caretPositionChanged();
    emit selectedTextChanged();
    update();
}

void FormattableTextArea::selectAll()
{
    m_textCursor.select(QTextCursor::SelectionType::Document);
    emit caretPositionChanged();
    emit selectedTextChanged();
    update();
}
