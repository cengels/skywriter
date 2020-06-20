#ifndef STRINGREPLACER_H
#define STRINGREPLACER_H

#include <QString>
#include <QHash>

#include "Replacement.h"

class StringReplacer
{
    public:
        StringReplacer();

        void setReplacement(const QString& target, const QString& replaceWith);
        void setReplacement(const QChar& target, const QChar& replaceWith);
        //! Replaces the given character(s) with replaceWithStart if it appears
        //! before a word, and with replaceWithEnd if it appears after a word.
        void setSmartReplacement(const QString& target, const QString& replaceWithStart, const QString& replaceWithEnd);
        //! Replaces the given character(s) with replaceWithStart if it appears
        //! before a word, and with replaceWithEnd if it appears after a word.
        void setSmartReplacement(const QChar& target, const QChar& replaceWithStart, const QChar& replaceWithEnd);
        bool isSmartReplacement(const QString& string) const;

        //! Clears all replacements from this StringReplacer.
        void clear();

        QString replace(const QString& source, const QString& before = "") const;

    private:
        // See Replacement. This was supposed to be a const Replacement, but
        // for reasons probably internal to how a QHash operates, using const
        // values is simply not possible.
        QHash<const QString, Replacement> m_replacements;
};

#endif // STRINGREPLACER_H
