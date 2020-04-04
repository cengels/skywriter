#include <QFont>
#include <QObject>
#include <QTextCursor>
#include <QTextEdit>

#include "formattable_text_area.h"

FormattableTextArea::FormattableTextArea(QObject *parent)
    : DocumentHandler(parent) {
}

void FormattableTextArea::toggleBold() {
    this->setBold(!this->bold());
}
