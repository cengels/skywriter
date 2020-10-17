#include <QTextDocument>
#include <QTextBlock>
#include <QDebug>
#include "selection.h"
#include "symbols.h"

namespace {
    bool isSeparator(const QChar& character) {
        return symbols::isWordSeparator(character) || symbols::stop_on_selection.contains(character);
    }
}

void selection::selectWord(QTextCursor& cursor)
{
    int start = cursor.position();
    int end = start;
    QChar current;

    if (cursor.atBlockEnd()) {
        const bool onlySeparators = symbols::isWordSeparator(current);
        start--;
        current = cursor.document()->characterAt(start - 1);

        while (symbols::isWordSeparator(current) == onlySeparators && current != symbols::space) {
            start--;
            current = cursor.document()->characterAt(start - 1);
        }

        start--;
    } else {
        // If the cursor is at the end of a word rather than inside of one,
        // must adjust the start position to handle that.
        if (symbols::isWordSeparator(cursor.document()->characterAt(start))
        && !symbols::isWordSeparator(cursor.document()->characterAt(start - 1))
        && !cursor.atStart()) {
            start--;
        }

        end = start;

        const bool onlySeparators = symbols::isWordSeparator(cursor.document()->characterAt(start));

        current = cursor.document()->characterAt(start);

        while (symbols::isWordSeparator(current) == onlySeparators && current != symbols::space) {
            start--;
            current = cursor.document()->characterAt(start);
        }

        current = cursor.document()->characterAt(end);

        while (symbols::isWordSeparator(current) == onlySeparators && current != symbols::space) {
            end++;
            current = cursor.document()->characterAt(end);
        }
    }

    cursor.setPosition(start + 1);
    cursor.setPosition(end, QTextCursor::KeepAnchor);
}

void selection::selectPreviousWord(QTextCursor& cursor, const QTextCursor::MoveMode mode, const bool ignoreBlockBoundaries)
{
    if (cursor.atStart()) {
        return;
    }

    const int end = cursor.position();
    const bool atBlockStart = cursor.atBlockStart();
    const int blockStart = atBlockStart
                           ? cursor.block().previous().position()
                           : cursor.block().position();
    int start = end;
    int symbolCount = 0;
    QChar current = cursor.document()->characterAt(start - 1);
    QChar last;

    // Select all initial word separators.
    while (start > blockStart && isSeparator(current)) {
        start--;

        if (current.category() == QChar::Separator_Paragraph) {
            if (!ignoreBlockBoundaries) {
                break;
            }
        } else {
            symbolCount++;
        }

        last = current;
        current = cursor.document()->characterAt(start - 1);
    }

    // Should only ever select subsequent words if less than two symbols
    // were selected in the same pass (excluding paragraph separators).
    // For instance, it will select BLOCK_END.\nBLOCK_START, but not BLOCK_END...\nBLOCK_START.
    // This is not a hard-and-fast rule and may be subject to change later.
    // User experience tests are necessary here to see if this behaviour is intuitive.
    if (symbolCount <= 1) {
        // Then, select everything up to the next word separator, unless there's
        // a newline.
        while (start > blockStart && !isSeparator(current)) {
            start--;
            current = cursor.document()->characterAt(start - 1);
        }
    } else {
        const int initialStart = start;
        // The selection may not end in whitespace characters, so deselect them
        // here if they are the last symbols selected.
        while (last.category() == QChar::Separator_Space) {
            start++;
            last = cursor.document()->characterAt(start);
        }

        // If all characters were deselected, undo this (something must be selected).
        if (start == end) {
            start = initialStart;
        }
    }

    cursor.setPosition(start, mode);
}

void selection::selectNextWord(QTextCursor& cursor, const QTextCursor::MoveMode mode, const bool ignoreBlockBoundaries)
{
    if (cursor.atEnd()) {
        return;
    }

    const int start = cursor.position();
    const int blockEnd = cursor.atBlockEnd()
                         ? cursor.block().next().position() + cursor.block().next().length()
                         : cursor.block().position() + cursor.block().length();
    int end = start;
    int symbolCount = 0;
    QChar current = cursor.document()->characterAt(end);
    QChar last;

    // Select all initial word separators.
    while (end + 1 < blockEnd && isSeparator(current)) {
        end++;

        if (current.category() == QChar::Separator_Paragraph) {
            if (!ignoreBlockBoundaries) {
                break;
            }
        } else {
            symbolCount++;
        }

        last = current;
        current = cursor.document()->characterAt(end);
    }

    // see above
    if (symbolCount <= 1) {
        // Then, select everything up to the next word separator, unless there's
        // a newline.
        while (end + 1 < blockEnd && !isSeparator(current)) {
            end++;
            current = cursor.document()->characterAt(end);
        }
    } else {
        const int initialEnd = end;
        // The selection may not end in whitespace characters, so deselect them
        // here if they are the last symbols selected.
        while (last.category() == QChar::Separator_Space) {
            end--;
            last = cursor.document()->characterAt(end - 1);
        }

        // If all characters were deselected, undo this (something must be selected).
        if (start == end) {
            end = initialEnd;
        }
    }

    cursor.setPosition(end, mode);
}
