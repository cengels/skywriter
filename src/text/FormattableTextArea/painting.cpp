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

bool FormattableTextArea::event(QEvent* event)
{
    if (event->type() == QEvent::MouseButtonPress) {
        forceActiveFocus();
    }

    return QQuickItem::event(event);
}

void FormattableTextArea::updateStyling()
{
    if (!m_document) {
        return;
    }

    const Theme* theme = ThemeManager::instance()->activeTheme();
    m_document->setDefaultFont(theme->font());
    QTextOption textOption = m_document->defaultTextOption();
    textOption.setWrapMode(QTextOption::WordWrap);
    textOption.setAlignment(static_cast<Qt::Alignment>(theme->textAlignment()));
    m_document->setDefaultTextOption(textOption);
    m_document->setTextWidth(this->width());
}

QSGNode* FormattableTextArea::updatePaintNode(QSGNode *oldNode, QQuickItem::UpdatePaintNodeData *updatePaintNodeData)
{
    Q_UNUSED(updatePaintNodeData)
    const QFont& font = ThemeManager::instance()->activeTheme()->font();
    const QColor& fontColor = ThemeManager::instance()->activeTheme()->fontColor();
    const bool hasSelection = m_textCursor.hasSelection();
//    int height = 0;
//    int lineWidth = 40;
//    int leading = fontMetrics.leading();

    QQuickTextNode* n = static_cast<QQuickTextNode*>(oldNode);
    if (!n)
        n = new QQuickTextNode(this);
    n->removeAllChildNodes();

    const QTextBlock& end = this->m_document->end();
    for (QTextBlock block = this->m_document->begin(); block != end; block = block.next())
    {
        const QPointF& blockPosition = block.layout()->position();
        const double blockHeight = block.layout()->boundingRect().height();

        if (blockPosition.y() + blockHeight < this->m_position) {
            continue;
        } else if (blockPosition.y() > this->m_position + this->height()) {
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

//        block.layout()->setFont(font);
//        QTextOption textOption = block.layout()->textOption();
//        textOption.setAlignment(static_cast<Qt::Alignment>(ThemeManager::instance()->activeTheme()->textAlignment()));
//        block.layout()->setTextOption(textOption);

        n->addTextLayout(blockPosition,
                         block.layout(),
                         fontColor,
                         QQuickText::TextStyle::Normal,
                         QColor(),
                         QColor(),
                         QColor(fontColor),
                         QColor(fontColor.lightnessF() > 0.5 ? fontColor.darker(200) : fontColor.lighter(200)),
                         selectionStart,
                         selectionEnd);

        if (block == m_textCursor.block()) {
            const QTextLine& line = block.layout()->lineForTextPosition(m_textCursor.positionInBlock());
            const qreal x = line.cursorToX(m_textCursor.positionInBlock());
            n->setCursor(QRectF(x + 3, line.y() + blockPosition.y(), 1, line.height()), fontColor);
        }
    }

    return n;
}
