#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QRawFont>
#include <QDebug>
#include <QIcon>
#include <QQuickStyle>
#include <QPalette>
#include <QQmlContext>
#include "text/FormattableTextArea/FormattableTextArea.h"
#include "progress/ProgressTracker.h"
#include "colors.h"

int main(int argc, char *argv[])
{
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

    qmlRegisterType<FormattableTextArea>("Skywriter.Text", 1, 0, "FormattableTextArea");

    qmlRegisterSingletonType(QUrl("qrc:///qml/types/settings/ApplicationSettings.qml"), "Skywriter.Settings", 1, 0, "Application");
    qmlRegisterSingletonType(QUrl("qrc:///qml/types/settings/DocumentSettings.qml"), "Skywriter.Settings", 1, 0, "Document");
    qmlRegisterSingletonType(QUrl("qrc:///qml/types/settings/WindowSettings.qml"), "Skywriter.Settings", 1, 0, "Window");

    qmlRegisterSingletonType<ProgressTracker>("Skywriter.Progress", 1, 0, "ProgressTracker", [](QQmlEngine *engine, QJSEngine *scriptEngine) -> QObject * {
        Q_UNUSED(engine)
        Q_UNUSED(scriptEngine)

        ProgressTracker *progressTracker = new ProgressTracker();
        return progressTracker;
    });

    QQuickStyle::setFallbackStyle("Fusion");

    QFontDatabase::addApplicationFont(":/fonts/Baloo2-Regular.ttf");
    QGuiApplication::setFont(QFont("Baloo 2", 11));

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("QT_VERSION", qVersion());
    engine.addImportPath(":/");
    engine.load(":/qml/views/MainWindow.qml");

    return app.exec();
}
