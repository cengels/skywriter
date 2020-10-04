#include <QTextDocument>
#include <QDebug>
#include "selection.h"
#include "symbols.h"

void selection::selectWord(QTextCursor& cursor)
{
    int start = cursor.anchor();
    int end = start;

    while (!symbols::isWordSeparator(cursor.document()->characterAt(start))) {
        start--;
    }

    while (!symbols::isWordSeparator(cursor.document()->characterAt(end))) {
        end++;
    }

    cursor.setPosition(start + 1);
    cursor.setPosition(end, QTextCursor::KeepAnchor);
}
