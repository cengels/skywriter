#ifndef HEADINGFORMAT_H
#define HEADINGFORMAT_H

#include <QTextBlockFormat>
#include <QTextCharFormat>
#include <QJsonObject>
#include <QJsonArray>
#include <QMetaEnum>

class HeadingFormat
{
    public:
        HeadingFormat(int headingLevel);

        enum TextDecorations {
            None = 0,
            Bold = 1,
            Italic = 2,
            Underline = 4
        };

        const QTextBlockFormat& blockFormat() const;
        const QTextCharFormat& charFormat() const;

        int headingLevel() const;

        Qt::Alignment alignment() const;
        void setAlignment(Qt::Alignment alignment);

        TextDecorations textDecorations() const;
        void setTextDecorations(TextDecorations decorations);

        double indent() const;
        void setIndent(double indent);

        double spaceBefore() const;
        void setSpaceBefore(double spaceBefore);

        double spaceAfter() const;
        void setSpaceAfter(double spaceAfter);

        double fontSize() const;
        void setFontSize(double fontSize);

        //! Sets the base font size for this HeadingFormat. The base font size
        //! will generally be the font size of the surrounding text (i.e. a
        //! regular paragraph). This function will set the font size to a
        //! product equal to (baseFontSize + (baseFontSize * (1 / heading level)).
        void setBaseFontSize(double base);

        QFont::Capitalization capitalization() const;
        void setCapitalization(QFont::Capitalization capitalization);

        double letterSpacing() const;
        void setLetterSpacing(double letterSpacing);

        void read(const QJsonObject& json);
        void write(QJsonArray& json) const;

        bool operator==(const HeadingFormat& other) const;
        bool operator!=(const HeadingFormat& other) const;

    private:
        QTextBlockFormat m_blockFormat;
        QTextCharFormat m_charFormat;
};

#endif // HEADINGFORMAT_H
