/////////////////////////////////////////////////////////////////////////////
///                                                                       ///
///   Contains all code responsible for counting words or other symbols   ///
///   within the text document.                                           ///
///                                                                       ///
/////////////////////////////////////////////////////////////////////////////

#include <algorithm>
#include <string>

#include <QTextDocument>
#include <QQuickTextDocument>
#include <QtConcurrent/QtConcurrent>

#include "FormattableTextArea.h"
#include "../TextIterator.h"
#include "../UserData.h"
#include "../symbols.h"

int FormattableTextArea::characterCount() const
{
    return this->m_characterCount;
}

int FormattableTextArea::wordCount() const
{
    return this->m_wordCount;
}

int FormattableTextArea::paragraphCount() const
{
    return this->m_paragraphCount;
}

int FormattableTextArea::pageCount() const
{
    return this->m_pageCount;
}

int FormattableTextArea::selectedCharacterCount() const
{
    return m_selectedCharacterCount;
}

int FormattableTextArea::selectedParagraphCount() const
{
    return m_selectedParagraphCount;
}

int FormattableTextArea::selectedWordCount() const
{
    return m_selectedWordCount;
}

int FormattableTextArea::selectedPageCount() const
{
    return m_selectedPageCount;
}

void FormattableTextArea::updateCharacterCount()
{
    if (!m_document) {
        return;
    }

    const int characterCount = m_document->characterCount();

    if (characterCount != this->m_characterCount) {
        this->m_characterCount = characterCount;
        emit characterCountChanged();
    }

    if (m_textCursor.hasSelection()) {
        const int selectedCharacters = m_textCursor.selectionEnd() - m_textCursor.selectionStart();
        if (m_selectedCharacterCount != selectedCharacters) {
            m_selectedCharacterCount = selectedCharacters;
            emit selectedCharacterCountChanged();
        }
    }
}

void FormattableTextArea::countWords(const int position, const int change)
{
    if (!m_document) {
        return;
    }

    QTextBlock block = m_document->findBlock(position);
    int end = position + change;

    // Maybe try just using one wordIterator and iterating through the whole thing at once?
    // Or just remove QTextCursor completely. There's no real reason to use it. Just access the blocks directly.
    // Another potential optimization is not returning strings from WordIterator, instead
    // just returning booleans.
    while (block.isValid() && block.position() < end) {
        TextIterator wordIterator = this->wordIterator(block);
        int i = 0;

        while (!wordIterator.atEnd()) {
            if (!wordIterator.current().isEmpty()) {
                i++;
            }

            wordIterator++;
        };

        UserData::fromBlock(block).setWordCount(i);

        block = block.next();
    }
}

void FormattableTextArea::updateWordCount()
{
    if (m_loading) {
        return;
    }

    int words = 0;

    for (const DocumentSegment* segment : m_documentStructure) {
        words += segment->wordCount();
    }

    if (words != this->m_wordCount) {
        m_wordCount = words;
        emit wordCountChanged();
    }

    this->updatePageCount();
}

void FormattableTextArea::updateSelectedWordCount()
{
    if (!m_document) {
        return;
    }

    if (!m_textCursor.hasSelection()) {
        if (m_selectedWordCount != 0) {
            m_selectedWordCount = 0;
            emit selectedWordCountChanged();
        }

        if (m_selectedPageCount != 0) {
            m_selectedPageCount = 0;
            emit selectedPageCountChanged();
        }

        return;
    }

    TextIterator wordIterator = this->wordIterator(m_textCursor);
    int i = 0;

    while (!wordIterator.atEnd()) {
        if (!wordIterator.current().isEmpty()) {
            i++;
        }

        wordIterator++;
    };

    if (i != this->m_selectedWordCount) {
        m_selectedWordCount = i;
        emit selectedWordCountChanged();
    }

    const int selectedPages = m_selectedWordCount / 250 + (m_selectedWordCount % 250 != 0 ? 1 : 0);
    if (m_selectedPageCount != selectedPages) {
        m_selectedPageCount = selectedPages;
        emit selectedPageCountChanged();
    }
}

void FormattableTextArea::updateParagraphCount()
{
    if (!m_document) {
        return;
    }

    const int blockCount = m_document->blockCount();

    if (blockCount != this->m_paragraphCount) {
        this->m_paragraphCount = blockCount;
        emit paragraphCountChanged();
    }

    if (m_textCursor.hasSelection()) {
        const int startBlock = m_document->findBlock(m_textCursor.selectionStart()).blockNumber();
        const int endBlock = m_document->findBlock(m_textCursor.selectionEnd()).blockNumber();
        const int selectedParagraphs = endBlock - startBlock + 1;
        if (m_selectedParagraphCount != selectedParagraphs) {
            m_selectedParagraphCount = selectedParagraphs;
            emit selectedParagraphCountChanged();
        }
    }
}

void FormattableTextArea::updatePageCount()
{
    if (!m_document) {
        return;
    }

    const int wordCount = this->wordCount();
    const int pageCount = wordCount / 250 + (wordCount % 250 != 0 ? 1 : 0);

    if (pageCount != this->m_pageCount) {
        this->m_pageCount = pageCount;
        emit pageCountChanged();
    }
}

void FormattableTextArea::updateCounts()
{
    if (m_document == nullptr) {
        return;
    }

    this->updateCharacterCount();
    // no update word count here since it's an expensive operation.
    this->updateParagraphCount();
}

void FormattableTextArea::updateSelectedCounts()
{
    if (m_document == nullptr) {
        return;
    }

    this->updateCharacterCount();
    this->updateParagraphCount();
    this->updateSelectedWordCount();
}

TextIterator FormattableTextArea::wordIterator(const QTextCursor& cursor) const
{
    return TextIterator(cursor, TextIterator::IterationType::ByWord);
}

TextIterator FormattableTextArea::wordIterator(const QTextBlock& block) const
{
    return TextIterator(block, TextIterator::IterationType::ByWord);
}
