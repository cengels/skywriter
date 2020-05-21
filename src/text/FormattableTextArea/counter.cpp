#include <QTextDocument>
#include "FormattableTextArea.h"

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
    const int characterCount = this->textDocument()->characterCount();

    if (characterCount != this->m_characterCount) {
        this->m_characterCount = characterCount;
        emit characterCountChanged();
    }
}

void FormattableTextArea::updateWordCount()
{
    QTextCursor textCursor = QTextCursor(this->textDocument());
    textCursor.movePosition(QTextCursor::Start);
    int i = 0;

    while (textCursor.movePosition(QTextCursor::NextWord)) {
        i++;
    }

    i -= m_highlighter->comments();

    if (i != this->m_wordCount) {
        m_wordCount = i;
        emit wordCountChanged();
    }
}

void FormattableTextArea::updateParagraphCount()
{
    const int blockCount = this->textDocument()->blockCount();

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
    this->updateCharacterCount();
    this->updateWordCount();
    this->updateParagraphCount();
    this->updatePageCount();
}
