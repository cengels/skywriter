#include "gtest/gtest.h"
#include <QTextCursor>
#include <QTextDocument>
#include "text/selection.h"
#include "../customqtprint.h"

class WordMovementTest : public ::testing::Test
{
    public:
        WordMovementTest() {
            this->document.setPlainText("A word.\nWith multiple periods...\nCombined with a dash-separated word. There's also an apostrophe.\n“I hate you,” Anakin said.");
            this->cursor = QTextCursor(&this->document);
        }

    protected:
        QTextDocument document;
        QTextCursor cursor;
};

TEST_F(WordMovementTest, shouldNotMoveBackwardAtStart) {
    selection::moveToPreviousWord(this->cursor);
    ASSERT_FALSE(this->cursor.hasSelection());
    EXPECT_EQ(this->cursor.anchor(), 0);
}

TEST_F(WordMovementTest, shouldNotMoveForwardAtEnd) {
    this->cursor.movePosition(QTextCursor::MoveOperation::End);
    selection::moveToNextWord(this->cursor);
    ASSERT_FALSE(this->cursor.hasSelection());
    EXPECT_EQ(this->cursor.anchor(), this->document.characterCount() - 1);
}

TEST_F(WordMovementTest, shouldMoveForwardBySingleLetterWord) {
    selection::moveToNextWord(this->cursor, QTextCursor::KeepAnchor);
    ASSERT_TRUE(this->cursor.hasSelection());
    EXPECT_EQ(this->cursor.selectedText(), "A");
}

TEST_F(WordMovementTest, shouldMoveBackwardBySingleLetterWordAtStartOfWord) {
    this->cursor.setPosition(2);
    selection::moveToPreviousWord(this->cursor, QTextCursor::KeepAnchor);
    ASSERT_TRUE(this->cursor.hasSelection());
    EXPECT_EQ(this->cursor.selectedText(), "A ");
}

TEST_F(WordMovementTest, shouldMoveForwardByMultiLetterWordAtEndOfPreviousWord) {
    this->cursor.setPosition(1);
    selection::moveToNextWord(this->cursor, QTextCursor::KeepAnchor);
    ASSERT_TRUE(this->cursor.hasSelection());
    EXPECT_EQ(this->cursor.selectedText(), " word");
}

TEST_F(WordMovementTest, shouldMoveBackwardByMultiLetterWordAtEndOfWord) {
    this->cursor.setPosition(6);
    selection::moveToPreviousWord(this->cursor, QTextCursor::KeepAnchor);
    ASSERT_TRUE(this->cursor.hasSelection());
    EXPECT_EQ(this->cursor.selectedText(), "word");
}

TEST_F(WordMovementTest, shouldMoveForwardToBlockEndBeforeFinalPunctuation) {
    this->cursor.setPosition(6);
    selection::moveToNextWord(this->cursor, QTextCursor::KeepAnchor);
    ASSERT_TRUE(this->cursor.hasSelection());
    EXPECT_EQ(this->cursor.selectedText(), ".");
}

TEST_F(WordMovementTest, shouldMoveForwardToNextWordAtBlockEnd) {
    this->cursor.setPosition(7);
    selection::moveToNextWord(this->cursor, QTextCursor::KeepAnchor);
    ASSERT_TRUE(this->cursor.hasSelection());
    EXPECT_EQ(this->cursor.selectedText(), QChar::ParagraphSeparator + "With");
}

TEST_F(WordMovementTest, shouldMoveBackwardToBlockStartFromWordEnd) {
    this->cursor.setPosition(12);
    selection::moveToPreviousWord(this->cursor, QTextCursor::KeepAnchor);
    ASSERT_TRUE(this->cursor.hasSelection());
    EXPECT_EQ(this->cursor.selectedText(), "With");
}

TEST_F(WordMovementTest, shouldMoveBackwardToPreviousWordFromBlockStart) {
    this->cursor.setPosition(8);
    selection::moveToPreviousWord(this->cursor, QTextCursor::KeepAnchor);
    ASSERT_TRUE(this->cursor.hasSelection());
    EXPECT_EQ(this->cursor.selectedText(), "word." + QChar::ParagraphSeparator);
}

