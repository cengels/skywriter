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

#include "../TextFormatter.h"
#include "../TextHighlighter.h"
#include "../TextIterator.h"
#include "../MarkdownParser.h"
#include "../StringReplacer.h"
#include "../DocumentSegment.h"
#include "../../Range.h"

QT_BEGIN_NAMESPACE
class QTextDocument;
class QQuickTextDocument;
class DocumentSegment;
class TextFormatter;
class TextHighlighter;
QT_END_NAMESPACE

class FormattableTextArea : public QQuickItem
{
    Q_OBJECT

    //! The scroll position of the FormattableTextArea in absolute y pixels.
    Q_PROPERTY(double contentY READ contentY WRITE setContentY NOTIFY contentYChanged)

    Q_PROPERTY(int caretPosition READ caretPosition WRITE setCaretPosition NOTIFY caretPositionChanged)

    Q_PROPERTY(QString fileName READ fileName NOTIFY fileUrlChanged)
    Q_PROPERTY(QString fileType READ fileType NOTIFY fileUrlChanged)
    Q_PROPERTY(QUrl fileUrl READ fileUrl NOTIFY fileUrlChanged)
    Q_PROPERTY(QUrl directoryUrl READ directoryUrl NOTIFY directoryUrlChanged)
    Q_PROPERTY(bool fileExists READ fileExists NOTIFY fileExistsChanged)

    Q_PROPERTY(QTextDocument* document READ document NOTIFY documentChanged)
    Q_PROPERTY(const QVector<DocumentSegment*>& documentStructure READ documentStructure NOTIFY documentStructureChanged)
    Q_PROPERTY(DocumentSegment* currentDocumentSegment READ currentDocumentSegment NOTIFY currentDocumentSegmentChanged)

    Q_PROPERTY(bool canUndo READ canUndo NOTIFY canUndoChanged)
    Q_PROPERTY(bool canRedo READ canRedo NOTIFY canRedoChanged)
    Q_PROPERTY(bool canPaste READ canPaste NOTIFY canPasteChanged)
    Q_PROPERTY(QString selectedText READ selectedText NOTIFY selectedTextChanged)

    Q_PROPERTY(bool modified READ modified WRITE setModified NOTIFY modifiedChanged)
    Q_PROPERTY(QDateTime lastModified READ lastModified NOTIFY lastModifiedChanged)
    Q_PROPERTY(bool loading READ loading MEMBER m_loading NOTIFY loadingChanged)

    Q_PROPERTY(int characterCount READ characterCount NOTIFY characterCountChanged)
    Q_PROPERTY(int paragraphCount READ paragraphCount NOTIFY paragraphCountChanged)
    Q_PROPERTY(int wordCount READ wordCount NOTIFY wordCountChanged)
    Q_PROPERTY(int pageCount READ pageCount NOTIFY pageCountChanged)

    Q_PROPERTY(int selectedCharacterCount READ selectedCharacterCount NOTIFY selectedCharacterCountChanged)
    Q_PROPERTY(int selectedParagraphCount READ selectedParagraphCount NOTIFY selectedParagraphCountChanged)
    Q_PROPERTY(int selectedWordCount READ selectedWordCount NOTIFY selectedWordCountChanged)
    Q_PROPERTY(int selectedPageCount READ selectedPageCount NOTIFY selectedPageCountChanged)

    Q_PROPERTY(double contentWidth READ contentWidth NOTIFY contentWidthChanged)
    Q_PROPERTY(double contentHeight READ contentHeight NOTIFY contentHeightChanged)
    Q_PROPERTY(double overflowArea READ overflowArea WRITE setOverflowArea NOTIFY overflowAreaChanged)

    Q_PROPERTY(int searchResultCount READ searchResultCount NOTIFY searchResultCountChanged)

    //! If true, emphasized text segments (enclosed in asterisks or underscores)
    //! will be underlined rather than italicized.
    Q_PROPERTY(bool underline MEMBER m_underline NOTIFY underlineChanged)

    public:
        explicit FormattableTextArea(QQuickItem *parent = nullptr);

        enum class SelectionMode {
            NoSelection,
            WordSelection,
            BlockSelection
        };

        enum class SearchOption {
            None = 0x0,
            RegEx = 0x1,
            CaseSensitive = 0x2,
            InSelection = 0x4,
            WholeWords = 0x8
        };
        Q_ENUM(SearchOption)
        Q_DECLARE_FLAGS(SearchOptions, SearchOption)
        Q_FLAG(SearchOptions)

        QSGNode* updatePaintNode(QSGNode *oldNode, QQuickItem::UpdatePaintNodeData *updatePaintNodeData) override;
        bool event(QEvent* event) override;
        void keyPressEvent(QKeyEvent* event) override;
        void mousePressEvent(QMouseEvent* event) override;
        void mouseMoveEvent(QMouseEvent* event) override;
        void mouseReleaseEvent(QMouseEvent* event) override;

        QTextDocument* document() const;
        const QVector<DocumentSegment*>& documentStructure() const;
        DocumentSegment* currentDocumentSegment() const;
        DocumentSegment* findDocumentSegment(int position) const;

        double contentY() const;
        void setContentY(double contentY);

        double overflowArea() const;
        void setOverflowArea(double overflowArea);

        int caretPosition() const;
        void setCaretPosition(int caretPosition);

        void moveCursor(QTextCursor::MoveOperation op, QTextCursor::MoveMode mode = QTextCursor::MoveAnchor, int by = 1);
        void clearUndoStack();

        bool modified() const;
        QDateTime lastModified() const;
        bool canUndo() const;
        bool canRedo() const;
        bool canPaste() const;
        QString selectedText() const;

