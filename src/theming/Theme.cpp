#include <QColor>
#include <QPalette>
#include <QJsonObject>
#include <QJsonArray>
#include <QVariant>
#include <QMetaEnum>
#include "Theme.h"
#include "../colors.h"

namespace {
    Theme* m_defaultLight = new Theme();
    Theme* m_defaultDark = new Theme();
    Theme* m_defaultSky = new Theme();

    QString duplicateName(const QString& name) {
        if (name[name.length() - 1].isDigit()) {
            QString suffix;

            const auto end = suffix.crend();

            for (auto i = suffix.crbegin(); i != end; i++) {
                const auto character = (*i);

                if (!character.isDigit()) {
                    break;
                }

                suffix.prepend(character);
            }

            const int number = suffix.toInt();
            return QStringLiteral("%1 %2")
                    .arg(name.chopped(suffix.length()))
                    .arg(number);
        }

        return QStringLiteral("%1 %2").arg(name).arg(1);
    }
}

Theme::Theme(QObject *parent) : QObject(parent),
    m_name(""),
    m_isReadOnly(false),
    m_font("Times New Roman", 18),
    m_backgroundImage(""),
    m_fillMode(FillMode::PreserveAspectCrop),
    m_documentWidth(0.9),
    m_documentHeight(1.0),
    m_paddingVertical(20),
    m_paddingHorizontal(50),
    m_firstLineIndent(10.0),
    m_lineHeight(1.0),
    m_paragraphSpacing(0.0),
    m_fontColor(QColor(Qt::GlobalColor::black)),
    m_windowBackground(QColor(Qt::GlobalColor::lightGray)),
    m_documentBackground(QColor(Qt::GlobalColor::white)),
    m_textAlignment(HAlignment::AlignLeft),
    m_uiBackground(QColor("#d9d9d9")),
    m_uiColor(QColor("#1a1a1a"))
{
    m_font.setStyleStrategy(QFont::PreferAntialias);
    m_font.setHintingPreference(QFont::PreferFullHinting);
}

Theme::Theme(const Theme& copy, QObject *parent) : QObject(parent),
    m_name(duplicateName(copy.m_name)),
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
    m_uiColor(copy.m_uiColor)
{
    m_font.setStyleStrategy(QFont::PreferAntialias);
    m_font.setHintingPreference(QFont::PreferFullHinting);
}

Theme* Theme::defaultLight()
{
    if (!m_defaultLight->m_isReadOnly) {
        m_defaultLight->m_isReadOnly = true;
        m_defaultLight->m_name = "Default (Light)";
    }

    return m_defaultLight;
}

Theme* Theme::defaultDark()
{
    if (!m_defaultDark->m_isReadOnly) {
        m_defaultDark->m_isReadOnly = true;
        m_defaultDark->m_name = "Default (Dark)";
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
        m_defaultSky->m_name = "Default (Sky)";
        m_defaultSky->m_fontColor = Skywriter::palette().text().color();
        m_defaultSky->m_documentBackground = Skywriter::palette().dark().color();
        m_defaultSky->m_windowBackground = Skywriter::palette().base().color();
        m_defaultSky->m_uiBackground = Skywriter::palette().alternateBase().color();
        m_defaultSky->m_uiColor = Skywriter::palette().text().color();
    }

    return m_defaultSky;
}

const QFont Theme::font() const
{
    return m_font;
}

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

void Theme::read(const QJsonObject& json)
{
    m_name = json["name"].toString();
    m_font = QFont(json["fontFamily"].toString(), json["fontSize"].toDouble());
    m_fillMode = FillMode(QMetaEnum::fromType<FillMode>().keyToValue(json["fillMode"].toString().toUtf8()));
    m_documentWidth = json["documentWidth"].toDouble();
    m_documentHeight = json["documentHeight"].toDouble();
    m_paddingHorizontal = json["paddingHorizontal"].toDouble();
    m_paddingVertical = json["paddingVertical"].toDouble();

    if (json.contains("firstLineIndent")) {
        m_firstLineIndent = json["firstLineIndent"].toDouble();
    }

    if (json.contains("lineHeight")) {
        m_lineHeight = json["lineHeight"].toDouble();
    }

    if (json.contains("paragraphSpacing")) {
        m_paragraphSpacing = json["paragraphSpacing"].toDouble();
    }

    if (json.contains("backgroundImage")) {
        m_backgroundImage = json["backgroundImage"].toString();
    }

    m_fontColor = QColor(json["fontColor"].toString());
    m_windowBackground = QColor(json["windowBackground"].toString());
    m_documentBackground = QColor(json["documentBackground"].toString());

    m_textAlignment = HAlignment(QMetaEnum::fromType<HAlignment>().keyToValue(json["textAlignment"].toString().toUtf8()));

    if (json.contains("uiBackground")) {
        m_uiBackground = QColor(json["uiBackground"].toString());
    }

    if (json.contains("uiColor")) {
        m_uiColor = QColor(json["uiColor"].toString());
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

        if (m_lineHeight > 0.0) {
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

        json.append(object);
    }
}

