#include <QDebug>

#include "HeadingFormat.h"
#include "defaults.h"

HeadingFormat::HeadingFormat(int headingLevel) :
    m_blockFormat(),
    m_charFormat(),
    m_name(QString())
{
    m_blockFormat.setHeadingLevel(headingLevel);
    setAlignment(defaults::headingAlignment);

    // Creates heading levels in patterns of two. That is:
    // h1: Main title (space above)
    // h2: Subtitle of main title (space below)
    // h3: Level 3 title (space above)
    // h4: Subtitle of level 3 title (space below)
    // ...

    setSpaceBefore(headingLevel % 2 == 1 && headingLevel != 1 ? defaults::headingSpacing : defaults::headingSpacing * 0.2);
    setSpaceAfter(headingLevel % 2 == 0 ? defaults::headingSpacing : defaults::headingSpacing * 0.2);
    setBaseFontSize(defaults::fontSize);

    if (headingLevel < 5) {
        setTextDecorations(TextDecorations::Bold);
    }
}

const QTextBlockFormat& HeadingFormat::blockFormat() const
{
    return m_blockFormat;
}

const QTextCharFormat& HeadingFormat::charFormat() const
{
    return m_charFormat;
}

int HeadingFormat::headingLevel() const
{
    return m_blockFormat.headingLevel();
}

QString HeadingFormat::name() const
{
    return m_name;
}

void HeadingFormat::setName(const QString& name)
{
    m_name = name;
}

Qt::Alignment HeadingFormat::alignment() const
{
    return m_blockFormat.alignment();
}

void HeadingFormat::setAlignment(Qt::Alignment alignment)
{
    m_blockFormat.setAlignment(alignment);
}

HeadingFormat::TextDecorations HeadingFormat::textDecorations() const
{
    int textDecorations = TextDecorations::None;

    if (m_charFormat.fontWeight() == 700) {
        textDecorations |= TextDecorations::Bold;
    }

    if (m_charFormat.fontItalic()) {
        textDecorations |= TextDecorations::Italic;
    }

    if (m_charFormat.underlineStyle() == QTextCharFormat::SingleUnderline) {
        textDecorations |= TextDecorations::Underline;
    }

    return static_cast<HeadingFormat::TextDecorations>(textDecorations);
}

void HeadingFormat::setTextDecorations(HeadingFormat::TextDecorations decorations)
{
    if (decorations & Bold) {
        m_charFormat.setFontWeight(QFont::Bold);
    }

    if (decorations & Italic) {
        m_charFormat.setFontItalic(true);
    }

    if (decorations & Underline) {
        m_charFormat.setFontUnderline(true);
    }
}

double HeadingFormat::indent() const
{
    return m_blockFormat.indent();
}

void HeadingFormat::setIndent(double indent)
{
    m_blockFormat.setIndent(indent);
}

double HeadingFormat::spaceBefore() const
{
    return m_blockFormat.topMargin();
}

void HeadingFormat::setSpaceBefore(double spaceBefore)
{
    m_blockFormat.setTopMargin(spaceBefore);
}

double HeadingFormat::spaceAfter() const
{
    return m_blockFormat.bottomMargin();
}

void HeadingFormat::setSpaceAfter(double spaceAfter)
{
    m_blockFormat.setBottomMargin(spaceAfter);
}

double HeadingFormat::fontSize() const
{
    return m_charFormat.fontPointSize();
}

void HeadingFormat::setFontSize(double fontSize)
{
    m_charFormat.setFontPointSize(fontSize);
}

void HeadingFormat::setBaseFontSize(double base)
{
    setFontSize(base + base * (1.0 / headingLevel()));
}

QFont::Capitalization HeadingFormat::capitalization() const
{
    return m_charFormat.fontCapitalization();
}

void HeadingFormat::setCapitalization(QFont::Capitalization capitalization)
{
    m_charFormat.setFontCapitalization(capitalization);
}

