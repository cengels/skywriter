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
    } else if (event->type() == QEvent::FocusOut) {
        m_blinking = false;
        m_caretTimer.stop();
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
    const QColor& fontColor = ThemeManager::instance()->activeTheme()->fontColor();
    const bool hasSelection = m_textCursor.hasSelection();

    QQuickTextNode* n = static_cast<QQuickTextNode*>(oldNode);
    if (!n)
        n = new QQuickTextNode(this);
    n->removeAllChildNodes();

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

        if (hasSelection) {
            const bool hasStartSelection = block.contains(m_textCursor.selectionStart());
            const bool hasEndSelection = block.contains(m_textCursor.selectionEnd());

            if (hasStartSelection) {
                selectionStart = m_textCursor.selectionStart() - block.position();
            } else if (hasEndSelection || m_textCursor.selectionStart() < block.position()) {
                selectionStart = 0;
            }

            if (hasEndSelection) {
                selectionEnd = m_textCursor.selectionEnd() - block.position() - 1;
            } else if (hasStartSelection || (selectionStart >= 0 && m_textCursor.selectionEnd() > block.position())) {
                selectionEnd = block.length() - 1;
            }
        }

        n->addTextLayout(QPointF(blockPosition.x(), blockPosition.y() + m_overflowArea - m_contentY),
                         block.layout(),
                         fontColor,
                         QQuickText::TextStyle::Normal,
                         QColor(),
                         QColor(),
                         QColor(fontColor),
                         QColor(fontColor.lightnessF() > 0.5 ? fontColor.darker(200) : fontColor.lighter(200)),
                         selectionStart,
                         selectionEnd);
    }

    if (this->hasFocus() && !m_blinking && !hasSelection) {
        n->setCursor(caretRectangle(), fontColor);
    }

    return n;
}
