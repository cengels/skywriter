#include <QColor>
#include "Theme.h"
#include "ThemeManager.h"

ThemeManager::ThemeManager(QObject *parent) : QObject(parent),
    m_themes({ Theme::defaultLight(), Theme::defaultDark() }),
    m_activeThemeIndex(0)
{ }

const QList<Theme*> ThemeManager::themes() const {
    return m_themes;
}

const Theme* ThemeManager::activeTheme() const {
    return m_themes.at(m_activeThemeIndex);
}

int ThemeManager::activeThemeIndex() const {
    return m_activeThemeIndex;
}

void ThemeManager::setActiveThemeIndex(int index) {
    if (m_activeThemeIndex != index) {
        m_activeThemeIndex = index;
        emit activeThemeChanged();
        emit activeThemeIndexChanged();
    }
}
