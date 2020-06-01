#ifndef FORMAT_H
#define FORMAT_H

#include <QTextCursor>
#include "../theming/Theme.h"

//! Contains convenience methods related to text formatting.
namespace format {
    //! Gets a new QTextCharFormat containing all the formats found in the text
    //! cursor's selection, or at its position if no text is selected.
    //! This function only returns the value if the entire range is styled.
    //! For instance, if the cursor selects a range where half the range is
    //! italicized and half the range is not, the resulting format will have
    //! fontItalics() set to false.
    const QTextCharFormat getMergedCharFormat(const QTextCursor& textCursor);
    //! Normalizes the format in the QTextCursor's selected range by making
    //! sure it conforms to the standards of the passed Theme.
    void normalize(QTextCursor& textCursor, const Theme* theme, const QString& sceneBreakString);
    //! Merges the given char format into all currently selected blocks.
    //! Like QTextCursor::mergeBlockCharFormat, but actually works.
    void mergeBlockCharFormat(const QTextCursor& textCursor, const QTextCharFormat& format);
    void insertSceneBreak(QTextCursor& textCursor, const QString sceneBreakText);

    enum BlockState {
        None = 0,
        EndsWithUnclosedComment = 1,
        Heading = 2,
        SceneBreak = 4
    };
}

#endif // FORMAT_H
