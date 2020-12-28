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
#include "../selection.h"

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
    } else {
        const HeadingFormat& format = ThemeManager::instance()->activeTheme()->headingFormat(level);
        m_textCursor.setBlockFormat(format.blockFormat());
    }

    refreshDocumentStructure();

    updateActive();
}

void FormattableTextArea::insertSceneBreak()
{
    m_textCursor.beginEditBlock();

    format::insertSceneBreak(m_textCursor);

    m_textCursor.endEditBlock();
}

void FormattableTextArea::copy()
{
    if (m_textCursor.hasSelection()) {
        bool couldPaste = canPaste();

        QClipboard* clipboard = QGuiApplication::clipboard();
        QMimeData* mimeData = new QMimeData();
        mimeData->setText(m_textCursor.selection().toPlainText());
        mimeData->setHtml(m_textCursor.selection().toHtml());
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
    QString insertedString;
    const QMimeData* mimeData = QGuiApplication::clipboard()->mimeData();

    if (mimeData->hasHtml()) {
        QTextDocument tempDocument;
        tempDocument.setUndoRedoEnabled(false);
        auto cursor = QTextCursor(&tempDocument);
        cursor.insertHtml(mimeData->html());
        cursor.select(QTextCursor::Document);

        format::clearFormat(cursor);

        m_textCursor.beginEditBlock();
        if (m_textCursor.blockFormat() == format::sceneBreakFormat) {
            m_textCursor.insertBlock(ThemeManager::instance()->activeTheme()->blockFormat());
        }

        m_textCursor.insertFragment(QTextDocumentFragment(&tempDocument));
        m_textCursor.endEditBlock();
    } else if (mimeData->hasText()) {
        m_textCursor.beginEditBlock();
        if (m_textCursor.blockFormat() == format::sceneBreakFormat) {
            m_textCursor.insertBlock(ThemeManager::instance()->activeTheme()->blockFormat());
        }

        insertedString = mimeData->text();
        m_textCursor.insertText(mimeData->text());
        m_textCursor.endEditBlock();
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
        m_textCursor.beginEditBlock();
        if (m_textCursor.blockFormat() == format::sceneBreakFormat) {
            m_textCursor.insertBlock(ThemeManager::instance()->activeTheme()->blockFormat());
        }

        insertedString = mimeData->text();
        m_textCursor.insertText(mimeData->text());
        m_textCursor.endEditBlock();
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
    m_isUndoRedo = true;
    m_document->undo(&m_textCursor);
    m_isUndoRedo = false;
    updateActive();
    emit caretPositionChanged();

    if (hadSelection) {
        emit selectedTextChanged();
    }

    if (!m_searchString.isEmpty()) {
        find(m_searchString, m_searchFlags);
    }
}

void FormattableTextArea::redo()
{
    bool hadSelection = m_textCursor.hasSelection();
    m_isUndoRedo = true;
    m_document->redo(&m_textCursor);
    m_isUndoRedo = false;
    updateActive();
    emit caretPositionChanged();

    if (hadSelection) {
        emit selectedTextChanged();
    }
}

void FormattableTextArea::selectWord()
{
    selection::selectWord(m_textCursor);
    emit caretPositionChanged();
    emit selectedTextChanged();
    update();
}

void FormattableTextArea::selectParagraph()
{
    const QTextBlock& block = m_textCursor.block();
    m_textCursor.setPosition(block.position());
    m_textCursor.setPosition(block.position() + block.length() - 1, QTextCursor::KeepAnchor);
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
