#include <QVector>
#include <QtConcurrent>

#include "DocumentSegment.h"
#include "TextIterator.h"
#include "symbols.h"

DocumentSegment::DocumentSegment(QObject *parent) : QObject(parent),
    m_position(0),
    m_depth(0),
    m_words(0)
{
    connect(this, &DocumentSegment::textChanged, this, &DocumentSegment::updateWords);
}

DocumentSegment::DocumentSegment(int position, int depth, QObject* parent) : QObject(parent),
    m_position(position),
    m_depth(depth),
    m_words(0)
{
    connect(this, &DocumentSegment::textChanged, this, &DocumentSegment::updateWords);
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

int DocumentSegment::words() const
{
    return m_words;
}

QString DocumentSegment::text() const
{
    const QTextDocument* doc = document();

    if (!doc) {
        return "";
    }

    const QString& text = doc->toPlainText();

    const DocumentSegment* nextSegment = next();
    int endPosition = nextSegment && nextSegment->isValid() ? nextSegment->position() : -1;

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
        return "";
    }

    return block.text();
}

QString DocumentSegment::subheading() const
{
    QTextBlock block = document()->findBlock(m_position).next();

    if (block.blockFormat().headingLevel() == 0) {
        return "";
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

void DocumentSegment::updateWords()
{
    QtConcurrent::run([=] {
        TextIterator iterator = TextIterator(text(), TextIterator::IterationType::ByWord);
        iterator.ignoreEnclosedBy(symbols::opening_comment, symbols::closing_comment);

        int i = 0;

        while (!iterator.atEnd()) {
            if (!iterator.current().isEmpty()) {
                i++;
            }

            iterator++;
        }

        m_words = i;

        emit wordsChanged();
    });
}
