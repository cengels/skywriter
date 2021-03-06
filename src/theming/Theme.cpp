#include <QColor>
#include <QPalette>
#include <QJsonObject>
#include <QJsonArray>
#include <QVariant>
#include <QMetaEnum>
#include <QDebug>
#include "Theme.h"
#include "../colors.h"
#include "defaults.h"

namespace {
    Theme* m_defaultLight = new Theme(QStringLiteral("Default (Light)"));
    Theme* m_defaultDark = new Theme(QStringLiteral("Default (Dark)"));
    Theme* m_defaultSky = new Theme(QStringLiteral("Default (Sky)"));
}

Theme::Theme(QObject* parent) : QObject(parent),
  m_name("Untitled"),
  m_isReadOnly(false),
  m_font("Times New Roman", defaults::fontSize),
  m_backgroundImage(""),
  m_fillMode(FillMode::PreserveAspectCrop),
  m_documentWidth(0.9),
  m_documentHeight(1.0),
  m_paddingVertical(20),
  m_paddingHorizontal(50),
  m_firstLineIndent(32.0),
  m_lineHeight(1.5),
  m_paragraphSpacing(24.0),
  m_fontColor(QColor(Qt::GlobalColor::black)),
  m_windowBackground(QColor(Qt::GlobalColor::lightGray)),
  m_documentBackground(QColor(Qt::GlobalColor::white)),
  m_textAlignment(HAlignment::AlignLeft),
  m_uiBackground(QColor("#d9d9d9")),
  m_uiColor(QColor("#1a1a1a")),
  m_formatH1(1),
  m_formatH2(2),
  m_formatH3(3),
  m_formatH4(4),
  m_formatH5(5),
  m_formatH6(6)
{
    m_font.setStyleStrategy(QFont::PreferAntialias);
    m_font.setHintingPreference(QFont::PreferFullHinting);
}

Theme::Theme(const QString& name, QObject *parent) : QObject(parent),
    m_name(name),
    m_isReadOnly(false),
    m_font("Times New Roman", defaults::fontSize),
    m_backgroundImage(""),
    m_fillMode(FillMode::PreserveAspectCrop),
    m_documentWidth(0.9),
    m_documentHeight(1.0),
    m_paddingVertical(20),
    m_paddingHorizontal(50),
    m_firstLineIndent(32.0),
    m_lineHeight(1.5),
    m_paragraphSpacing(24.0),
    m_fontColor(QColor(Qt::GlobalColor::black)),
    m_windowBackground(QColor(Qt::GlobalColor::lightGray)),
    m_documentBackground(QColor(Qt::GlobalColor::white)),
    m_textAlignment(HAlignment::AlignLeft),
    m_uiBackground(QColor("#d9d9d9")),
    m_uiColor(QColor("#1a1a1a")),
    m_formatH1(1),
    m_formatH2(2),
    m_formatH3(3),
    m_formatH4(4),
    m_formatH5(5),
    m_formatH6(6)
{
    Q_ASSERT(!m_name.isEmpty());
    m_font.setStyleStrategy(QFont::PreferAntialias);
    m_font.setHintingPreference(QFont::PreferFullHinting);
}

Theme::Theme(const QString& name, const Theme& copy) : QObject(copy.parent()),
    m_name(name),
    m_isReadOnly(false),
    m_font(copy.m_font),
    m_backgroundImage(copy.m_backgroundImage),
    m_fillMode(copy.m_fillMode),
    m_documentWidth(copy.m_documentWidth),
    m_documentHeight(copy.m_documentHeight),
    m_paddingVertical(copy.m_paddingVertical),
    m_paddingHorizontal(copy.m_paddingHorizontal),
    m_firstLineIndent(copy.m_firstLineIndent),
    m_lineHeight(copy.m_lineHeight),
    m_paragraphSpacing(copy.m_paragraphSpacing),
    m_fontColor(copy.m_fontColor),
    m_windowBackground(copy.m_windowBackground),
    m_documentBackground(copy.m_documentBackground),
    m_textAlignment(copy.m_textAlignment),
    m_uiBackground(copy.m_uiBackground),
    m_uiColor(copy.m_uiColor),
    m_formatH1(copy.m_formatH1),
    m_formatH2(copy.m_formatH2),
    m_formatH3(copy.m_formatH3),
    m_formatH4(copy.m_formatH4),
    m_formatH5(copy.m_formatH5),
    m_formatH6(copy.m_formatH6)
{
    Q_ASSERT(!m_name.isEmpty());
    m_font.setStyleStrategy(QFont::PreferAntialias);
    m_font.setHintingPreference(QFont::PreferFullHinting);
}

Theme* Theme::defaultLight()
{
    if (!m_defaultLight->m_isReadOnly) {
        m_defaultLight->m_isReadOnly = true;
    }

    return m_defaultLight;
}

