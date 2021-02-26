#include <QVector>

#include "UserData.h"
#include "DocumentSegment.h"
#include "TextIterator.h"
#include "symbols.h"

DocumentSegment::DocumentSegment(QObject *parent) : QObject(parent),
    m_position(0),
    m_depth(0),
    m_wordCount(0)
{
    setAutoDelete(false);
}

DocumentSegment::DocumentSegment(int position, int depth, QObject* parent) : QObject(parent),
    m_position(position),
    m_depth(depth),
    m_wordCount(0)
{
    setAutoDelete(false);
}

int DocumentSegment::position() const
{
    return m_position;
}

void DocumentSegment::setPosition(int position)
{
    m_position = position;

    emit positionChanged();
}

int DocumentSegment::length() const
{
    const QTextDocument* doc = document();

    if (!doc) {
        return 0;
    }

    const DocumentSegment* nextSegment = next();

    if (nextSegment) {
        return nextSegment->position() - position();
    } else {
        return doc->characterCount() - position();
    }
}

int DocumentSegment::wordCount() const
{
    return m_wordCount;
}

QString DocumentSegment::text() const
{
    const QTextDocument* doc = document();

    if (!doc) {
        return QString();
    }

    const QString& text = doc->toPlainText();

    const DocumentSegment* nextSegment = next();
    int endPosition = nextSegment && nextSegment->isValid() ? nextSegment->position() - m_position : -1;

    return text.mid(m_position, endPosition);
}

DocumentSegment* DocumentSegment::next() const
{
    if (!parent()) {
        return nullptr;
    }

    const FormattableTextArea* textArea = qobject_cast<FormattableTextArea*>(parent());

    if (!textArea->document()) {
        return nullptr;
    }

    const int index = this->index();

    if (index == -1 || index == textArea->documentStructure().size() - 1) {
        return nullptr;
    }

    return textArea->documentStructure().at(index + 1);
}

DocumentSegment* DocumentSegment::previous() const
{
    if (!parent()) {
        return nullptr;
    }

    const FormattableTextArea* textArea = qobject_cast<FormattableTextArea*>(parent());

    if (!textArea->document()) {
        return nullptr;
    }

    const int index = this->index();

    if (index <= 0) {
        return nullptr;
    }

    return textArea->documentStructure().at(index - 1);
}

QTextDocument* DocumentSegment::document() const
{
    if (!parent()) {
        return nullptr;
    }

    const FormattableTextArea* textArea = qobject_cast<FormattableTextArea*>(parent());

    return textArea->document();
}

QString DocumentSegment::heading() const
{
    const QTextBlock block = document()->findBlock(m_position);

    if (block.blockFormat().headingLevel() == 0) {
        return QString();
    }

    return block.text();
}

QString DocumentSegment::subheading() const
{
    QTextBlock block = document()->findBlock(m_position).next();

    if (block.blockFormat().headingLevel() == 0 || block.blockFormat().headingLevel() % 2 != 0) {
        return QString();
    }

    return block.text();
}

int DocumentSegment::depth() const
{
    return m_depth;
}

bool DocumentSegment::isValid() const
{
    const QTextDocument* doc = document();

    if (!doc) {
        return false;
    }

    return m_position > 0 && m_position < doc->characterCount() && m_depth > 0;
}

int DocumentSegment::index() const
{
    if (!parent()) {
        return -1;
    }

    const FormattableTextArea* textArea = qobject_cast<FormattableTextArea*>(parent());

    if (!textArea->document()) {
        return -1;
    }

    // Can't use indexOf here due to an incomprehensible compiler error.
    for (int i = 0; i < textArea->documentStructure().size(); i++) {
        if (textArea->documentStructure().at(i) == this) {
            return i;
        }
    }

    return -1;
}

QTextBlock DocumentSegment::firstBlock() const
{
    const QTextDocument* doc = this->document();

    if (doc == nullptr) {
        return QTextBlock();
    }

    return doc->findBlock(this->position());
}

QTextBlock DocumentSegment::lastBlock() const
{
    const QTextDocument* doc = this->document();

    if (doc == nullptr) {
        return QTextBlock();
    }

    return doc->findBlock(this->position() + this->length() - 1);
}

void DocumentSegment::updateWordCount()
{
    QTextBlock block = this->firstBlock();
    const QTextBlock lastBlock = this->lastBlock();

    m_wordCount = 0;

    while (block.isValid() && block != lastBlock) {
        m_wordCount += UserData::fromBlock(block).wordCount();
        block = block.next();
    }

    emit wordsChanged();
}

bool DocumentSegment::operator==(const DocumentSegment& other) const
{
    return position() == other.position() && depth() == other.depth();
}

bool DocumentSegment::operator==(const DocumentSegment* other) const
{
    if (!other) {
        return false;
    }

    return position() == other->position() && depth() == other->depth();
}

bool DocumentSegment::operator!=(const DocumentSegment& other) const
{
    return !(*this == other);
}

bool DocumentSegment::operator!=(const DocumentSegment* other) const
{
    return !(*this == other);
}
