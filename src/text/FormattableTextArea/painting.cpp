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
    } else if (event->type() == QEvent::KeyPress) {
        const QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
        const QTextCursor::MoveMode moveMode = keyEvent->modifiers().testFlag(Qt::KeyboardModifier::ShiftModifier)
                                  ? QTextCursor::KeepAnchor
                                  : QTextCursor::MoveAnchor;

        switch (keyEvent->key()) {
            case Qt::Key_Right:
                moveCursor(QTextCursor::Right, moveMode);
                break;
            case Qt::Key_Left:
                moveCursor(QTextCursor::Left, moveMode);
                break;
            case Qt::Key_Up:
                moveCursor(QTextCursor::Up, moveMode);
                break;
            case Qt::Key_Down:
                moveCursor(QTextCursor::Down, moveMode);
                break;
            case Qt::Key_Back:
            case Qt::Key_Backspace:
                m_textCursor.deletePreviousChar();
                update();
                break;
            case Qt::Key_Delete:
                m_textCursor.deleteChar();
                update();
                break;
            default:
                const QString text = keyEvent->text();

                if (!text.isEmpty()) {
                    m_textCursor.insertText(text);
                    m_textCursor.movePosition(QTextCursor::NextCharacter, QTextCursor::MoveAnchor, text.length() - 1);
                    update();
                }
                break;
        }
    }

    return QQuickItem::event(event);
}

void FormattableTextArea::updateStyling()
{

}

QSGNode* FormattableTextArea::updatePaintNode(QSGNode *oldNode, QQuickItem::UpdatePaintNodeData *updatePaintNodeData)
{
    Q_UNUSED(updatePaintNodeData)
    QFontMetrics fontMetrics(ThemeManager::instance()->activeTheme()->font());
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
        const bool hasStartSelection = hasSelection && block.contains(m_textCursor.selectionStart());
        const bool hasEndSelection = hasSelection && block.contains(m_textCursor.selectionEnd());

        if (hasStartSelection) {
            selectionStart = m_textCursor.selectionStart() - block.position();
        } else if (hasEndSelection || m_textCursor.selectionStart() < block.position()) {
            selectionStart = 0;
        }

        if (hasEndSelection) {
            selectionEnd = m_textCursor.selectionEnd() - block.position() - 1;
        } else if (hasStartSelection || m_textCursor.selectionEnd() > block.position()) {
            selectionEnd = block.length() - 1;
        }

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