Theme* Theme::defaultDark()
{
    if (!m_defaultDark->m_isReadOnly) {
        m_defaultDark->m_isReadOnly = true;
        m_defaultDark->m_fontColor = QColor(QColor(200, 200, 200));
        m_defaultDark->m_documentBackground = QColor(60, 60, 60);
        m_defaultDark->m_windowBackground = QColor(30, 30, 30);
        m_defaultDark->m_uiBackground = QColor(36, 36, 36);
        m_defaultDark->m_uiColor = QColor(184, 184, 184);
    }

    return m_defaultDark;
}

Theme* Theme::defaultSky()
{
    if (!m_defaultSky->m_isReadOnly) {
        m_defaultSky->m_isReadOnly = true;
        m_defaultSky->m_fontColor = colors::palette().text().color();
        m_defaultSky->m_documentBackground = colors::palette().dark().color();
        m_defaultSky->m_windowBackground = colors::palette().base().color();
        m_defaultSky->m_uiBackground = colors::palette().alternateBase().color();
        m_defaultSky->m_uiColor = colors::palette().text().color();
    }

    return m_defaultSky;
}

const QString& Theme::name() const
{
    return m_name;
}

const QFont Theme::font() const
{
    return m_font;
}

const QColor Theme::fontColor() const { return m_fontColor; }

QString Theme::fontFamily() const
{
    return m_font.family();
}

void Theme::setFontFamily(const QString& fontFamily)
{
    m_font.setFamily(fontFamily);
    emit fontFamilyChanged();
    emit fontChanged();
}

double Theme::fontSize() const
{
    return m_font.pointSizeF();
}

void Theme::setFontSize(double size)
{
    m_font.setPointSizeF(size);
    emit fontSizeChanged();
    emit fontChanged();
}

Theme::HAlignment Theme::textAlignment() const
{
    return m_textAlignment;
}

double Theme::firstLineIndent() const
{
    return m_firstLineIndent;
}

double Theme::lineHeight() const
{
    return m_lineHeight;
}

double Theme::paragraphSpacing() const
{
    return m_paragraphSpacing;
}

const HeadingFormat& Theme::headingFormat(int headingLevel) const
{
    Q_ASSERT(headingLevel > 0 && headingLevel < 7);

    switch (headingLevel) {
        case 1: return m_formatH1;
        case 2: return m_formatH2;
        case 3: return m_formatH3;
        case 4: return m_formatH4;
        case 5: return m_formatH5;
        case 6: return m_formatH6;
    }
}

const QTextCharFormat Theme::charFormat() const
{
    QTextCharFormat format;

    format.setFontFamily(fontFamily());
    format.setFontPointSize(fontSize());
    format.setProperty(QTextFormat::FontSizeAdjustment, 0);
    format.setForeground(fontColor());
    format.setFontUnderline(false);
    format.setAnchor(false);
    // This call is necessary. If a format has both a fontFamily() and fontFamilies()
    // defined, it will always prioritize the entries in fontFamilies() over
    // fontFamily(). We set fontFamilies() to an empty list here so that when
    // the user pastes some rich-text that has fontFamilies() defined (HTML),
    // it will clear them during mergeCharFormat().
    format.setFontFamilies(QStringList());

    return format;
}

const QTextBlockFormat Theme::blockFormat() const
{
    QTextBlockFormat format;

    format.setAlignment(static_cast<Qt::Alignment>(textAlignment()));
    format.setBottomMargin(paragraphSpacing());

    // Values above 3.0 are considered absolute line heights, to be added onto
    // the base line height.

    format.setLineHeight(lineHeight() <= 3.0 ? lineHeight() * 100 : lineHeight(),
                         lineHeight() <= 3.0 ? QTextBlockFormat::ProportionalHeight : QTextBlockFormat::LineDistanceHeight);
    format.setTextIndent(firstLineIndent());
    format.setHeadingLevel(0);

    return format;
}

