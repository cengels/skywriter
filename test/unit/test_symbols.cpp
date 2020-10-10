#include "gtest/gtest.h"
#include "../src/text/symbols.h"

// This file only tests basic functionality at this point, no edge cases.
// However, now that these tests exist, it will make it easier to add tests
// for edge cases later down the line, once they become a problem.

namespace {
    TEST(symbols, isWordSeparator) {
        EXPECT_TRUE(symbols::isWordSeparator(','));
        EXPECT_TRUE(symbols::isWordSeparator('.'));
        EXPECT_TRUE(symbols::isWordSeparator('#'));
        EXPECT_TRUE(symbols::isWordSeparator('*'));
        EXPECT_TRUE(symbols::isWordSeparator('!'));
        EXPECT_TRUE(symbols::isWordSeparator(0x2014));
        EXPECT_TRUE(symbols::isWordSeparator(' '));

        EXPECT_FALSE(symbols::isWordSeparator('-'));
        EXPECT_FALSE(symbols::isWordSeparator('A'));
        EXPECT_FALSE(symbols::isWordSeparator('g'));
    }

    TEST(symbols, containsWordSeparator) {
        EXPECT_FALSE(symbols::containsWordSeparator("brave"));
        EXPECT_FALSE(symbols::containsWordSeparator("test-driven")) << "Simple dashes are seen as word separators!";
        EXPECT_TRUE(symbols::containsWordSeparator("good."));
        EXPECT_TRUE(symbols::containsWordSeparator("Hello?"));
        EXPECT_TRUE(symbols::containsWordSeparator("why are"));
        EXPECT_TRUE(symbols::containsWordSeparator("\"Wait"));
        EXPECT_TRUE(symbols::containsWordSeparator("“Wait"));
        EXPECT_TRUE(symbols::containsWordSeparator("hello\ntest"));
        EXPECT_TRUE(symbols::containsWordSeparator("hello\rtest"));
    }

    TEST(symbols, sanitizeShouldRemoveEndOfText) {
        EXPECT_EQ(symbols::sanitize("\u0003"), "");
    }

    TEST(symbols, sanitizeShouldIgnoreLetters) {
        EXPECT_EQ(symbols::sanitize("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"), "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");
    }

    TEST(symbols, sanitizeShouldIgnorePrintableSymbols) {
        EXPECT_EQ(symbols::sanitize(".,-—_–#*´$"), ".,-—_–#*´$");
    }
}