TEST_F(WordMovementTest, shouldMoveForwardToSelectAllPeriods) {
    this->cursor.setPosition(22);
    selection::moveToNextWord(this->cursor, QTextCursor::KeepAnchor);
    ASSERT_TRUE(this->cursor.hasSelection());
    EXPECT_EQ(this->cursor.selectedText(), "periods");
    selection::moveToNextWord(this->cursor, QTextCursor::KeepAnchor);
    EXPECT_EQ(this->cursor.selectedText(), "periods...");
    selection::moveToNextWord(this->cursor, QTextCursor::KeepAnchor);
    EXPECT_EQ(this->cursor.selectedText(), "periods..." + QChar::ParagraphSeparator + "Combined");
}

TEST_F(WordMovementTest, shouldMoveBackwardToSelectAllPeriods) {
    this->cursor.setPosition(41);
    selection::moveToPreviousWord(this->cursor, QTextCursor::KeepAnchor);
    ASSERT_TRUE(this->cursor.hasSelection());
    EXPECT_EQ(this->cursor.selectedText(), "Combined");
    selection::moveToPreviousWord(this->cursor, QTextCursor::KeepAnchor);
    EXPECT_EQ(this->cursor.selectedText(), "..." + QChar::ParagraphSeparator + "Combined");
    selection::moveToPreviousWord(this->cursor, QTextCursor::KeepAnchor);
    EXPECT_EQ(this->cursor.selectedText(), "periods..." + QChar::ParagraphSeparator + "Combined");
}

TEST_F(WordMovementTest, shouldMoveForwardToDash) {
    this->cursor.setPosition(49);
    selection::moveToNextWord(this->cursor, QTextCursor::KeepAnchor);
    ASSERT_TRUE(this->cursor.hasSelection());
    EXPECT_EQ(this->cursor.selectedText(), "dash");
    selection::moveToNextWord(this->cursor, QTextCursor::KeepAnchor);
    EXPECT_EQ(this->cursor.selectedText(), "dash-separated");
}

TEST_F(WordMovementTest, shouldMoveBackwardToDash) {
    this->cursor.setPosition(63);
    selection::moveToPreviousWord(this->cursor, QTextCursor::KeepAnchor);
    ASSERT_TRUE(this->cursor.hasSelection());
    EXPECT_EQ(this->cursor.selectedText(), "separated");
    selection::moveToPreviousWord(this->cursor, QTextCursor::KeepAnchor);
    EXPECT_EQ(this->cursor.selectedText(), "dash-separated");
}

TEST_F(WordMovementTest, shouldMoveForwardToQuotedWordAfterBlockEnd) {
    this->cursor.setPosition(97);
    selection::moveToNextWord(this->cursor, QTextCursor::KeepAnchor);
    ASSERT_TRUE(this->cursor.hasSelection());
    EXPECT_EQ(this->cursor.selectedText(), QChar::ParagraphSeparator + "“I");
}

TEST_F(WordMovementTest, shouldMoveBackwardToBeforeStartingQuote) {
    this->cursor.setPosition(101);
    selection::moveToPreviousWord(this->cursor, QTextCursor::KeepAnchor);
    ASSERT_TRUE(this->cursor.hasSelection());
    EXPECT_EQ(this->cursor.selectedText(), "I ");
}

TEST_F(WordMovementTest, shouldMoveBackwardToSelectOnlyStartingQuote) {
    this->cursor.setPosition(99);
    selection::moveToPreviousWord(this->cursor, QTextCursor::KeepAnchor);
    ASSERT_TRUE(this->cursor.hasSelection());
    EXPECT_EQ(this->cursor.selectedText(), "“");
}

TEST_F(WordMovementTest, shouldMoveForwardToSelectEndingQuote) {
    this->cursor.setPosition(105);
    selection::moveToNextWord(this->cursor, QTextCursor::KeepAnchor);
    ASSERT_TRUE(this->cursor.hasSelection());
    EXPECT_EQ(this->cursor.selectedText(), " you");
    selection::moveToNextWord(this->cursor, QTextCursor::KeepAnchor);
    EXPECT_EQ(this->cursor.selectedText(), " you,”");
}
