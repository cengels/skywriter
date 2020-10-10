#include "gtest/gtest.h"
#include <QTextCursor>
#include <QTextDocument>
#include "text/selection.h"
#include "../customqtprint.h"

class SelectionTest : public ::testing::Test
{
    public:
        SelectionTest() {
            this->document.setPlainText("A word.\nWith multiple periods...\nCombined with a dash-separated word. There's also an apostrophe.\n“I hate you,” Anakin said.");
            this->cursor = QTextCursor(&this->document);
        }

    protected:
        QTextDocument document;
        QTextCursor cursor;
};

TEST_F(SelectionTest, shouldSelectSingleLetterWordAtStart) {
    selection::selectWord(this->cursor);
    ASSERT_TRUE(this->cursor.hasSelection());
    EXPECT_EQ(this->cursor.selectedText(), "A");
}

TEST_F(SelectionTest, shouldSelectSingleLetterWordAtEnd) {
    this->cursor.movePosition(QTextCursor::MoveOperation::NextCharacter);
    selection::selectWord(this->cursor);
    ASSERT_TRUE(this->cursor.hasSelection());
    EXPECT_EQ(this->cursor.selectedText(), "A");
}

TEST_F(SelectionTest, shouldSelectMultiLetterWordAtStart) {
    this->cursor.movePosition(QTextCursor::MoveOperation::NextCharacter, QTextCursor::MoveAnchor, 2);
    selection::selectWord(this->cursor);
    ASSERT_TRUE(this->cursor.hasSelection());
    EXPECT_EQ(this->cursor.selectedText(), "word");
}

TEST_F(SelectionTest, shouldSelectMultiLetterWordInMiddle) {
    this->cursor.movePosition(QTextCursor::MoveOperation::NextCharacter, QTextCursor::MoveAnchor, 4);
    selection::selectWord(this->cursor);
    ASSERT_TRUE(this->cursor.hasSelection());
    EXPECT_EQ(this->cursor.selectedText(), "word");
}

TEST_F(SelectionTest, shouldSelectMultiLetterWordAtEnd) {
    this->cursor.movePosition(QTextCursor::MoveOperation::NextCharacter, QTextCursor::MoveAnchor, 6);
    selection::selectWord(this->cursor);
    ASSERT_TRUE(this->cursor.hasSelection());
    EXPECT_EQ(this->cursor.selectedText(), "word");
}

TEST_F(SelectionTest, shouldSelectPeriodAtEndOfBlock) {
    this->cursor.movePosition(QTextCursor::MoveOperation::NextCharacter, QTextCursor::MoveAnchor, 7);
    selection::selectWord(this->cursor);
    ASSERT_TRUE(this->cursor.hasSelection());
    EXPECT_EQ(this->cursor.selectedText(), ".");
}

TEST_F(SelectionTest, shouldSelectMultiplePeriodsAtEndOfBlock) {
    this->cursor.movePosition(QTextCursor::MoveOperation::NextCharacter, QTextCursor::MoveAnchor, 32);
    selection::selectWord(this->cursor);
    ASSERT_TRUE(this->cursor.hasSelection());
    EXPECT_EQ(this->cursor.selectedText(), "...");
}

TEST_F(SelectionTest, shouldSelectHyphenatedWord) {
    this->cursor.movePosition(QTextCursor::MoveOperation::NextCharacter, QTextCursor::MoveAnchor, 53);
    selection::selectWord(this->cursor);
    ASSERT_TRUE(this->cursor.hasSelection());
    EXPECT_EQ(this->cursor.selectedText(), "dash-separated");
}

TEST_F(SelectionTest, shouldSelectWordWithApostrophe) {
    this->cursor.movePosition(QTextCursor::MoveOperation::NextCharacter, QTextCursor::MoveAnchor, 72);
    selection::selectWord(this->cursor);
    ASSERT_TRUE(this->cursor.hasSelection());
    EXPECT_EQ(this->cursor.selectedText(), "There's");
}

TEST_F(SelectionTest, shouldSelectWordAfterQuote) {
    this->cursor.movePosition(QTextCursor::MoveOperation::NextCharacter, QTextCursor::MoveAnchor, 99);
    selection::selectWord(this->cursor);
    ASSERT_TRUE(this->cursor.hasSelection());
    EXPECT_EQ(this->cursor.selectedText(), "I");
}

TEST_F(SelectionTest, shouldSelectSymbolBeforeQuoteAndQuote) {
    this->cursor.movePosition(QTextCursor::MoveOperation::NextCharacter, QTextCursor::MoveAnchor, 110);
    selection::selectWord(this->cursor);
    ASSERT_TRUE(this->cursor.hasSelection());
    EXPECT_EQ(this->cursor.selectedText(), ",”");
}
