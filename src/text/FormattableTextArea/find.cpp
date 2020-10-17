/////////////////////////////////////////////////////////////////////////////
///                                                                       ///
///   Contains all code responsible for the find and replace function.    ///
///                                                                       ///
/////////////////////////////////////////////////////////////////////////////

#include "FormattableTextArea.h"
#include "../UserData.h"

void FormattableTextArea::find(const QString& searchString, const SearchOptions options)
{
    m_searchString = searchString;
    m_searchFlags = options;

    const int previousSearchResultsCount = searchResultCount();
    m_searchResults.clear();

    if (searchString.isEmpty()) {
        emit searchResultsChanged();

        if (previousSearchResultsCount != searchResultCount()) {
            emit searchResultCountChanged();
        }

        m_highlighter->refresh();

        return;
    }

    QTextDocument::FindFlags flags = 0x0;
    QTextCursor cursor(m_document);
    bool stopOnSelection = false;

    if (options.testFlag(SearchOption::WholeWords)) {
        flags.setFlag(QTextDocument::FindFlag::FindWholeWords);
    }

    if (options.testFlag(SearchOption::CaseSensitive)) {
        flags.setFlag(QTextDocument::FindFlag::FindCaseSensitively);
    }

    if (options.testFlag(SearchOption::InSelection) && m_textCursor.hasSelection()) {
        cursor.setPosition(m_textCursor.selectionStart());
        stopOnSelection = true;
    }

    const bool useRegEx = options.testFlag(SearchOption::RegEx);
    QRegularExpression regEx = useRegEx ? QRegularExpression(searchString) : QRegularExpression();

    while (!cursor.isNull()) {
        if (useRegEx) {
            cursor = m_document->find(regEx, cursor, flags);
        } else {
            cursor = m_document->find(searchString, cursor, flags);
        }

        if (!cursor.isNull() && (!stopOnSelection || cursor.selectionEnd() <= m_textCursor.selectionEnd())) {
            const Range<int> range = Range<int>(cursor.selectionStart(), cursor.selectionEnd());
            m_searchResults.append(range);
        }
    }

    emit searchResultsChanged();

    if (previousSearchResultsCount != searchResultCount()) {
        emit searchResultCountChanged();
    }

    m_highlighter->refresh();
}

void FormattableTextArea::jumpToNext()
{
    if (searchResults().isEmpty()) {
        return;
    }

    bool found = false;

    for (const Range<int>& range : searchResults()) {
        if (m_textCursor.selectionStart() < range.from() || m_textCursor.selectionEnd() == range.from()) {
            QString previousSelection = m_textCursor.selectedText();
            m_textCursor.setPosition(range.from());
            m_textCursor.setPosition(range.until(), QTextCursor::KeepAnchor);
            updateActive();
            found = true;

            emit caretPositionChanged();

            if (previousSelection != m_textCursor.selectedText()) {
                emit selectedTextChanged();
            }

            return;
        }
    }

    if (!found) {
        m_textCursor.movePosition(QTextCursor::Start);
        jumpToNext();
    }
}

void FormattableTextArea::jumpToPrevious()
{
    if (searchResults().isEmpty()) {
        return;
    }

    bool found = false;
    QVectorIterator<Range<int>> iterator(searchResults());
    iterator.toBack();

    while (iterator.hasPrevious()) {
        const Range<int>& range = iterator.previous();
        if (m_textCursor.selectionStart() >= range.until()) {
            QString previousSelection = m_textCursor.selectedText();
            m_textCursor.setPosition(range.from());
            m_textCursor.setPosition(range.until(), QTextCursor::KeepAnchor);
            updateActive();
            found = true;

            emit caretPositionChanged();

            if (previousSelection != m_textCursor.selectedText()) {
                emit selectedTextChanged();
            }

            return;
        }
    }

    if (!found) {
        m_textCursor.movePosition(QTextCursor::End);
        jumpToPrevious();
    }
}

void FormattableTextArea::clearMatches()
{
    find(QString());
}

void FormattableTextArea::replaceNext(const QString& text)
{
    if (searchResults().isEmpty()) {
        return;
    }

    bool found = false;

    for (const Range<int>& range : searchResults()) {
        if (m_textCursor.selectionStart() <= range.from() || m_textCursor.selectionEnd() == range.from()) {
            QString previousSelection = m_textCursor.selectedText();
            m_textCursor.setPosition(range.from());
            m_textCursor.setPosition(range.until(), QTextCursor::KeepAnchor);
            found = true;

            m_searchResults.removeOne(range);

            break;
        }
    }

    if (!found) {
        m_textCursor.movePosition(QTextCursor::Start);
        replaceNext(text);
    }

    m_textCursor.insertText(text);
    m_textCursor.setPosition(m_textCursor.position() - text.size(), QTextCursor::KeepAnchor);

    updateActive();
    emit selectedTextChanged();
    emit caretPositionChanged();
    emit searchResultsChanged();
    emit searchResultCountChanged();
}

void FormattableTextArea::replaceAll(const QString& text)
{
    if (searchResults().isEmpty()) {
        return;
    }

    QTextCursor cursor(m_document);

    cursor.beginEditBlock();

    // Must add the difference to all future ranges.
    int difference = 0;

    for (const Range<int>& range : searchResults()) {
        cursor.setPosition(range.from() + difference);
        cursor.setPosition(range.until() + difference, QTextCursor::KeepAnchor);
        difference += text.size() - range.length();
        cursor.insertText(text);
    }

    cursor.endEditBlock();

    updateActive();

    clearMatches();
}

void FormattableTextArea::updateFindRanges()
{
    // Can't check anything based on what characters were added/removed here
    // since inserted characters may have resulted in a new match and
    // removed characters may have resulted in a match being lost.
    // The only possible optimization would be to limit the find() rerun
    // to the block(s) in which the changes occurred, but as long as there
    // is no performance problem, that would be a premature optimization.

    if (!m_searchString.isEmpty()) {
        find(m_searchString, m_searchFlags);
    }
}
