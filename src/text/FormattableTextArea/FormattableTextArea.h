#ifndef FORMATTABLETEXTAREA_H
#define FORMATTABLETEXTAREA_H

#include <QFont>
#include <QObject>
#include <QTextCursor>
#include <QUrl>
#include <QDateTime>
#include <QQuickItem>
#include <QTextLayout>
#include <QTextDocument>
#include <QTimer>

#include "../TextHighlighter.h"
#include "../TextIterator.h"
#include "../MarkdownParser.h"

QT_BEGIN_NAMESPACE
class QTextDocument;
class QQuickTextDocument;
QT_END_NAMESPACE

class FormattableTextArea : public QQuickItem
{
    Q_OBJECT

    //! The scroll position of the FormattableTextArea in absolute y pixels.
    Q_PROPERTY(double contentY WRITE setContentY MEMBER m_contentY NOTIFY contentYChanged)

    Q_PROPERTY(int caretPosition READ caretPosition WRITE setCaretPosition NOTIFY caretPositionChanged)

    Q_PROPERTY(QString fileName READ fileName NOTIFY fileUrlChanged)
    Q_PROPERTY(QString fileType READ fileType NOTIFY fileUrlChanged)
    Q_PROPERTY(QUrl fileUrl READ fileUrl NOTIFY fileUrlChanged)
    Q_PROPERTY(QUrl directoryUrl READ directoryUrl NOTIFY directoryUrlChanged)

    Q_PROPERTY(bool modified READ modified WRITE setModified NOTIFY modifiedChanged)
    Q_PROPERTY(QDateTime lastModified READ lastModified NOTIFY lastModifiedChanged)
    Q_PROPERTY(bool loading READ loading MEMBER m_loading NOTIFY loadingChanged)

    Q_PROPERTY(int characterCount READ characterCount NOTIFY characterCountChanged)
    Q_PROPERTY(int paragraphCount READ paragraphCount NOTIFY paragraphCountChanged)
    Q_PROPERTY(int wordCount READ wordCount NOTIFY wordCountChanged)
    Q_PROPERTY(int pageCount READ pageCount NOTIFY pageCountChanged)

    Q_PROPERTY(double contentWidth READ contentWidth NOTIFY contentWidthChanged)
    Q_PROPERTY(double contentHeight READ contentHeight NOTIFY contentHeightChanged)
    Q_PROPERTY(double overflowArea MEMBER m_overflowArea NOTIFY overflowAreaChanged)

    Q_PROPERTY(double firstLineIndent MEMBER m_firstLineIndent NOTIFY firstLineIndentChanged)

    //! If true, emphasized text segments (enclosed in asterisks or underscores)
    //! will be underlined rather than italicized.
    Q_PROPERTY(bool underline MEMBER m_underline NOTIFY underlineChanged);

    public:
        explicit FormattableTextArea(QQuickItem *parent = nullptr);

        QSGNode* updatePaintNode(QSGNode *oldNode, QQuickItem::UpdatePaintNodeData *updatePaintNodeData) override;
        bool event(QEvent* event) override;
        void keyPressEvent(QKeyEvent* event) override;
        void mousePressEvent(QMouseEvent* event) override;
        void mouseMoveEvent(QMouseEvent* event) override;
        void mouseReleaseEvent(QMouseEvent* event) override;

        void setContentY(double contentY);

        int caretPosition() const;
        void setCaretPosition(int caretPosition);

        void moveCursor(QTextCursor::MoveOperation op, QTextCursor::MoveMode mode = QTextCursor::MoveAnchor, int by = 1);
        void clearUndoStack();

        bool modified() const;
        QDateTime lastModified() const;

        int characterCount() const;
        int paragraphCount() const;
        int wordCount() const;
        int pageCount() const;

        double contentWidth() const;
        double contentHeight() const;

        QString stylesheet() const;
        void setStyleSheet(const QString& stylesheet);

        QString fileName() const;
        QString fileType() const;
        QUrl fileUrl() const;
        QUrl directoryUrl() const;
        bool loading() const;

        TextIterator wordIterator() const;

    public Q_SLOTS:
        void load(const QUrl &fileUrl);
        void saveAs(const QUrl &fileUrl);

        void copy();
        void paste();

        void undo();
        void redo();

        void toggleBold();
        void toggleItalics();
        void toggleStrikethrough();

        void selectWord();
        void selectParagraph();

        QRectF caretRectangle() const;

    Q_SIGNALS:
        void documentChanged();

        void contentYChanged();
        void caretPositionChanged();

        void textChanged();
        void fileUrlChanged();
        void directoryUrlChanged();

        void loaded();

        void modifiedChanged();
        void lastModifiedChanged();
        void loadingChanged();

        void characterCountChanged();
        void paragraphCountChanged();
        void wordCountChanged(bool isProgress);
        void pageCountChanged();

        void contentWidthChanged();
        void contentHeightChanged();
        void overflowAreaChanged();

        void firstLineIndentChanged();
        void underlineChanged();

    private:
        void updateStyling();

        const QTextCharFormat getSelectionFormat() const;
        void mergeFormat(const QTextCharFormat &format);

        int hitTest(const QPointF& point) const;
        void newDocument(QTextDocument* document = new QTextDocument());
        QTextDocument *m_document;
        TextHighlighter* m_highlighter;

        QTextCursor m_textCursor;
        double m_contentY;
        double m_overflowArea;

        QUrl m_fileUrl;
        bool m_loading;
        bool m_counting;

        void setModified(bool modified);
        void setFileUrl(const QUrl& url);

        void handleTextChange();
        //! Calls update() and sets appropriate variables that indicate the
        //! user is actively editing the document instead of just being idle.
        //! This includes ensuring the caret blinking timer is reset.
        void updateActive();

        int m_characterCount;
        int m_wordCount;
        int m_paragraphCount;
        int m_pageCount;
        void updateCounts();
        void updateCharacterCount();
        void updateWordCount();
        void updateParagraphCount();
        void updatePageCount();

        int m_firstLineIndent;
        bool m_underline;
        QTimer m_caretTimer;
        bool m_blinking;
};

#endif // FORMATTABLETEXTAREA_H
