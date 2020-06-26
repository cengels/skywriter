/////////////////////////////////////////////////////////////////////////////
///                                                                       ///
///    Contains all code responsible for establishing and updating the    ///
///    document structure.                                                ///
///                                                                       ///
/////////////////////////////////////////////////////////////////////////////

#include "FormattableTextArea.h"
#include "../symbols.h"

void FormattableTextArea::updateDocumentStructure(const int position, const int added, const int removed)
{
    const int change = added - removed;

    if (m_documentStructure.isEmpty()) {
        return;
    }

    if (change > 0) {
        // Text was inserted.

        DocumentSegment* targetSegment = nullptr;

        for (DocumentSegment* segment : m_documentStructure) {
            if (targetSegment) {
                segment->setPosition(segment->position() + change);
                continue;
            }

            const DocumentSegment* next = segment->next();

            if (next == nullptr || (position >= segment->position() && position < next->position())) {
                targetSegment = segment;
            }
        }

        if (targetSegment) {
            emit targetSegment->textChanged();

            if (position == 0
                    || symbols::isWordSeparator(m_document->characterAt(position - 1))
                    || removed > 0 // If text was removed, it's impossible to guarantee it didn't contain word separators.
                    || (added > 1 && symbols::containsWordSeparator(m_document->toRawText().mid(position, added)))) {
                targetSegment->updateWords();
            }
        }
    } else if (change < 0) {
        // Text was removed.

        int textEnd = position - change;
        int length = m_documentStructure.count();
        int startSegment = -1;
        int endSegment = -1;

        for (int i = 0; i < length; i++) {
            DocumentSegment* segment = m_documentStructure.at(i);
            const int segmentStart = segment->position();
            const int segmentEnd = i < length - 1 ? m_documentStructure.at(i + 1)->position() : m_document->characterCount();

            if (segmentStart <= position && startSegment == -1) {
                startSegment = i;
            }

            if (startSegment != -1 && textEnd < segmentEnd) {
                endSegment = i;
            }

            if (startSegment < i) {
                segment->setPosition(segment->position() + change);
            }
        }

        if (startSegment != endSegment) {
            // Text was removed from more than one DocumentSegment.
            // This means that a DocumentSegment boundary was removed,
            // i.e. the number of DocumentSegments is not the same
            // as before. Must reinstantiate the entire structure.

            refreshDocumentStructure();
            return;
        }

        DocumentSegment* targetSegment = m_documentStructure.at(startSegment);

        if (targetSegment) {
            emit targetSegment->textChanged();

            if (symbols::isWordSeparator(m_document->characterAt(position - 1))) {
                targetSegment->updateWords();
            }
        }
    }
}

void FormattableTextArea::refreshDocumentStructure()
{
    for (DocumentSegment* segment : m_documentStructure) {
        segment->deleteLater();
    }

    m_documentStructure.clear();

    if (!m_document) {
        emit documentStructureChanged();

        return;
    }

    DocumentSegment* firstSegment = new DocumentSegment(0, 1, this);
    connect(firstSegment, &DocumentSegment::wordsChanged, this, &FormattableTextArea::updateWordCount);
    m_documentStructure.append(firstSegment);

    QTextBlock previous = QTextBlock();
    QTextBlock previousHeading = QTextBlock();
    QTextBlock block = m_document->firstBlock();

    while (block.isValid()) {
        bool isHeading = block.blockFormat().headingLevel() > 0;
        bool isSubheading = previous.blockFormat().headingLevel() == block.blockFormat().headingLevel() - 1
                            // uneven headings are considered subheadings
                            && block.blockFormat().headingLevel() % 2 == 0;

        if (isHeading && !isSubheading) {
            if (previous.isValid()) {
                int depth;
                DocumentSegment* const previousSegment = m_documentStructure.last();

                if (!previousHeading.isValid() || previousHeading.blockFormat().headingLevel() == block.blockFormat().headingLevel()) {
                    depth = previousSegment->depth();
                } else {
                    depth = previousSegment->depth() + 1;
                }

                DocumentSegment* segment = new DocumentSegment(block.position(), depth, this);
                connect(segment, &DocumentSegment::wordsChanged, this, &FormattableTextArea::updateWordCount);
                m_documentStructure.append(segment);

                if (previousSegment) {
                    emit previousSegment->textChanged();
                    previousSegment->updateWords();
                }
            }

            previousHeading = block;
        }

        previous = block;
        block = block.next();
    }

    emit m_documentStructure.last()->textChanged();
    m_documentStructure.last()->updateWords();
    emit documentStructureChanged();
}
