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
    // refreshDocumentStructure() always produces a document structure
    // consistent with the actual text document. It's relatively fast
    // (usually around 20ms per call in medium size documents)
    // which is acceptable during larger one-time operations like
    // loading a new document or pasting something into the document,
    // but not when typing.
    // This function is fast but only handles changes within a
    // document segment, not between them.

    if (m_documentStructure.isEmpty()) {
        return;
    }

    DocumentSegment* segment = findDocumentSegment(position);
    const int positionInSegment = position - segment->position();
    const int segmentLength = segment->length();
    const int charactersLeft = segment->length() - positionInSegment;

    if (removed >= charactersLeft) {
        // Characters were removed beyond the DocumentSegment's boundary,
        // which likely means that two segments have merged.

        refreshDocumentStructure();
        return;
    }

    const int change = added - removed;
    segment->setLength(segmentLength + change);
    segment->updateWordCount();
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
    connect(firstSegment, &DocumentSegment::wordCountChanged, this, &FormattableTextArea::updateWordCount);
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
                const int headingDifference = block.blockFormat().headingLevel() - previousHeading.blockFormat().headingLevel();

                if (!previousHeading.isValid()) {
                    depth = previousSegment->depth();
                } else {
                    depth = previousSegment->depth() + headingDifference;
                }

                DocumentSegment* segment = new DocumentSegment(block.position(), depth, this);
                connect(segment, &DocumentSegment::wordCountChanged, this, &FormattableTextArea::updateWordCount);
                m_documentStructure.append(segment);

                if (previousSegment) {
                    emit previousSegment->textChanged();
                }
            }

            previousHeading = block;
        }

        previous = block;
        block = block.next();
    }

    emit m_documentStructure.last()->textChanged();
    emit documentStructureChanged();
    m_currentDocumentSegment = findDocumentSegment(m_textCursor.position());
    m_lastCaretPosition = m_textCursor.position();
    emit currentDocumentSegmentChanged();

    // Prevent each individual call of segment->updateWordCount()
    // from calling FormattableTextArea::updateWordCount()
    m_loading = true;

    for (DocumentSegment* segment : m_documentStructure) {
        segment->updateWordCount();
    }

    m_loading = false;
    updateWordCount();
}

DocumentSegment* FormattableTextArea::currentDocumentSegment() const
{
    return m_currentDocumentSegment;
}

DocumentSegment* FormattableTextArea::findDocumentSegment(int position) const
{
    if (position < 0 || position >= m_document->characterCount()) {
        return nullptr;
    }

    auto iteratorResult = std::find_if(m_documentStructure.begin(), m_documentStructure.end(), [position](DocumentSegment* segment) {
        // m_documentStructure is guaranteed to be ordered correctly,
        // so we don't need to check segment->length() too.
        return segment->position() <= position;
    });

    if (iteratorResult) {
        return *iteratorResult;
    }

    return nullptr;
}
