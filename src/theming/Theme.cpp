#include <QColor>
#include "Theme.h"

namespace {
    Theme* m_defaultLight = new Theme();
    Theme* m_defaultDark = new Theme();
}

Theme::Theme(QObject *parent) : QObject(parent),
    m_name(""),
    m_isReadOnly(false),
    m_font("Times New Roman", 12),
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
    m_textAlignment(HAlignment::AlignLeft)
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
    }

    return m_defaultDark;
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

