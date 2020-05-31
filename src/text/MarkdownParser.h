#ifndef MARKDOWNPARSER_H
#define MARKDOWNPARSER_H

#include <QObject>
#include <QTextDocument>
#include <QTextCursor>
#include <QStack>
#include <md4c/src/md4c.h>

#include "../theming/ThemeManager.h"

class MarkdownParser
{
    public:
        //! Constructs a new MarkdownParser with the specified document
        //! and creates a new cursor that will iterate through the document
        //! from beginning to end.
        MarkdownParser(QTextDocument* document);
        //! Constructs a new MarkdownParser from the specified QTextCursor
        //! and iterates through the document from the cursor's current
        //! position.
        MarkdownParser(QTextCursor* cursor);

        //! Parses the specified string as markdown and inserts it into the
        //! document at the cursor's position.
        void parse(const QString& string);
        //! Turns the document into markdown, starting from the cursor's
        //! position.
        QString stringify() const;

    private:
        enum ParserFlags {
            None = 0,
            NoNewBlockNeeded = 1
        };

        static int onEnterBlock(MD_BLOCKTYPE type, void *detail, void *context);
        static int onLeaveBlock(MD_BLOCKTYPE type, void *detail, void *context);
        static int onEnterSpan(MD_SPANTYPE type, void *detail, void *context);
        static int onLeaveSpan(MD_SPANTYPE type, void *detail, void *context);
        static int onText(MD_TEXTTYPE type, const MD_CHAR *text, MD_SIZE size, void *context);
        int onEnterBlock(MD_BLOCKTYPE type, void *detail);
        int onLeaveBlock(MD_BLOCKTYPE type, void *detail);
        int onEnterSpan(MD_SPANTYPE type, void *detail);
        int onLeaveSpan(MD_SPANTYPE type, void *detail);
        int onText(MD_TEXTTYPE type, const MD_CHAR *text, MD_SIZE size);

        static const QTextCharFormat CHAR_FORMAT_REGULAR;
        static const QTextCharFormat CHAR_FORMAT_EMPHASIS;
        static const QTextCharFormat CHAR_FORMAT_DOUBLE_EMPHASIS;
        static const QTextCharFormat CHAR_FORMAT_STRIKETHROUGH;

        void startFormat(const QTextCharFormat& format);
        void endFormat();

        MD_PARSER m_parse_info;
        QTextDocument* m_document;
        QTextCursor* m_textCursor;
        QStack<QTextCharFormat> m_formatStack;
        int m_flags;
};

#endif // MARKDOWNPARSER_H
