#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QRawFont>
#include <QDebug>
#include <QIcon>

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);
    app.setWindowIcon(QIcon(":/images/air.png"));
    app.setOrganizationName("cengels");
    app.setOrganizationDomain("www.cengels.com");
    app.setApplicationName("Skywriter");
    app.setApplicationVersion(QString::number(VERSION_MAJOR) + "."
                                      + QString::number(VERSION_MINOR) + "."
                                      + QString::number(VERSION_BUILD));

    QFontDatabase::addApplicationFont(":/fonts/Baloo2-Regular.ttf");

    app.setFont(QFont("Baloo 2", 11));

    QQmlApplicationEngine engine(":/qml/main.qml");

    return app.exec();
}
