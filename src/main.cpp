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
#include "theming/Theme.h"
#include "theming/ThemeManager.h"

namespace {
    template <typename T>
    void registerSingleton(const char *uri, int versionMajor, int versionMinor, const char *qmlName) {
        qmlRegisterSingletonType<T>(uri, versionMajor, versionMinor, qmlName, [](QQmlEngine *engine, QJSEngine *scriptEngine) -> QObject * {
            Q_UNUSED(engine)
            Q_UNUSED(scriptEngine)

            T *singleton = new T();
            return singleton;
        });
    }

    void registerQmlTypes()
    {
        qmlRegisterType<FormattableTextArea>("Skywriter.Text", 1, 0, "FormattableTextArea");

        registerSingleton<ProgressTracker>("Skywriter.Progress", 1, 0, "ProgressTracker");

        qmlRegisterType<Theme>("Skywriter.Theming", 1, 0, "Theme");
        registerSingleton<ThemeManager>("Skywriter.Theming", 1, 0, "ThemeManager");

        qmlRegisterSingletonType(QUrl("qrc:///qml/types/settings/ApplicationSettings.qml"), "Skywriter.Settings", 1, 0, "Application");
        qmlRegisterSingletonType(QUrl("qrc:///qml/types/settings/DocumentSettings.qml"), "Skywriter.Settings", 1, 0, "Document");
        qmlRegisterSingletonType(QUrl("qrc:///qml/types/settings/WindowSettings.qml"), "Skywriter.Settings", 1, 0, "Window");
    }
}

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

    registerQmlTypes();

    QQuickStyle::setFallbackStyle("Fusion");

    QFontDatabase::addApplicationFont(":/fonts/Baloo2-Regular.ttf");
    QFont font("Baloo 2", 11);
    font.setStyleStrategy(QFont::PreferAntialias);
    QGuiApplication::setFont(font);

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("QT_VERSION", qVersion());
    engine.addImportPath(":/");
    engine.load(":/qml/views/MainWindow.qml");

    return app.exec();
}
