#ifndef FORMATTABLETEXTAREA_H
#define FORMATTABLETEXTAREA_H

#include <QFont>
#include <QObject>
#include <QTextCursor>

#include "document_handler.h"

class FormattableTextArea : public DocumentHandler {
    Q_OBJECT

    public:
        explicit FormattableTextArea(QObject *parent = nullptr);

    public Q_SLOTS:
        void toggleBold();
};

#endif // FORMATTABLETEXTAREA_H
