#ifndef FORMAT_H
#define FORMAT_H

#include <QTextCursor>
#include <QTextBlockFormat>
#include <QTextCharFormat>
#include "../theming/Theme.h"

//! Contains convenience methods related to text formatting.
namespace format {
    const QTextBlockFormat sceneBreakFormat = [] {
        QTextBlockFormat format;
        format.setAlignment(Qt::AlignHCenter);
        format.setTopMargin(80);
        format.setBottomMargin(80);
        return format;
    }();

    //! Gets a new QTextCharFormat containing all the formats found in the text
    //! cursor's selection, or at its position if no text is selected.
    //! This function only returns the value if the entire range is styled.
    //! For instance, if the cursor selects a range where half the range is
    //! italicized and half the range is not, the resulting format will have
    //! fontItalics() set to false.
    const QTextCharFormat getMergedCharFormat(const QTextCursor& textCursor);
    void insertSceneBreak(QTextCursor& textCursor);
    //! Clears all non-user-defined formats from the QTextCursor's selection.
    void clearFormat(QTextCursor& textCursor);

    enum BlockState {
        None = 0x0,
        NeedsUpdate = 0x1,
        EndsWithUnclosedComment = 0x2,
        Heading = 0x4,
        SceneBreak = 0x8
    };

    Q_DECLARE_FLAGS(BlockStates, BlockState);
}

Q_DECLARE_OPERATORS_FOR_FLAGS(format::BlockStates);

#endif // FORMAT_H
