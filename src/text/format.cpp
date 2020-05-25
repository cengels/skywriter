#include <QTextCursor>
#include <QTextBlock>
#include <QTextDocumentFragment>
#include <QTextDocument>
#include <QDebug>
#include "format.h"

namespace {
    bool isInRange(const QTextLayout::FormatRange& range, const int position)
    {
        return position >= range.start && position <= range.start + range.length;
    }

    void mergeCharFormat(QTextCharFormat& merger, const QTextCharFormat& toMerge)
    {
        if (merger.fontWeight() == QFont::Bold && toMerge.fontWeight() == QFont::Normal) {
            merger.setFontWeight(QFont::Normal);
        }
        if (merger.fontItalic() && !toMerge.fontItalic()) {
            merger.setFontItalic(false);
        }
        // Commented out since Markdown does not support underlining.
        // if (mergedFormat.fontUnderline() && !range.format.fontUnderline()) {
        //     mergedFormat.setFontUnderline(false);
        // }
        if (merger.fontStrikeOut() && !toMerge.fontStrikeOut()) {
            merger.setFontStrikeOut(false);
        }
    }
}

const QTextCharFormat format::getMergedCharFormat(const QTextCursor& textCursor)
{
    const int start = textCursor.selectionStart();
    const int end = textCursor.selectionEnd();
    const QTextBlock startBlock = textCursor.document()->findBlock(start);
    const QTextBlock endBlock = textCursor.document()->findBlock(end);
    bool startReached = false;
    bool sameBlock = startBlock.blockNumber() == endBlock.blockNumber();

    QTextCharFormat mergedFormat;
    mergedFormat.setFontWeight(QFont::Bold);
    mergedFormat.setFontItalic(true);
    mergedFormat.setFontStrikeOut(true);

    for (auto iterator = startBlock.begin(); !iterator.atEnd(); iterator++) {
        bool containsStart = false;
        if (!startReached && iterator.fragment().contains(start)) {
            containsStart = true;
            startReached = true;
        }

        if (sameBlock && !containsStart && iterator.fragment().contains(end)) {
            break;
        }

        if (startReached) {
            mergeCharFormat(mergedFormat, iterator.fragment().charFormat());
        }
    }

    if (!sameBlock) {
        for (auto iterator = endBlock.begin(); !iterator.atEnd(); iterator++) {
            if (iterator.fragment().contains(end)) {
                break;
            }

            mergeCharFormat(mergedFormat, iterator.fragment().charFormat());
        }
    }

    return mergedFormat;
}
