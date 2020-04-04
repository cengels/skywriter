#include <QGuiApplication>
#include <QApplication>
#include <QQmlApplicationEngine>
#include <QRawFont>
#include <QDebug>
#include <QIcon>

#include "formattable_text_area.h"

int main(int argc, char *argv[])
{
    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication::setOrganizationName("cengels");
    QGuiApplication::setOrganizationDomain("www.cengels.com");
    QGuiApplication::setApplicationName("Skywriter");
    QGuiApplication::setApplicationVersion(QString::number(VERSION_MAJOR) + "."
                                      + QString::number(VERSION_MINOR) + "."
                                      + QString::number(VERSION_BUILD));
    QApplication app(argc, argv);
    QGuiApplication::setWindowIcon(QIcon(":/images/air.png"));

    qmlRegisterType<FormattableTextArea>("com.skywriter.text", 1, 0, "FormattableTextArea");

    QFontDatabase::addApplicationFont(":/fonts/Baloo2-Regular.ttf");
    QApplication::setFont(QFont("Baloo 2", 11));

    QQmlApplicationEngine engine(":/qml/main_text_area.qml");

    return app.exec();
}
