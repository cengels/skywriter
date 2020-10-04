#include <QtTest/QtTest>
#include "../src/text/symbols.h"

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
};

QTEST_MAIN(TestSymbols)
#include "testsymbols.moc"
