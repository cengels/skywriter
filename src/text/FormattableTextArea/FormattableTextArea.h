#ifndef FORMATTABLETEXTAREA_H
#define FORMATTABLETEXTAREA_H

#include <QFont>
#include <QObject>
#include <QTextCursor>
#include <QUrl>
#include <QDateTime>
#include <QQuickPaintedItem>
#include <QTextLayout>
#include <QTextDocument>
#include <QTextEdit>

#include "../TextHighlighter.h"
#include "../TextIterator.h"

QT_BEGIN_NAMESPACE
class QTextDocument;
class QQuickTextDocument;
QT_END_NAMESPACE

class FormattableTextArea : public QQuickPaintedItem
{
    Q_OBJECT

    Q_PROPERTY(double position WRITE setPosition MEMBER m_position NOTIFY positionChanged)

    Q_PROPERTY(int cursorPosition READ cursorPosition WRITE setCursorPosition NOTIFY cursorPositionChanged)
    Q_PROPERTY(int selectionStart READ selectionStart WRITE setSelectionStart NOTIFY selectionStartChanged)
    Q_PROPERTY(int selectionEnd READ selectionEnd WRITE setSelectionEnd NOTIFY selectionEndChanged)

    Q_PROPERTY(QString fileName READ fileName NOTIFY fileUrlChanged)
    Q_PROPERTY(QString fileType READ fileType NOTIFY fileUrlChanged)
    Q_PROPERTY(QUrl fileUrl READ fileUrl NOTIFY fileUrlChanged)
    Q_PROPERTY(QUrl directoryUrl READ directoryUrl NOTIFY directoryUrlChanged)

    Q_PROPERTY(bool modified READ modified WRITE setModified NOTIFY modifiedChanged)
    Q_PROPERTY(QDateTime lastModified READ lastModified NOTIFY lastModifiedChanged)

    Q_PROPERTY(int characterCount READ characterCount NOTIFY characterCountChanged)
    Q_PROPERTY(int paragraphCount READ paragraphCount NOTIFY paragraphCountChanged)
    Q_PROPERTY(int wordCount READ wordCount NOTIFY wordCountChanged)
    Q_PROPERTY(int pageCount READ pageCount NOTIFY pageCountChanged)

    Q_PROPERTY(double contentWidth READ contentWidth NOTIFY contentWidthChanged)
    Q_PROPERTY(double contentHeight READ contentHeight NOTIFY contentHeightChanged)

    Q_PROPERTY(double firstLineIndent MEMBER m_firstLineIndent NOTIFY firstLineIndentChanged)

    public:
        explicit FormattableTextArea(QQuickItem *parent = nullptr);
        ~FormattableTextArea();

        void paint(QPainter *painter) override;
        bool eventFilter(QObject* object, QEvent* event) override;
        bool event(QEvent* event) override;

        void setPosition(double position);

        int cursorPosition() const;
        void setCursorPosition(int position);

        int selectionStart() const;
        void setSelectionStart(int position);

        int selectionEnd() const;
        void setSelectionEnd(int position);

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

        TextIterator wordIterator() const;

    public Q_SLOTS:
        void load(const QUrl &fileUrl);
        void saveAs(const QUrl &fileUrl);

        void toggleBold();
        void toggleItalics();
        void toggleStrikethrough();

    Q_SIGNALS:
        void documentChanged();
        void cursorPositionChanged();
        void selectionStartChanged();
        void selectionEndChanged();

        void textChanged();
        void fileUrlChanged();
        void directoryUrlChanged();

        void loaded();
        void error(const QString &message);

        void modifiedChanged();
        void lastModifiedChanged();

        void positionChanged();

        void characterCountChanged();
        void paragraphCountChanged();
        void wordCountChanged();
        void pageCountChanged();

        void contentWidthChanged();
        void contentHeightChanged();

        void firstLineIndentChanged();

    private:
        void updateStyling();

        QTextCursor textCursor() const;
        const QTextCharFormat getSelectionFormat() const;
        void mergeFormat(const QTextCharFormat &format);

        void newDocument(QTextDocument* document = new QTextDocument());
        QTextDocument *m_document;
        QTextLayout *m_textLayout;
        TextHighlighter* m_highlighter;

        double m_position;

        int m_cursorPosition;
        int m_selectionStart;
        int m_selectionEnd;
        QUrl m_fileUrl;

        void setModified(bool modified);
        void setFileUrl(const QUrl& url);

        void handleTextChange();

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

        QTextEdit* m_textEdit;
};

#endif // FORMATTABLETEXTAREA_H
