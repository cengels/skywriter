#include <QTextDocument>
#include "formattable_text_area.h"

int FormattableTextArea::characterCount() const {
    return this->m_characterCount;
}

int FormattableTextArea::wordCount() const {
    return this->m_wordCount;
}

int FormattableTextArea::paragraphCount() const {
    return this->m_paragraphCount;
}

int FormattableTextArea::pageCount() const {
    return this->m_pageCount;
}

void FormattableTextArea::updateCharacterCount() {
}

void FormattableTextArea::updateWordCount() {
}

void FormattableTextArea::updateParagraphCount() {
}

void FormattableTextArea::updatePageCount() {
}

void FormattableTextArea::updateCounts() {
    this->updateCharacterCount();
    this->updateWordCount();
    this->updateParagraphCount();
    this->updatePageCount();
}