void Theme::read(const QJsonObject& json)
{
    m_name = json["name"].toString();
    m_font = QFont(json.contains("fontFamily") ? json["fontFamily"].toString() : "Times New Roman",
                   json.contains("fontSize") ? json["fontSize"].toDouble() : 12);
    m_fillMode = json.contains("fillMode") ? FillMode(QMetaEnum::fromType<FillMode>().keyToValue(json["fillMode"].toString().toUtf8())) : FillMode::PreserveAspectCrop;
    m_documentWidth = json.contains("documentWidth") ? json["documentWidth"].toDouble() : 0.9;
    m_documentHeight = json.contains("documentHeight") ? json["documentHeight"].toDouble() : 1.0;
    m_paddingHorizontal = json.contains("paddingHorizontal") ? json["paddingHorizontal"].toDouble() : 50;
    m_paddingVertical = json.contains("paddingVertical") ? json["paddingVertical"].toDouble() : 20;
    m_firstLineIndent = json.contains("firstLineIndent") ? json["firstLineIndent"].toDouble() : 0.0;
    m_lineHeight = json.contains("lineHeight") ? json["lineHeight"].toDouble() : 1.0;
    m_paragraphSpacing = json.contains("paragraphSpacing") ? json["paragraphSpacing"].toDouble() : 0.0;
    m_backgroundImage = json.contains("backgroundImage") ? json["backgroundImage"].toString() : "";
    m_fontColor = json.contains("fontColor") ? QColor(json["fontColor"].toString()) : QColor(Qt::GlobalColor::black);
    m_windowBackground = json.contains("windowBackground") ? QColor(json["windowBackground"].toString()) : QColor(Qt::GlobalColor::lightGray);
    m_documentBackground = json.contains("documentBackground") ? QColor(json["documentBackground"].toString()) : QColor(Qt::GlobalColor::white);
    m_textAlignment = json.contains("textAlignment") ? HAlignment(QMetaEnum::fromType<HAlignment>().keyToValue(json["textAlignment"].toString().toUtf8())) : HAlignment::AlignJustify;
    m_uiBackground = json.contains("uiBackground") ? QColor(json["uiBackground"].toString()) : QColor("#d9d9d9");
    m_uiColor = json.contains("uiColor") ? QColor(json["uiColor"].toString()) : QColor("#1a1a1a");

    if (json.contains("headings")) {
        QJsonArray headings = json["headings"].toArray();

        for (const QJsonValue value : headings) {
            const QJsonObject object = value.toObject();
            int level = object["level"].toInt();
            Q_ASSERT(level != 0);

            HeadingFormat& format = const_cast<HeadingFormat&>(headingFormat(level));

            format.read(object);

            if (!object.contains("fontSize")) {
                format.setBaseFontSize(m_font.pointSizeF());
            }
        }
    } else if (json.contains("fontSize")) {
        m_formatH1.setBaseFontSize(m_font.pointSizeF());
        m_formatH2.setBaseFontSize(m_font.pointSizeF());
        m_formatH3.setBaseFontSize(m_font.pointSizeF());
        m_formatH4.setBaseFontSize(m_font.pointSizeF());
        m_formatH5.setBaseFontSize(m_font.pointSizeF());
        m_formatH6.setBaseFontSize(m_font.pointSizeF());
    }
}

void Theme::write(QJsonArray& json) const
{
    if (!m_isReadOnly) {
        QJsonObject object;
        object["name"] = m_name;
        object["fontFamily"] = fontFamily();
        object["fontSize"] = fontSize();
        object["fillMode"] = QMetaEnum::fromType<FillMode>().valueToKey(m_fillMode);
        object["documentWidth"] = m_documentWidth;
        object["documentHeight"] = m_documentHeight;
        object["paddingHorizontal"] = m_paddingHorizontal;
        object["paddingVertical"] = m_paddingVertical;

        if (m_firstLineIndent > 0.0) {
            object["firstLineIndent"] = m_firstLineIndent;
        }

        if (m_lineHeight != 1.0) {
            object["lineHeight"] = m_lineHeight;
        }

        if (m_paragraphSpacing > 0.0) {
            object["paragraphSpacing"] = m_paragraphSpacing;
        }

        if (!m_backgroundImage.isEmpty()) {
            object["backgroundImage"] = m_backgroundImage;
        }

        object["fontColor"] = QJsonValue::fromVariant(QVariant(m_fontColor));
        object["windowBackground"] = QJsonValue::fromVariant(QVariant(m_windowBackground));
        object["documentBackground"] = QJsonValue::fromVariant(QVariant(m_documentBackground));
        object["textAlignment"] = QMetaEnum::fromType<HAlignment>().valueToKey(m_textAlignment);
        object["uiBackground"] = QJsonValue::fromVariant(QVariant(m_uiBackground));
        object["uiColor"] = QJsonValue::fromVariant(QVariant(m_uiColor));

        QJsonArray headings;

        m_formatH1.write(headings);
        m_formatH2.write(headings);
        m_formatH3.write(headings);
        m_formatH4.write(headings);
        m_formatH5.write(headings);
        m_formatH6.write(headings);

        if (!headings.isEmpty()) {
            object["headings"] = headings;
        }

        json.append(object);
    }
}

Theme* Theme::fromJson(const QJsonObject& json, QObject* parent)
{
    Theme* theme = new Theme(json["name"].toString());
    theme->read(json);

    return theme;
}

const QString Theme::headingName(int headingLevel) const
{
    return headingFormat(headingLevel).name();
}
