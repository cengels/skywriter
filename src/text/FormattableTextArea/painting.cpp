//////////////////////////////////////////////////////////////////////////////
///                                                                        ///
///        Contains all code responsible for drawing the text area.        ///
///                                                                        ///
//////////////////////////////////////////////////////////////////////////////

#include <QFontMetrics>
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

        switch (keyEvent->key()) {
                qDebug() << "yes";
                this->setSelectionStart(m_selectionStart + 1);
                this->setSelectionEnd(m_selectionStart);
                break;
            case Qt::Key_Left:
                this->setSelectionStart(m_selectionStart - 1);
                this->setSelectionEnd(m_selectionStart);
                break;
            default:
                const QString text = keyEvent->text();

                if (!text.isEmpty()) {
                    m_textCursor.insertText(text);
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
//    int height = 0;
//    int lineWidth = 40;
//    int leading = fontMetrics.leading();

    QQuickTextNode* n = static_cast<QQuickTextNode*>(oldNode);
    if (!n)
        n = new QQuickTextNode(this);
    n->removeAllChildNodes();

    const auto end = this->m_document->end();
    for (QTextBlock block = this->m_document->begin(); block != end; block = block.next()) {

        const QPointF& blockPosition = block.layout()->position();
        const double blockHeight = block.layout()->boundingRect().height();

        if (blockPosition.y() + blockHeight < this->m_position) {
            continue;
        } else if (blockPosition.y() > this->m_position + this->height()) {
            break;
        }

        n->addTextLayout(blockPosition,
                         block.layout(),
                         ThemeManager::instance()->activeTheme()->fontColor(),
                         QQuickText::TextStyle::Normal,
                         QColor(),
                         QColor(),
                         QColor(),
                         QColor(),
                         -1,
                         -1);
    }

//    m_layout->beginLayout();
//    int counter = 1;
//    while (1) {
//        QTextLine line = m_layout->createLine();
//        if (!line.isValid())
//            break;
//        line.setLineWidth(counter*lineWidth);
//        height += leading;
//        line.setPosition(QPointF(0, height));
//        height += line.height();
//        counter++;
//    }
//    m_layout->endLayout();
//    n->setCursor(QRectF(0, 0, 20, 1), QColor("red"));
    return n;
}
