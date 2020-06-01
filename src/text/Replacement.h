#ifndef REPLACEMENT_H
#define REPLACEMENT_H

#include <QString>

class Replacement
{
    public:
        explicit Replacement();
        Replacement(const Replacement& replacement);
        Replacement(const QString replacement);
        Replacement(const QString openingSymbol, const QString closingSymbol);

        bool isValid() const;
        bool isSmartReplacement() const;

        //! Returns the string to replace target with.
        //! If this is a smart replacement, throws an error.
        const QString& replacement() const;
        //! Returns the opening symbol for the smart replacement.
        //! If this is not a smart replacement, throws an error.
        const QString& openingSymbol() const;
        //! Returns the closing symbol for the smart replacement.
        //! If this is not a smart replacement, throws an error.
        const QString& closingSymbol() const;

    private:
        // I wanted these two properties to be const, but it seems that's not
        // possible if you want instances of this object to be contained
        // in a QHash for some reason.
        QString m_openingSymbol;
        QString m_closingSymbol;
};

#endif // REPLACEMENT_H
