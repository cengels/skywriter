/////////////////////////////////////////////////////////////////////////////
///                                                                       ///
///   Contains all code responsible for counting words or other symbols   ///
///   within the text document.                                           ///
///                                                                       ///
/////////////////////////////////////////////////////////////////////////////

#include <QTextDocument>
#include <QQuickTextDocument>
#include <QTextDocument>

#include "FormattableTextArea.h"
#include "../TextIterator.h"
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

void FormattableTextArea::updateCharacterCount()
{
    const int characterCount = m_document->characterCount();

    if (characterCount != this->m_characterCount) {
        this->m_characterCount = characterCount;
        emit characterCountChanged();
    }
}

void FormattableTextArea::updateWordCount()
{
    TextIterator textIterator = this->wordIterator();
    int i = 0;

    while (!textIterator.atEnd()) {
        if (!textIterator.current().isEmpty()) {
            i++;
        }

        textIterator++;
    };

    if (i != this->m_wordCount) {
        m_wordCount = i;
        emit wordCountChanged();
    }
}

void FormattableTextArea::updateParagraphCount()
{
    const int blockCount = m_document->blockCount();

    if (blockCount != this->m_paragraphCount) {
        this->m_paragraphCount = blockCount;
        emit paragraphCountChanged();
    }
}

void FormattableTextArea::updatePageCount()
{
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
    this->updateWordCount();
    this->updateParagraphCount();
    this->updatePageCount();
}

TextIterator FormattableTextArea::wordIterator() const
{
    TextIterator iterator = TextIterator(m_document->toPlainText(), TextIterator::IterationType::ByWord);
    iterator.ignoreEnclosedBy(symbols::opening_comment, symbols::closing_comment);

    return iterator;
}
