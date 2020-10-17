#include "gtest/gtest.h"
#include <QTextCursor>
#include <QTextDocument>
#include "text/selection.h"
#include "../customqtprint.h"

class WordSelectionTest : public ::testing::Test
{
    public:
        WordSelectionTest() {
            this->document.setPlainText("A word.\nWith multiple periods...\nCombined with a dash-separated word. There's also an apostrophe.\n“I hate you,” Anakin said.");
            this->cursor = QTextCursor(&this->document);
        }

    protected:
        QTextDocument document;
        QTextCursor cursor;
};

TEST_F(WordSelectionTest, shouldSelectSingleLetterWordAtStart) {
    selection::selectWord(this->cursor);
    ASSERT_TRUE(this->cursor.hasSelection());
    EXPECT_EQ(this->cursor.selectedText(), "A");
}

TEST_F(WordSelectionTest, shouldSelectSingleLetterWordAtEnd) {
    this->cursor.setPosition(1);
    selection::selectWord(this->cursor);
    ASSERT_TRUE(this->cursor.hasSelection());
    EXPECT_EQ(this->cursor.selectedText(), "A");
}

TEST_F(WordSelectionTest, shouldSelectMultiLetterWordAtStart) {
    this->cursor.setPosition(2);
    selection::selectWord(this->cursor);
    ASSERT_TRUE(this->cursor.hasSelection());
    EXPECT_EQ(this->cursor.selectedText(), "word");
}

TEST_F(WordSelectionTest, shouldSelectMultiLetterWordInMiddle) {
    this->cursor.setPosition(4);
    selection::selectWord(this->cursor);
    ASSERT_TRUE(this->cursor.hasSelection());
    EXPECT_EQ(this->cursor.selectedText(), "word");
}

TEST_F(WordSelectionTest, shouldSelectMultiLetterWordAtEnd) {
    this->cursor.setPosition(6);
    selection::selectWord(this->cursor);
    ASSERT_TRUE(this->cursor.hasSelection());
    EXPECT_EQ(this->cursor.selectedText(), "word");
}

TEST_F(WordSelectionTest, shouldSelectLastWordIncludingPeriodAtEndOfBlock) {
    this->cursor.setPosition(7);
    selection::selectWord(this->cursor);
    ASSERT_TRUE(this->cursor.hasSelection());
    EXPECT_EQ(this->cursor.selectedText(), "word.");
}

TEST_F(WordSelectionTest, shouldSelectMultiplePeriodsAtEndOfBlock) {
    this->cursor.setPosition(32);
    selection::selectWord(this->cursor);
    ASSERT_TRUE(this->cursor.hasSelection());
    EXPECT_EQ(this->cursor.selectedText(), "...");
}

TEST_F(WordSelectionTest, shouldSelectPartialHyphenatedWord) {
    this->cursor.setPosition(52);
    selection::selectWord(this->cursor);
    ASSERT_TRUE(this->cursor.hasSelection());
    EXPECT_EQ(this->cursor.selectedText(), "dash");
}

TEST_F(WordSelectionTest, shouldSelectPartialHyphenatedWord2) {
    this->cursor.setPosition(56);
    selection::selectWord(this->cursor);
    ASSERT_TRUE(this->cursor.hasSelection());
    EXPECT_EQ(this->cursor.selectedText(), "separated");
}

TEST_F(WordSelectionTest, shouldSelectWordWithApostrophe) {
    this->cursor.setPosition(72);
    selection::selectWord(this->cursor);
    ASSERT_TRUE(this->cursor.hasSelection());
    EXPECT_EQ(this->cursor.selectedText(), "There's");
}

TEST_F(WordSelectionTest, shouldSelectWordAfterQuote) {
    this->cursor.setPosition(99);
    selection::selectWord(this->cursor);
    ASSERT_TRUE(this->cursor.hasSelection());
    EXPECT_EQ(this->cursor.selectedText(), "I");
}

TEST_F(WordSelectionTest, shouldSelectSymbolBeforeQuoteAndQuote) {
    this->cursor.setPosition(110);
    selection::selectWord(this->cursor);
    ASSERT_TRUE(this->cursor.hasSelection());
    EXPECT_EQ(this->cursor.selectedText(), "you,”");
}
