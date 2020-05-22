#include <QColor>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include "Theme.h"
#include "ThemeManager.h"
#include "../persistence.h"

namespace {
    QString m_progressPath = QString();
    QString progressPath()
    {
        if (m_progressPath.isNull()) {
            m_progressPath = persistence::documentsPath() + "/themes2.json";
        }

        return m_progressPath;
    }
    ThemeManager* m_instance = nullptr;
}

ThemeManager::ThemeManager(QObject *parent) : QObject(parent),
    m_themes({ Theme::defaultSky(), Theme::defaultLight(), Theme::defaultDark() }),
    m_activeThemeIndex(0)
{
    Q_ASSERT(!m_instance);

    load();
}

const QList<Theme*> ThemeManager::themes() const {
    return m_themes;
}

Theme* ThemeManager::activeTheme() const {
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

void ThemeManager::load() {
    QFile saveFile(progressPath());

    if (!saveFile.open(QIODevice::ReadWrite)) {
        qWarning("Couldn't open save file.");
        return;
    }

    QByteArray themeData = saveFile.readAll();
    saveFile.close();

    QJsonDocument document = QJsonDocument::fromJson(themeData);

    if (!document.isArray()) {
        // No themes written yet
        return;
    }

    QJsonArray array = document.array();

    foreach (const QJsonValue value, array) {
        Theme* theme = new Theme(this);
        theme->read(value.toObject());

        if (!std::any_of(m_themes.cbegin(), m_themes.cend(), [&theme](const Theme* item) { return item->name() == theme->name(); })) {
            // If the user manually edited the themes.json file so that there
            // are now duplicate names, ignores all subsequent themes with the
            // same name to avoid issues.
            m_themes.append(theme);
        }
    }
}

void ThemeManager::save() const {
    QFile saveFile(progressPath());

    if (!saveFile.open(QIODevice::WriteOnly)) {
        qWarning("Couldn't open save file.");
        return;
    }

    QJsonArray array;

    foreach (const Theme* theme, m_themes) {
        theme->write(array);
    }

    QJsonDocument document(array);

    saveFile.write(document.toJson());
    saveFile.close();
}

ThemeManager* ThemeManager::instance() {
    if (!m_instance) {
        m_instance = new ThemeManager();
    }

    return m_instance;
}
