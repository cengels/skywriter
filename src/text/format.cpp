#include <QTextCursor>
#include <QTextBlock>
#include <QTextDocumentFragment>
#include <QTextDocument>
#include <QDebug>

#include "format.h"
#include "UserData.h"
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

void format::insertSceneBreak(QTextCursor& textCursor)
{
    if (!textCursor.block().text().isEmpty()) {
        textCursor.insertBlock();
    }

    const QTextBlockFormat& format = textCursor.block().blockFormat() == format::sceneBreakFormat
        ? ThemeManager::instance()->activeTheme()->blockFormat()
        : format::sceneBreakFormat;
        
    textCursor.setBlockFormat(format);
}

void format::clearFormat(QTextCursor& textCursor)
{
    bool endReached = false;
    QTextCursor formatter(textCursor.document());

    for (QTextBlock block = textCursor.document()->findBlock(textCursor.selectionStart());
         block.isValid() && !endReached;
         block = block.next())
    {
        if (block.contains(textCursor.selectionEnd())) {
            endReached = true;
        }

        formatter.setPosition(block.position());

        // Ideally we'd leave headings alive here, but that doesn't seem
        // to work even without the below call.
        formatter.setBlockFormat(QTextBlockFormat());

        for (QTextLayout::FormatRange& formatRange : block.textFormats()) {
            // formatRange.format is a copy, so unfortunately we have to
            // call QTextCursor.setCharFormat() afterwards.
            QTextCharFormat& format = formatRange.format;
            format.clearProperty(QTextFormat::FontFamily);
            format.clearProperty(QTextFormat::FontPixelSize);
            format.clearProperty(QTextFormat::FontPointSize);
            format.clearProperty(QTextFormat::LayoutDirection);
            format.clearProperty(QTextFormat::BackgroundImageUrl);
            format.clearProperty(QTextFormat::OutlinePen);
            format.clearProperty(QTextFormat::FontFixedPitch);
            format.clearProperty(QTextFormat::FontSizeAdjustment);
            format.clearProperty(QTextFormat::FontLetterSpacingType);
            format.clearProperty(QTextFormat::FontCapitalization);
            format.clearProperty(QTextFormat::FontLetterSpacing);
            format.clearProperty(QTextFormat::FontWordSpacing);
            format.clearProperty(QTextFormat::FontStretch);
            format.clearProperty(QTextFormat::FontStyleHint);
            format.clearProperty(QTextFormat::FontStretch);
            format.clearProperty(QTextFormat::FontStyleStrategy);
            format.clearProperty(QTextFormat::FontKerning);
            format.clearProperty(QTextFormat::FontHintingPreference);
            format.clearProperty(QTextFormat::FontOverline);
            format.clearProperty(QTextFormat::FontUnderline);
            format.clearProperty(QTextFormat::FontStrikeOut);
            format.clearProperty(QTextFormat::IsAnchor);
            format.clearProperty(QTextFormat::AnchorHref);
            format.clearProperty(QTextFormat::AnchorName);
            format.clearProperty(QTextFormat::FontFamilies);
            formatter.setPosition(block.position() + formatRange.start);
            formatter.setPosition(block.position() + formatRange.start + formatRange.length, QTextCursor::KeepAnchor);
            formatter.setCharFormat(format);
        }
    }
}
