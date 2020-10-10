#include <QTextDocument>
#include <QTextBlock>
#include <QDebug>
#include "selection.h"
#include "symbols.h"

void selection::selectWord(QTextCursor& cursor)
{
    int start = cursor.anchor();
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
