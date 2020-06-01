#include <QException>

#include "Replacement.h"

Replacement::Replacement() : m_openingSymbol(), m_closingSymbol() { }

Replacement::Replacement(const Replacement& replacement) :
    m_openingSymbol(replacement.m_openingSymbol),
    m_closingSymbol(replacement.m_closingSymbol) { }

Replacement::Replacement(const QString replaceWith)
    : m_openingSymbol(replaceWith), m_closingSymbol() { }

Replacement::Replacement(const QString openingSymbol, const QString closingSymbol)
    : m_openingSymbol(openingSymbol), m_closingSymbol(closingSymbol) { }

bool Replacement::isValid() const
{
    return !m_openingSymbol.isEmpty();
}

bool Replacement::isSmartReplacement() const
{
    return !m_closingSymbol.isEmpty();
}

const QString& Replacement::replacement() const
{
    if (isSmartReplacement()) {
        throw QException();
    }

    return m_openingSymbol;
}

const QString& Replacement::openingSymbol() const
{
    if (!isSmartReplacement()) {
        throw QException();
    }

    return m_openingSymbol;
}

const QString& Replacement::closingSymbol() const
{
    if (!isSmartReplacement()) {
        throw QException();
    }

    return m_closingSymbol;
}
