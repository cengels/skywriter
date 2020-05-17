#ifndef THEMEMANAGER_H
#define THEMEMANAGER_H

#include <QObject>

#include "Theme.h"

/*!
    Singleton responsible for managing the currently active theme and writing
    changes to application themes to the file system.
*/
class ThemeManager : public QObject {
    Q_OBJECT

    Q_PROPERTY(const QList<Theme*> themes READ themes NOTIFY themesChanged);
    Q_PROPERTY(const Theme* activeTheme READ activeTheme NOTIFY activeThemeChanged);
    Q_PROPERTY(int activeThemeIndex READ activeThemeIndex WRITE setActiveThemeIndex NOTIFY activeThemeIndexChanged);

    public:
        explicit ThemeManager(QObject *parent = nullptr);

        const QList<Theme*> themes() const;

        const Theme* activeTheme() const;

        int activeThemeIndex() const;
        void setActiveThemeIndex(int index);

    public Q_SLOTS:

    Q_SIGNALS:
        void themesChanged();
        void activeThemeChanged();
        void activeThemeIndexChanged();

    private:
        const QList<Theme*> m_themes;
        int m_activeThemeIndex;
};

#endif // THEMEMANAGER_H
