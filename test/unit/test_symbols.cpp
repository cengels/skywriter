#include <QtTest/QtTest>
#include "../src/text/symbols.h"

// This file only tests basic functionality at this point, no edge cases.
// However, now that these tests exist, it will make it easier to add tests
// for edge cases later down the line, once they become a problem.

class TestSymbols: public QObject
{
	Q_OBJECT
	
	private slots:
        void isWordSeparator_data()
        {
            QTest::addColumn<QChar>("separator");
            QTest::addColumn<bool>("result");

            QTest::newRow(",") << QChar(',') << true;
            QTest::newRow(".") << QChar('.') << true;
            QTest::newRow("#") << QChar('#') << true;
            QTest::newRow("*") << QChar('*') << true;
            QTest::newRow("!") << QChar('!') << true;
            QTest::newRow("-") << QChar('-') << false;
            QTest::newRow("—") << QChar(0x2014) << true;
            QTest::newRow(" ") << QChar(' ') << true;
            QTest::newRow("A") << QChar('A') << false;
            QTest::newRow("g") << QChar('g') << false;
        }

        void isWordSeparator()
        {
            QFETCH(QChar, separator);

            QTEST(symbols::isWordSeparator(separator), "result");
        }

        void containsWordSeparator_data()
        {
            QTest::addColumn<QString>("source");
            QTest::addColumn<bool>("result");

            QTest::newRow("no word separator") << "brave" << false;
            QTest::newRow(".") << "good." << true;
            QTest::newRow("?") << "Hello?" << true;
            QTest::newRow("space") << "why are" << true;
            QTest::newRow("-") << "test-driven" << false;
            QTest::newRow("curly quotes") << "“Wait" << true;
        }

        void containsWordSeparator()
        {
            QFETCH(QString, source);

            QTEST(symbols::containsWordSeparator(source), "result");
        }

        void sanitize_data()
        {
            QTest::addColumn<QString>("source");
            QTest::addColumn<QString>("result");

            QTest::newRow("END OF TEXT") << "\u0003" << "";
            QTest::newRow("normal letters") << "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ" << "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
            QTest::newRow("printable symbols") << ".,-—_–#*´$" << ".,-—_–#*´$";
        }

        void sanitize()
        {
            QFETCH(QString, source);

            QTEST(symbols::sanitize(source), "result");
        }
};

QTEST_MAIN(TestSymbols)
#include "testsymbols.moc"
