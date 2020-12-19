#include <QAbstractTextDocumentLayout>

#include "TextHighlighter.h"
#include "../theming/ThemeManager.h"

namespace {
    QColor searchMatchBackground() {
        const QColor fontColor = ThemeManager::instance()->activeTheme()->fontColor();
        return QColor(fontColor.red(), fontColor.green(), fontColor.blue(), 35);
    }
}

TextHighlighter::TextHighlighter(QTextDocument* parent) : QObject(parent),
    m_findRanges(QVector<Range<int>>()),
    m_findRangeIterator(m_findRanges.constBegin()),
    m_atY(0)
{ }

const QVector<Range<int>>& TextHighlighter::findRanges() const
{
    return m_findRanges;
}

void TextHighlighter::setFindRanges(const QVector<Range<int>>& ranges)
{
    m_findRanges = ranges;
    emit this->needsRepaint();
}

void TextHighlighter::startHighlighting(int atY)
{
    m_findRangeIterator = m_findRanges.constBegin();
    m_atY = atY;
}

void TextHighlighter::highlightBlock(QQuickTextNode& documentNode, const QTextBlock& block)
{
    while (m_findRangeIterator != m_findRanges.constEnd()
        && m_findRangeIterator->from() < block.position()
        && m_findRangeIterator->to() < block.position()) {
        m_findRangeIterator++;
    }

    int baseX = block.document()->documentLayout()->blockBoundingRect(block).topLeft().x();

    while (m_findRangeIterator != m_findRanges.constEnd()) {
        if (!block.contains(m_findRangeIterator->from()) || !block.contains(m_findRangeIterator->to())) {
            return;
        }

        int blockStart = m_findRangeIterator->from() - block.position();
        int blockEnd = m_findRangeIterator->to() - block.position();
        QTextLine startLine = block.layout()->lineForTextPosition(blockStart);
        QTextLine endLine = block.layout()->lineForTextPosition(blockEnd);
        bool isSameLine = startLine.lineNumber() == endLine.lineNumber();

        qreal startX = startLine.cursorToX(blockStart) + baseX;
        qreal width = isSameLine
            ? endLine.cursorToX(blockEnd, QTextLine::Edge::Trailing) + baseX - startX
            : startLine.width() + baseX - startX;
        const QRectF rect = QRectF(startX, startLine.y() + m_atY + block.layout()->position().y(),
                                   width, startLine.height());
        documentNode.addRectangleNode(rect, searchMatchBackground());

        if (!isSameLine) {
            if (startLine.lineNumber() + 1 != endLine.lineNumber()) {
                // Range spans at least three lines
                QTextLine midStartLine = block.layout()->lineAt(startLine.lineNumber() + 1);
                QTextLine midEndLine = block.layout()->lineAt(endLine.lineNumber() - 1);
                const QRectF midRect = QRectF(midStartLine.position().x() + baseX, midStartLine.y() + m_atY + block.layout()->position().y(),
                                              midStartLine.width(), midEndLine.position().y() + midEndLine.height() - midStartLine.position().y());
                documentNode.addRectangleNode(midRect, searchMatchBackground());
            }

            // Range spans two or more lines
            qreal endX = endLine.position().x() + baseX;
            const QRectF endRect = QRectF(endX, endLine.y() + m_atY + block.layout()->position().y(),
                                          endLine.cursorToX(blockEnd, QTextLine::Edge::Trailing) + baseX - endX, endLine.height());
            documentNode.addRectangleNode(endRect, searchMatchBackground());
        }

        m_findRangeIterator++;
    }
}