double HeadingFormat::letterSpacing() const
{
    return m_charFormat.fontLetterSpacing();
}

void HeadingFormat::setLetterSpacing(double letterSpacing)
{
    return m_charFormat.setFontLetterSpacing(letterSpacing);
}

void HeadingFormat::read(const QJsonObject& json)
{
    Q_ASSERT(json["level"].toInt() == headingLevel());

    if (json.contains("name")) {
        setName(json["name"].toString());
    }

    if (json.contains("alignment")) {
        setAlignment(Qt::Alignment(QMetaEnum::fromType<Qt::Alignment>().keyToValue(json["alignment"].toString().toUtf8())));
    }

    if (json.contains("indent")) {
        setIndent(json["indent"].toDouble());
    }

    if (json.contains("spaceBefore")) {
        setSpaceBefore(json["spaceBefore"].toDouble());
    }

    if (json.contains("spaceAfter")) {
        setSpaceAfter(json["spaceAfter"].toDouble());
    }

    if (json.contains("fontSize")) {
        setFontSize(json["fontSize"].toDouble());
    }

    if (json.contains("capitalization")) {
        setCapitalization(QFont::Capitalization(QMetaEnum::fromType<QFont::Capitalization>().keyToValue(json["capitalization"].toString().toUtf8())));
    }

    int textDecorations = TextDecorations::None;

    if (json["bold"].toBool()) {
        textDecorations |= TextDecorations::Bold;
    }

    if (json["italic"].toBool()) {
        textDecorations |= TextDecorations::Italic;
    }

    if (json["underline"].toBool()) {
        textDecorations |= TextDecorations::Underline;
    }

    setTextDecorations(static_cast<TextDecorations>(textDecorations));

    if (json.contains("letterSpacing")) {
        setLetterSpacing(json["letterSpacing"].toDouble());
    }
}

void HeadingFormat::write(QJsonArray& json) const
{
    QJsonObject object;

    if (!name().isEmpty()) {
        object["name"] = name();
    }

    if (alignment() != defaults::headingAlignment) {
        object["alignment"] = QMetaEnum::fromType<Qt::Alignment>().valueToKey(alignment());
    }

    if (indent() != 0.0) {
        object["indent"] = indent();
    }

    if (spaceBefore() != (headingLevel() % 2 == 1 && headingLevel() != 1 ? defaults::headingSpacing : 0)) {
        object["spaceBefore"] = spaceBefore();
    }

    if (spaceAfter() != (headingLevel() % 2 == 0 ? defaults::headingSpacing : 0)) {
        object["spaceAfter"] = spaceAfter();
    }

    if (fontSize() != defaults::fontSize + defaults::fontSize * (1 / headingLevel())) {
        object["fontSize"] = fontSize();
    }

    if (capitalization() != 0x0000) {
        object["capitalization"] = QMetaEnum::fromType<QFont::Capitalization>().valueToKey(capitalization());
    }

    if (letterSpacing() != 0.0) {
        object["letterSpacing"] = letterSpacing();
    }

    if (textDecorations() != TextDecorations::None) {
        object["bold"] = bool(textDecorations() & TextDecorations::Bold);
        object["italic"] = bool(textDecorations() & TextDecorations::Italic);
        object["underline"] = bool(textDecorations() & TextDecorations::Underline);
    }

    if (!object.isEmpty()) {
        object["level"] = headingLevel();
        json.append(object);
    }
}

bool HeadingFormat::operator==(const HeadingFormat& other) const
{
    return headingLevel() == other.headingLevel()
        && name() == other.name()
        && alignment() == other.alignment()
        && indent() == other.indent()
        && spaceBefore() == other.spaceBefore()
        && spaceAfter() == other.spaceAfter()
        && fontSize() == other.fontSize()
        && capitalization() == other.capitalization()
        && letterSpacing() == other.letterSpacing()
        && textDecorations() == other.textDecorations();

}

bool HeadingFormat::operator!=(const HeadingFormat& other) const
{
    return !(*this == other);
}