        const QVector<Range<int>>& searchResults() const;
        int searchResultCount() const;

        int characterCount() const;
        int paragraphCount() const;
        int wordCount() const;
        int pageCount() const;

        int selectedCharacterCount() const;
        int selectedParagraphCount() const;
        int selectedWordCount() const;
        int selectedPageCount() const;

        double contentWidth() const;
        double contentHeight() const;

        QString stylesheet() const;
        void setStyleSheet(const QString& stylesheet);

        QString fileName() const;
        QString fileType() const;
        QUrl fileUrl() const;
        QUrl directoryUrl() const;
        bool fileExists() const;

        bool loading() const;

        TextIterator wordIterator() const;
        TextIterator selectedWordIterator() const;

        void updateWordCount(bool isUserChange = true);

    public Q_SLOTS:
        //! Resets the text area by detaching the document and clearing the
        //! loaded file.
        void reset();
        void load(const QUrl &fileUrl);
        void saveAs(const QUrl &fileUrl, bool keepBackup = true);
        //! Backs up the current file.
        void backup();
        bool rename(const QUrl& newName);

        //! Copies the selected text.
        void copy();
        //! Pastes at the text cursor's position.
        void paste();
        //! Pastes at the text cursor's position without retaining formatting.
        void pasteUnformatted();
        //! Removes the selected text.
        void remove();
        //! Cuts the selected text.
        void cut();

        void undo();
        void redo();

        void toggleBold();
        void toggleItalics();
        void toggleStrikethrough();
        void applyHeading(int level);
        //! Inserts a scene break on the current line if the line is empty,
        //! or the next line otherwise.
        void insertSceneBreak();

        void clearReplacements();
        void setReplacement(const QString& target, const QString& replaceWith);
        void setReplacement(const QChar& target, const QChar& replaceWith);
        void setSmartReplacement(const QString& target, const QString& replaceWithStart, const QString& replaceWithEnd);
        void setSmartReplacement(const QChar& target, const QChar& replaceWithStart, const QChar& replaceWithEnd);

        void selectWord();
        void selectParagraph();
        void selectAll();

        //! Finds all occurrences of the given string with the specified search options.
        void find(const QString& searchString, const SearchOptions options = SearchOption::None);
        //! Jumps to the next occurrence of the string passed as part of the find() function.
        void jumpToNext();
        //! Jumps to the previous occurrence of the string passed as part of the find() function.
        void jumpToPrevious();
        void clearMatches();
        void replaceNext(const QString& text);
        void replaceAll(const QString& text);

        QRectF caretRectangle() const;

    Q_SIGNALS:
        void documentChanged();
        void documentStructureChanged();
        void currentDocumentSegmentChanged();

        void contentYChanged();
        void caretPositionChanged();

        void textChanged(int position, int added, int removed);
        void fileUrlChanged();
        void directoryUrlChanged();
        void fileExistsChanged();

        void canUndoChanged(bool);
        void canRedoChanged(bool);
        void canPasteChanged();
        void selectedTextChanged();

        void loaded();

        void modifiedChanged();
        void lastModifiedChanged();
        void loadingChanged();

        void searchResultCountChanged();

        void characterCountChanged();
        void paragraphCountChanged();
        void wordCountChanged(bool isProgress);
        void pageCountChanged();

        void selectedCharacterCountChanged();
        void selectedParagraphCountChanged();
        void selectedWordCountChanged();
        void selectedPageCountChanged();

        void contextMenuRequested();

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

        QTextDocument* newDocument();
        void connectDocument();
        QTextDocument* m_document;
        QVector<DocumentSegment*> m_documentStructure;
        DocumentSegment* m_currentDocumentSegment;
        TextFormatter* m_formatter;
        TextHighlighter* m_highlighter;
        StringReplacer m_replacer;

        QTextCursor m_textCursor;
        double m_contentY;
        double m_overflowArea;

        QUrl m_fileUrl;
        bool m_loading;
        bool m_isUndoRedo;

        void setModified(bool modified);
        void setFileUrl(const QUrl& url);

        void handleTextChange(const int position, const int added, const int removed);
        //! Calls update() and sets appropriate variables that indicate the
        //! user is actively editing the document instead of just being idle.
        //! This includes ensuring the caret blinking timer is reset.
        void updateActive();
        void expandSelection();

        void updateDocumentStructure(const int position, const int added, const int removed);
        void updateFindRanges();
        void refreshDocumentStructure();

        int m_characterCount;
        int m_selectedCharacterCount;
        int m_wordCount;
        int m_selectedWordCount;
        int m_paragraphCount;
        int m_selectedParagraphCount;
        int m_pageCount;
        int m_selectedPageCount;
        void updateCounts();
        void updateSelectedCounts();
        void updateCharacterCount();
        void updateSelectedWordCount();
        void updateParagraphCount();
        void updatePageCount();

        void addHorizontalRule(QQuickTextNode& n, const QTextBlock& block, const qreal width, const QColor& color);
        void addWrappedSelectionIndicator(QQuickTextNode& n, const QTextBlock& block, const QColor& color);

        QString m_searchString;
        SearchOptions m_searchFlags;

        bool m_underline;

        QTimer m_caretTimer;
        bool m_blinking;
        std::atomic<int> m_activeWordCounters;

        QMouseEvent m_lastMouseUpEvent;
        QMouseEvent m_lastMouseDownEvent;
        int m_lastCaretPosition;
        SelectionMode m_selectionMode;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(FormattableTextArea::SearchOptions)

#endif // FORMATTABLETEXTAREA_H
