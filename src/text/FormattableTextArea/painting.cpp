//////////////////////////////////////////////////////////////////////////////
///                                                                        ///
///        Contains all code responsible for drawing the text area.        ///
///                                                                        ///
//////////////////////////////////////////////////////////////////////////////

#include <QFontMetrics>
#include <QRegularExpression>
#include <QAbstractTextDocumentLayout>
#include <QTextDocumentFragment>
#include <QSGSimpleRectNode>
#include <private/qquicktextnode_p.h>

#include "FormattableTextArea.h"
#include "../../theming/ThemeManager.h"
#include "../format.h"
#include "../UserData.h"

namespace {
    constexpr int WIDTH_HORIZONTAL_RULE = 400;
}

bool FormattableTextArea::event(QEvent* event)
{
    if (event->type() == QEvent::MouseButtonPress) {
        forceActiveFocus();
    } else if (event->type() == QEvent::FocusIn) {
        m_caretTimer.start();

        if (canPaste()) {
            emit canPasteChanged();
        }
    } else if (event->type() == QEvent::FocusOut) {
        m_blinking = false;
        m_caretTimer.stop();
        update();
    }

    return QQuickItem::event(event);
}

void FormattableTextArea::updateStyling()
{
    if (!m_document) {
        return;
    }

    bool wasModified = this->modified();

    const Theme* theme = ThemeManager::instance()->activeTheme();

    m_document->setDefaultFont(theme->font());
    QTextOption textOption = m_document->defaultTextOption();
    textOption.setWrapMode(QTextOption::WordWrap);
    textOption.setAlignment(static_cast<Qt::Alignment>(theme->textAlignment()));
    m_document->setDefaultTextOption(textOption);
    m_document->setTextWidth(this->width());

    QTextCursor cursor(m_document);
    cursor.select(QTextCursor::Document);
    format::normalize(cursor, theme);

    if (!wasModified) {
        this->setModified(false);
    }

    update();
}

QSGNode* FormattableTextArea::updatePaintNode(QSGNode *oldNode, QQuickItem::UpdatePaintNodeData *updatePaintNodeData)
{
    Q_UNUSED(updatePaintNodeData)

    // This may seem like a call without effect, but without it setting the
    // document's text width (when resizing the text area horizontally) results
    // in one paint with an invalid layout, i.e. one frame without any text.
    // It appears this call actually forces a layout pass or something along
    // those lines.
    m_document->documentLayout()->documentSize();

    QQuickTextNode* n = static_cast<QQuickTextNode*>(oldNode);
    if (!n)
        n = new QQuickTextNode(this);
    n->removeAllChildNodes();

    if (!m_document || m_textCursor.isNull()) {
        return n;
    }

    const QColor& fontColor = ThemeManager::instance()->activeTheme()->fontColor();
    const bool hasSelection = m_textCursor.hasSelection();

    if (m_highlighter) {
        m_highlighter->startHighlighting(m_overflowArea - m_contentY);
    }

    const QTextBlock& end = this->m_document->end();
    for (QTextBlock block = this->m_document->begin(); block != end; block = block.next())
    {
        const QPointF& blockPosition = block.layout()->position();
        const double blockHeight = block.layout()->boundingRect().height();

        if (blockPosition.y() + blockHeight + m_overflowArea < m_contentY) {
            continue;
        } else if (blockPosition.y() + m_overflowArea > m_contentY + this->height()) {
            break;
        }

        int selectionStart = -1;
        int selectionEnd = -1;
        bool hasStartSelection = false;
        bool hasEndSelection = false;

        if (hasSelection) {
            hasStartSelection = block.contains(m_textCursor.selectionStart());
            hasEndSelection = block.contains(m_textCursor.selectionEnd());

            if (hasStartSelection) {
                selectionStart = m_textCursor.selectionStart() - block.position();
            } else if (hasEndSelection || (m_textCursor.selectionStart() < block.position() && m_textCursor.selectionEnd() > block.position())) {
                selectionStart = 0;
            }

            if (hasEndSelection) {
                selectionEnd = m_textCursor.selectionEnd() - block.position() - 1;
            } else if (hasStartSelection || (selectionStart >= 0 && m_textCursor.selectionEnd() > block.position())) {
                selectionEnd = block.length() - 1;
            }
        }

        if (block.blockFormat() == format::sceneBreakFormat) {
            this->addHorizontalRule(*n, block, WIDTH_HORIZONTAL_RULE, fontColor);
            continue;
        } else if (!block.text().isEmpty()) {
            n->addTextLayout(QPointF(blockPosition.x(), blockPosition.y() + m_overflowArea - m_contentY),
                             block.layout(),
                             fontColor,
                             QQuickText::TextStyle::Normal,
                             QColor(),
                             QColor(),
                             fontColor,
                             fontColor.lightnessF() > 0.5 ? fontColor.darker(200) : fontColor.lighter(200),
                             selectionStart,
                             selectionEnd);
        }

        if (selectionStart >= 0 && !hasEndSelection) {
            // If the selection exceeds the current block, adds a rectangle at
            // the end to indicate that the paragraph separator is included
            // in the selection.
            this->addWrappedSelectionIndicator(*n, block, fontColor);
        }

        if (m_highlighter) {
            m_highlighter->highlightBlock(*n, block);
        }
    }

    if (this->hasFocus() && (hasSelection || !m_blinking)) {
        // Should be slanted if in italics mode but doesn't seem to be possible
        // with QQuickTextNode.
        n->setCursor(caretRectangle(), fontColor);
    }

    return n;
}

QRectF FormattableTextArea::caretRectangle() const
{
    const QTextLine& line = m_textCursor.block().layout()->lineForTextPosition(m_textCursor.positionInBlock());

    if (!line.isValid()) {
        // Without this check, the program crashes when user clicks "new".
        return QRect();
    }

    qreal x = line.cursorToX(m_textCursor.positionInBlock()) + m_document->documentLayout()->blockBoundingRect(m_textCursor.block()).topLeft().x();

    if (m_textCursor.block().blockFormat() == format::sceneBreakFormat) {
        x += qMin((qreal)WIDTH_HORIZONTAL_RULE, line.width()) / 2;
    }

    return QRectF(x, line.y() + m_textCursor.block().layout()->position().y() + m_overflowArea - m_contentY, 1, line.height());
}

void FormattableTextArea::addHorizontalRule(QQuickTextNode& n, const QTextBlock& block, const qreal width, const QColor& color) {
    const qreal lineWidth = block.layout()->lineAt(0).width();
    const qreal ruleWidth = qMin(width, lineWidth);
    QPointF blockPosition = block.layout()->position();

    n.addRectangleNode(
                QRectF(blockPosition.x() + (lineWidth / 2 - ruleWidth / 2),
                        blockPosition.y() + m_overflowArea - m_contentY + block.layout()->lineAt(0).height() / 2,
                        ruleWidth,
                        1),
                color);
}

void FormattableTextArea::addWrappedSelectionIndicator(QQuickTextNode& n, const QTextBlock& block, const QColor& color) {
    const QTextLine lastLine = block.layout()->lineAt(block.layout()->lineCount() - 1);
    QPointF blockPosition = block.layout()->position();

    n.addRectangleNode(
                QRectF(blockPosition.x() + lastLine.naturalTextRect().x() + lastLine.naturalTextWidth(),
                        blockPosition.y() + m_overflowArea - m_contentY + lastLine.y(),
                        10,
                        lastLine.height()),
                color);
}
