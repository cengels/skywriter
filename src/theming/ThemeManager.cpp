#include <QColor>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include "Theme.h"
#include "ThemeManager.h"
#include "../persistence.h"
#include "../ErrorManager.h"

namespace {
    QString m_progressPath = QString();
    QString progressPath()
    {
        if (m_progressPath.isNull()) {
            m_progressPath = persistence::documentsPath() + "/themes.json";
        }

        return m_progressPath;
    }
    ThemeManager* m_instance = nullptr;

    QString duplicateName(const QString& name) {
        if (name[name.length() - 1].isDigit()) {
            QString suffix;

            const auto end = name.crend();

            for (auto i = name.crbegin(); i != end; i++) {
                const auto character = (*i);

                if (!character.isDigit()) {
                    break;
                }

                suffix.prepend(character);
            }

            const int number = suffix.toInt();
            return QStringLiteral("%1 %2")
                    .arg(name.chopped(suffix.length()))
                    .arg(number + 1);
        }

        return QStringLiteral("%1 %2").arg(name).arg(1);
    }
}

ThemeManager::ThemeManager(QObject *parent) : QObject(parent),
    m_themes({ Theme::defaultSky(), Theme::defaultLight(), Theme::defaultDark() }),
    m_activeThemeIndex(0)
{
    Q_ASSERT(!m_instance);

    for (Theme* theme : m_themes) {
        theme->setParent(this);
    }

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
        emit ErrorManager::instance()->error(tr("Couldn't open save file."));
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

    for (const QJsonValue value : array) {
        Theme* theme = Theme::fromJson(value.toObject(), this);

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

    persistence::overwrite(saveFile, static_cast<std::function<bool(QTextStream&)>>([&](QTextStream& out) {
        if (!saveFile.open(QIODevice::WriteOnly)) {
            emit ErrorManager::instance()->error(tr("Couldn't open save file."));
            return false;
        }

        QJsonArray array;

        for (const Theme* theme : m_themes) {
            theme->write(array);
        }

        QJsonDocument document(array);

        out << document.toJson();

        return true;
                           }));
}

Theme* ThemeManager::createNew()
{
    Theme* newTheme = createTheme();
    m_themes.append(newTheme);
    emit themesChanged();
    setActiveThemeIndex(m_themes.length() - 1);
    save();

    return newTheme;
}

Theme* ThemeManager::duplicate()
{
    Theme* copy = createTheme(activeTheme());
    m_themes.append(copy);
    emit themesChanged();
    setActiveThemeIndex(m_themes.length() - 1);
    save();

    return copy;
}

void ThemeManager::remove()
{
    m_themes.removeAt(activeThemeIndex());
    emit themesChanged();
    const int length = m_themes.length();
    if (activeThemeIndex() >= length) {
        setActiveThemeIndex(length - 1);
    } else {
        emit activeThemeChanged();
    }

    save();
}

Theme* ThemeManager::createTheme(const Theme* from)
{
    QString name = from ? from->name() : "Untitled";
    bool nameOk = false;
    bool tryAgain = false;

    // Check against all themes to see if the name already exists.
    // If yes, add a number at the end and check against all themes again.
    // Repeat until no themes match anymore.
    while (!nameOk) {
        for (const Theme* theme : m_themes) {
            if (theme->name() == name) {
                name = duplicateName(name);
                tryAgain = true;
            }
        }

        if (tryAgain) {
            tryAgain = false;
            continue;
        }

        nameOk = true;
    }

    if (from) {
        return new Theme(name, *from);
    }

    return new Theme(name, this);
}

ThemeManager* ThemeManager::instance() {
    if (!m_instance) {
        m_instance = new ThemeManager();
    }

    return m_instance;
}
