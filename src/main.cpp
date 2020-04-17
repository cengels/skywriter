#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QRawFont>
#include <QDebug>
#include <QIcon>
#include <QQuickStyle>
#include <QPalette>
#include "formattable_text_area/formattable_text_area.h"
#include "colors.h"

int main(int argc, char *argv[])
{
    qmlRegisterType<FormattableTextArea>("com.skywriter.text", 1, 0, "FormattableTextArea");

    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    // Necessary to avoid flickering when resizing the window.
    QGuiApplication::setAttribute(Qt::AA_UseOpenGLES);
    QGuiApplication::setOrganizationName("cengels");
    QGuiApplication::setOrganizationDomain("www.cengels.com");
    QGuiApplication::setApplicationName("Skywriter");
    QGuiApplication::setApplicationVersion(QString::number(VERSION_MAJOR) + "."
                                      + QString::number(VERSION_MINOR) + "."
                                      + QString::number(VERSION_BUILD));
    const QGuiApplication app(argc, argv);
    QGuiApplication::setWindowIcon(QIcon(":/images/air.png"));
    QGuiApplication::setPalette(Skywriter::palette());

    QQuickStyle::setFallbackStyle("Fusion");

    QFontDatabase::addApplicationFont(":/fonts/Baloo2-Regular.ttf");
    QGuiApplication::setFont(QFont("Baloo 2", 11));

    QQmlApplicationEngine engine(":/qml/MainView.qml");

    return app.exec();
}
