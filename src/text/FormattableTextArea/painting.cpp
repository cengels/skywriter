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
    format::normalize(cursor, theme, m_sceneBreak);

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

        if (!block.text().isEmpty()) {
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
            const QTextLine lastLine = block.layout()->lineAt(block.layout()->lineCount() - 1);
            n->addRectangleNode(
                        QRectF(QPointF(blockPosition.x() + lastLine.naturalTextRect().x() + lastLine.naturalTextWidth(),
                                       blockPosition.y() + m_overflowArea - m_contentY + lastLine.y()),
                               QSizeF(10, lastLine.height())),
                        fontColor);
        }
    }

    if (this->hasFocus() && (hasSelection || !m_blinking)) {
        // Should be slanted if in italics mode but doesn't seem to be possible
        // with QQuickTextNode.
        n->setCursor(caretRectangle(), fontColor);
    }

    return n;
}
