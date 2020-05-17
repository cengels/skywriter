#include <QColor>
#include "Theme.h"

namespace {
    Theme* m_defaultLight = new Theme();
    Theme* m_defaultDark = new Theme();
}

Theme::Theme(QObject *parent) : QObject(parent),
    m_name(""),
    m_isReadOnly(false),
    m_fontSize(12.0),
    m_fontFamily("Times New Roman"),
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
{ }

Theme* Theme::defaultLight() {
    if (!m_defaultLight->m_isReadOnly) {
        m_defaultLight->m_isReadOnly = true;
        m_defaultLight->m_name = "Default (Light)";
    }

    return m_defaultLight;
}

Theme* Theme::defaultDark() {
    if (!m_defaultDark->m_isReadOnly) {
        m_defaultDark->m_isReadOnly = true;
        m_defaultDark->m_name = "Default (Dark)";
        m_defaultDark->m_fontColor = QColor(Qt::GlobalColor::white);
        m_defaultDark->m_windowBackground = QColor(Qt::GlobalColor::darkGray);
        m_defaultDark->m_windowBackground = QColor(10, 10, 10);
    }

    return m_defaultDark;
}
