#ifndef FORMATTABLETEXTAREA_H
#define FORMATTABLETEXTAREA_H

#include <QFont>
#include <QObject>
#include <QTextCursor>
#include <QUrl>
#include <QDateTime>

QT_BEGIN_NAMESPACE
class QTextDocument;
class QQuickTextDocument;
QT_END_NAMESPACE

class FormattableTextArea : public QObject {
    Q_OBJECT

    Q_PROPERTY(QQuickTextDocument *document READ document WRITE setDocument NOTIFY documentChanged)
    Q_PROPERTY(int cursorPosition READ cursorPosition WRITE setCursorPosition NOTIFY cursorPositionChanged)
    Q_PROPERTY(int selectionStart READ selectionStart WRITE setSelectionStart NOTIFY selectionStartChanged)
    Q_PROPERTY(int selectionEnd READ selectionEnd WRITE setSelectionEnd NOTIFY selectionEndChanged)

    Q_PROPERTY(QString fileName READ fileName NOTIFY fileUrlChanged)
    Q_PROPERTY(QString fileType READ fileType NOTIFY fileUrlChanged)
    Q_PROPERTY(QUrl fileUrl READ fileUrl NOTIFY fileUrlChanged)

    Q_PROPERTY(bool modified READ modified WRITE setModified NOTIFY modifiedChanged)
    Q_PROPERTY(QDateTime lastModified READ lastModified NOTIFY lastModifiedChanged)

    Q_PROPERTY(int characterCount READ characterCount NOTIFY characterCountChanged)
    Q_PROPERTY(int paragraphCount READ paragraphCount NOTIFY paragraphCountChanged)
    Q_PROPERTY(int wordCount READ wordCount NOTIFY wordCountChanged)
    Q_PROPERTY(int pageCount READ pageCount NOTIFY pageCountChanged)

    public:
        explicit FormattableTextArea(QObject *parent = nullptr);

        QQuickTextDocument *document() const;
        void setDocument(QQuickTextDocument *document);

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

        QString stylesheet() const;
        void setStyleSheet(const QString& stylesheet);

        QString fileName() const;
        QString fileType() const;
        QUrl fileUrl() const;

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

        void loaded(const QString &text);
        void error(const QString &message);

        void modifiedChanged();
        void lastModifiedChanged();

        void characterCountChanged();
        void paragraphCountChanged();
        void wordCountChanged();
        void pageCountChanged();

    private:
        void reset();

        QTextCursor textCursor() const;
        QTextDocument *textDocument() const;
        const QTextCharFormat getSelectionFormat() const;
        void mergeFormat(const QTextCharFormat &format);

        QQuickTextDocument *m_document;

        int m_cursorPosition;
        int m_selectionStart;
        int m_selectionEnd;
        QUrl m_fileUrl;

        void setModified(bool modified);

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
};

#endif // FORMATTABLETEXTAREA_H