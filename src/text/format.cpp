#include <QTextCursor>
#include <QTextBlock>
#include <QTextDocumentFragment>
#include <QTextDocument>
#include <QDebug>

#include "format.h"
#include "../theming/ThemeManager.h"

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

    const QTextBlockFormat sceneBreakFormat = [] {
        QTextBlockFormat format;
        format.setAlignment(Qt::AlignHCenter);
        format.setTopMargin(80);
        format.setBottomMargin(80);
        return format;
    }();
}

const QTextCharFormat format::getMergedCharFormat(const QTextCursor& textCursor)
{
    if (!textCursor.hasSelection()) {
        return textCursor.charFormat();
    }

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

    if (!startReached) {
        mergeCharFormat(mergedFormat, startBlock.textFormats().last().format);
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

void format::normalize(QTextCursor& textCursor, const Theme* theme, const QString& sceneBreakString)
{
    if (!textCursor.hasSelection())
        return;

    int endingBlock = textCursor.document()->findBlock(textCursor.selectionEnd()).blockNumber();

    QTextCursor iterationCursor(textCursor.document());
    iterationCursor.setPosition(textCursor.selectionStart());

    iterationCursor.joinPreviousEditBlock();

    while (iterationCursor.blockNumber() <= endingBlock) {
        int blockNumber = iterationCursor.blockNumber();
        if (iterationCursor.block().blockFormat().headingLevel()) {
            const HeadingFormat& headingFormat = theme->headingFormat(iterationCursor.block().blockFormat().headingLevel());
            iterationCursor.setBlockFormat(headingFormat.blockFormat());
            iterationCursor.select(QTextCursor::BlockUnderCursor);
            iterationCursor.mergeCharFormat(theme->charFormat());
        } else {
            if (iterationCursor.block().userState() != -1 && iterationCursor.block().userState() & BlockState::SceneBreak) {
                if (iterationCursor.block().text() != sceneBreakString) {
                    insertSceneBreak(iterationCursor, sceneBreakString);
                }
            } else if (iterationCursor.block().text() == sceneBreakString) {
                insertSceneBreak(iterationCursor, sceneBreakString, true);
            } else {
                iterationCursor.setBlockFormat(theme->blockFormat());
            }

            iterationCursor.select(QTextCursor::BlockUnderCursor);
            iterationCursor.mergeCharFormat(theme->charFormat());
        }
        iterationCursor.movePosition(QTextCursor::NextBlock);

        if (blockNumber == iterationCursor.blockNumber()) {
            // Reached the end, abort
            break;
        }
    }

    iterationCursor.endEditBlock();
}

void format::mergeBlockCharFormat(const QTextCursor& textCursor, const QTextCharFormat& format)
{
    // For some reason, QTextCursor::mergeBlockCharFormat() is currently
    // broken (doesn't do anything), so this function aims to reimplement it.

    QTextDocument* document = textCursor.document();
    const QTextBlock& startBlock = document->findBlock(textCursor.selectionStart());
    const QTextBlock& endBlock = document->findBlock(textCursor.selectionEnd());

    QTextCursor tempCursor(document);
    tempCursor.setPosition(startBlock.position());
    tempCursor.setPosition(endBlock.position() + endBlock.length() - 1, QTextCursor::KeepAnchor);
    tempCursor.mergeCharFormat(format);
}

void format::insertSceneBreak(QTextCursor& textCursor, const QString sceneBreakText, bool replaceBlock)
{
    if (sceneBreakText.isEmpty()) {
        return;
    }

    if (!textCursor.block().text().isEmpty() && textCursor.block().text() != sceneBreakText && !replaceBlock) {
        textCursor.insertBlock();
    }

    if (!replaceBlock && ((textCursor.block().userState() != -1 && textCursor.block().userState() & format::SceneBreak) || textCursor.block().text() == sceneBreakText)) {
        // Remove scene break if already exists.
        textCursor.select(QTextCursor::SelectionType::BlockUnderCursor);
        textCursor.removeSelectedText();
    } else {
        if (!textCursor.block().text().isEmpty()) {
            textCursor.select(QTextCursor::SelectionType::BlockUnderCursor);
            textCursor.removeSelectedText();
            textCursor.insertBlock();
        }

        textCursor.block().setUserState((textCursor.block().userState() == -1 ? 0 : textCursor.block().userState()) | format::SceneBreak);
        textCursor.setBlockFormat(sceneBreakFormat);
        textCursor.insertText(sceneBreakText);
    }
}

bool format::isSceneBreak(QTextCursor& textCursor)
{
    return textCursor.block().blockFormat() == sceneBreakFormat;
}
