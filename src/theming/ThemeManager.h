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

    Q_PROPERTY(QList<Theme*> themes READ themes NOTIFY themesChanged);
    Q_PROPERTY(Theme* activeTheme READ activeTheme NOTIFY activeThemeChanged);
    Q_PROPERTY(int activeThemeIndex READ activeThemeIndex WRITE setActiveThemeIndex NOTIFY activeThemeIndexChanged);

    public:
        explicit ThemeManager(QObject *parent = nullptr);

        const QList<Theme*> themes() const;

        Theme* activeTheme() const;

        int activeThemeIndex() const;
        void setActiveThemeIndex(int index);

        static ThemeManager* instance();

    public Q_SLOTS:
        //! Loads all themes from the file system.
        void load();
        //! Saves all themes to the file system.
        void save() const;
        //! Creates a new theme.
        Theme* createNew();
        //! Duplicates the selected theme.
        Theme* duplicate();
        //! Removes the selected theme.
        void remove();

    Q_SIGNALS:
        void themesChanged();
        void activeThemeChanged();
        void activeThemeIndexChanged();

    private:
        Theme* createTheme(const Theme* from = nullptr);
        QList<Theme*> m_themes;
        int m_activeThemeIndex;
};

#endif // THEMEMANAGER_H
