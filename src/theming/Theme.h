#ifndef THEME_H
#define THEME_H

#include <QObject>
#include <QUrl>
#include <QColor>
#include <QFont>

struct Theme : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name MEMBER m_name NOTIFY nameChanged);
    Q_PROPERTY(bool isReadOnly MEMBER m_isReadOnly NOTIFY isReadOnlyChanged);
    Q_PROPERTY(double fontSize READ fontSize WRITE setFontSize NOTIFY fontSizeChanged);
    Q_PROPERTY(QString fontFamily READ fontFamily WRITE setFontFamily NOTIFY fontFamilyChanged);
    Q_PROPERTY(QFont font READ font NOTIFY fontChanged);
    Q_PROPERTY(QColor fontColor MEMBER m_fontColor NOTIFY fontColorChanged);
    Q_PROPERTY(QString backgroundImage MEMBER m_backgroundImage NOTIFY backgroundImageChanged);
    Q_PROPERTY(FillMode fillMode MEMBER m_fillMode NOTIFY fillModeChanged);
    Q_PROPERTY(double documentWidth MEMBER m_documentWidth NOTIFY documentWidthChanged);
    Q_PROPERTY(double documentHeight MEMBER m_documentHeight NOTIFY documentHeightChanged);
    Q_PROPERTY(int paddingVertical MEMBER m_paddingVertical NOTIFY paddingVerticalChanged);
    Q_PROPERTY(int paddingHorizontal MEMBER m_paddingHorizontal NOTIFY paddingHorizontalChanged);
    Q_PROPERTY(double firstLineIndent MEMBER m_firstLineIndent NOTIFY firstLineIndentChanged);
    Q_PROPERTY(double lineHeight MEMBER m_lineHeight NOTIFY lineHeightChanged);
    Q_PROPERTY(double paragraphSpacing MEMBER m_paragraphSpacing NOTIFY paragraphSpacingChanged);
    Q_PROPERTY(QColor windowBackground MEMBER m_windowBackground NOTIFY windowBackgroundChanged);
    Q_PROPERTY(QColor documentBackground MEMBER m_documentBackground NOTIFY documentBackgroundChanged);
    Q_PROPERTY(HAlignment textAlignment MEMBER m_textAlignment NOTIFY textAlignmentChanged);

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

        const QFont font() const;

        QString fontFamily() const;
        void setFontFamily(const QString& fontFamily);

        double fontSize() const;
        void setFontSize(double size);

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

};

#endif // THEME_H
