#include <QDebug>

#include "StringReplacer.h"
#include "symbols.h"

namespace {
    const QString& getReplacement(const Replacement& replacement, const QString& before = QString())
    {
        if (replacement.isSmartReplacement()) {
            if (!before.isEmpty() && symbols::smart_replacement_prefixes.contains(before.at(before.length() - 1))) {
                return replacement.openingSymbol();
            }

            return replacement.closingSymbol();
        }

        return replacement.replacement();
    }
}

StringReplacer::StringReplacer() : m_replacements() { }

void StringReplacer::setReplacement(const QString& target, const QString& replaceWith)
{
    m_replacements.insert(target, Replacement(replaceWith));
}

void StringReplacer::setReplacement(const QChar& target, const QChar& replaceWith)
{
    m_replacements.insert(target, Replacement(replaceWith));
}

void StringReplacer::setSmartReplacement(const QString& target, const QString& replaceWithStart, const QString& replaceWithEnd)
{
    m_replacements.insert(target, Replacement(replaceWithStart, replaceWithEnd));
}

void StringReplacer::setSmartReplacement(const QChar& target, const QChar& replaceWithStart, const QChar& replaceWithEnd)
{
    m_replacements.insert(target, Replacement(replaceWithStart, replaceWithEnd));
}

bool StringReplacer::isSmartReplacement(const QString& string) const
{
    if (m_replacements.contains(string)) {
        return m_replacements.value(string).isSmartReplacement();
    }

    return false;
}

void StringReplacer::clear()
{
    m_replacements.clear();
}

QString StringReplacer::replace(const QString& source, const QString& before) const
{
    if (m_replacements.contains(source)) {
        // If the entire source string matches, replace it directly.

        return getReplacement(m_replacements.value(source), before);
    }

    QHashIterator<const QString, Replacement> i(m_replacements);
    QString result(source);

    while (i.hasNext()) {
        i.next();

        int index = result.indexOf(i.key());

        while (index > -1) {
            const QString& prefix = index == 0 ? before : result.left(index);
            result.replace(index, i.key().length(), prefix);

            index = result.indexOf(i.key());
        }
    }

    return result;
}
