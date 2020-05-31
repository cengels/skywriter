#ifndef THEME_H
#define THEME_H

#include <QObject>
#include <QUrl>
#include <QColor>
#include <QFont>

#include "HeadingFormat.h"

struct Theme : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name MEMBER m_name NOTIFY nameChanged);
    Q_PROPERTY(bool isReadOnly MEMBER m_isReadOnly NOTIFY isReadOnlyChanged);
    Q_PROPERTY(double fontSize READ fontSize WRITE setFontSize NOTIFY fontSizeChanged);
    Q_PROPERTY(QString fontFamily READ fontFamily WRITE setFontFamily NOTIFY fontFamilyChanged);
    Q_PROPERTY(QFont font READ font NOTIFY fontChanged);
    Q_PROPERTY(QColor fontColor READ fontColor MEMBER m_fontColor NOTIFY fontColorChanged);
    Q_PROPERTY(QString backgroundImage MEMBER m_backgroundImage NOTIFY backgroundImageChanged);
    Q_PROPERTY(FillMode fillMode MEMBER m_fillMode NOTIFY fillModeChanged);
    Q_PROPERTY(double documentWidth MEMBER m_documentWidth NOTIFY documentWidthChanged);
    Q_PROPERTY(double documentHeight MEMBER m_documentHeight NOTIFY documentHeightChanged);
    Q_PROPERTY(int paddingVertical MEMBER m_paddingVertical NOTIFY paddingVerticalChanged);
    Q_PROPERTY(int paddingHorizontal MEMBER m_paddingHorizontal NOTIFY paddingHorizontalChanged);
    Q_PROPERTY(double firstLineIndent READ firstLineIndent MEMBER m_firstLineIndent NOTIFY firstLineIndentChanged);
    Q_PROPERTY(double lineHeight READ lineHeight MEMBER m_lineHeight NOTIFY lineHeightChanged);
    Q_PROPERTY(double paragraphSpacing READ paragraphSpacing MEMBER m_paragraphSpacing NOTIFY paragraphSpacingChanged);
    Q_PROPERTY(QColor windowBackground MEMBER m_windowBackground NOTIFY windowBackgroundChanged);
    Q_PROPERTY(QColor documentBackground MEMBER m_documentBackground NOTIFY documentBackgroundChanged);
    Q_PROPERTY(HAlignment textAlignment READ textAlignment MEMBER m_textAlignment NOTIFY textAlignmentChanged);
    Q_PROPERTY(QColor uiBackground MEMBER m_uiBackground NOTIFY uiBackgroundChanged);
    Q_PROPERTY(QColor uiColor MEMBER m_uiColor NOTIFY uiColorChanged);
    Q_PROPERTY(HeadingFormat formatH1 MEMBER m_formatH1 NOTIFY formatH1Changed);
    Q_PROPERTY(HeadingFormat formatH2 MEMBER m_formatH2 NOTIFY formatH2Changed);
    Q_PROPERTY(HeadingFormat formatH3 MEMBER m_formatH3 NOTIFY formatH3Changed);
    Q_PROPERTY(HeadingFormat formatH4 MEMBER m_formatH4 NOTIFY formatH4Changed);
    Q_PROPERTY(HeadingFormat formatH5 MEMBER m_formatH5 NOTIFY formatH5Changed);
    Q_PROPERTY(HeadingFormat formatH6 MEMBER m_formatH6 NOTIFY formatH6Changed);

    public:
        explicit Theme(QObject *parent = nullptr);
        explicit Theme(const Theme& copy, QObject *parent = nullptr);

        enum FillMode { Stretch, PreserveAspectFit, PreserveAspectCrop, Tile, TileVertically, TileHorizontally, Pad };
        Q_ENUM(FillMode);

        enum HAlignment { AlignLeft = Qt::AlignLeft,
                          AlignRight = Qt::AlignRight,
                          AlignHCenter = Qt::AlignHCenter,
                          AlignJustify = Qt::AlignJustify };
        Q_ENUM(HAlignment)

        //! Returns the default light theme.
        static Theme* defaultLight();
        //! Returns the default dark theme.
        static Theme* defaultDark();
        //! Returns the default application theme.
        static Theme* defaultSky();

        const QString& name() const;

        const QFont font() const;
        const QColor fontColor() const;
        HAlignment textAlignment() const;

        QString fontFamily() const;
        void setFontFamily(const QString& fontFamily);

        double fontSize() const;
        void setFontSize(double size);

        double firstLineIndent() const;
        double lineHeight() const;
        double paragraphSpacing() const;

        const HeadingFormat& headingFormat(int headingLevel) const;

        void read(const QJsonObject& json);
        void write(QJsonArray& json) const;

    Q_SIGNALS:
        void nameChanged();
        void isReadOnlyChanged();
        void fontSizeChanged();
        void fontFamilyChanged();
        void fontChanged();
        void backgroundImageChanged();
        void fillModeChanged();
        void documentWidthChanged();
        void documentHeightChanged();
        void paddingVerticalChanged();
        void paddingHorizontalChanged();
        void firstLineIndentChanged();
        void lineHeightChanged();
        void paragraphSpacingChanged();
        void fontColorChanged();
        void windowBackgroundChanged();
        void documentBackgroundChanged();
        void textAlignmentChanged();
        void uiBackgroundChanged();
        void uiColorChanged();
        void formatH1Changed();
        void formatH2Changed();
        void formatH3Changed();
        void formatH4Changed();
        void formatH5Changed();
        void formatH6Changed();

    private:
        QString m_name;
        bool m_isReadOnly;
        QFont m_font;
        QString m_backgroundImage;
        FillMode m_fillMode;
        double m_documentWidth;
        double m_documentHeight;
        int m_paddingVertical;
        int m_paddingHorizontal;
        double m_firstLineIndent;
        double m_lineHeight;
        double m_paragraphSpacing;
        QColor m_fontColor;
        QColor m_windowBackground;
        QColor m_documentBackground;
        HAlignment m_textAlignment;
        QColor m_uiBackground;
        QColor m_uiColor;
        HeadingFormat m_formatH1;
        HeadingFormat m_formatH2;
        HeadingFormat m_formatH3;
        HeadingFormat m_formatH4;
        HeadingFormat m_formatH5;
        HeadingFormat m_formatH6;
};

#endif // THEME_H
