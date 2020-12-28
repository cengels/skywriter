#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QRawFont>
#include <QDebug>
#include <QIcon>
#include <QQuickStyle>
#include <QStyleHints>
#include <QPalette>
#include <QQmlContext>
#include <QWindow>
#include <QQuickWindow>
#include "text/FormattableTextArea/FormattableTextArea.h"
#include "progress/ProgressTracker.h"
#include "colors.h"
#include "theming/Theme.h"
#include "theming/ThemeManager.h"
#include "Mouse.h"
#include "ErrorManager.h"
#include "QmlHelper.h"

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

    void registerQmlTypes(QGuiApplication& app)
    {
        qmlRegisterType<FormattableTextArea>("Skywriter.Text", 1, 0, "FormattableTextArea");

        registerSingleton<ProgressTracker>("Skywriter.Progress", 1, 0, "ProgressTracker");

        qmlRegisterSingletonType(QUrl("qrc:///qml/utility/settings/ApplicationSettings.qml"), "Skywriter.Settings", 1, 0, "Application");
        qmlRegisterSingletonType(QUrl("qrc:///qml/utility/settings/DocumentSettings.qml"), "Skywriter.Settings", 1, 0, "Document");
        qmlRegisterSingletonType(QUrl("qrc:///qml/utility/settings/WindowSettings.qml"), "Skywriter.Settings", 1, 0, "Window");
        qmlRegisterSingletonType(QUrl("qrc:///qml/utility/settings/UserSettings.qml"), "Skywriter.Settings", 1, 0, "User");

        ThemeManager::instance()->setParent(&app);
        ErrorManager::instance()->setParent(&app);
        qmlRegisterSingletonInstance("Skywriter.Theming", 1, 0, "ThemeManager", ThemeManager::instance());
        qmlRegisterType<Theme>("Skywriter.Theming", 1, 0, "Theme");
        qmlRegisterSingletonInstance("Skywriter.Errors", 1, 0, "ErrorManager", ErrorManager::instance());
        qRegisterMetaType<QEvent*>("QEvent*");
        qRegisterMetaType<QVector<DocumentSegment*>>("QVector<DocumentSegment*>");
        qRegisterMetaType<const DocumentSegment*>("const DocumentSegment*");
        qRegisterMetaType<Property*>("Property*");
        qRegisterMetaType<QVector<Property*>>("QVector<Property*>");
    }
}

int main(int argc, char *argv[])
{
    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    // Necessary to avoid flickering when resizing the window.
    QGuiApplication::setAttribute(Qt::AA_UseOpenGLES);
    QGuiApplication::setAttribute(Qt::AA_UseSoftwareOpenGL);
    QGuiApplication::setOrganizationName("cengels");
    QGuiApplication::setOrganizationDomain("www.cengels.com");
    QGuiApplication::setApplicationName("Skywriter");
    QGuiApplication::setApplicationVersion(QString::number(VERSION_MAJOR) + "."
                                      + QString::number(VERSION_MINOR) + "."
                                      + QString::number(VERSION_BUILD));
    QGuiApplication app(argc, argv);
    QGuiApplication::setWindowIcon(QIcon(":/images/air.png"));
    QGuiApplication::setPalette(colors::palette());
    QGuiApplication::styleHints()->setMousePressAndHoldInterval(300);

    QQuickStyle::setFallbackStyle("Fusion");

    // enables anti-aliasing for shapes
    QSurfaceFormat format;
    format.setSamples(8);
    QSurfaceFormat::setDefaultFormat(format);

    QFontDatabase::addApplicationFont(":/fonts/Baloo2-Regular.ttf");
    QFont font("Baloo 2", 11);
    font.setHintingPreference(QFont::PreferFullHinting);
    font.setStyleStrategy(QFont::NoAntialias);
    QGuiApplication::setFont(font);

    registerQmlTypes(app);
    Mouse* mouse = new Mouse(&app);
    qmlRegisterSingletonInstance("Skywriter.Events", 1, 0, "Mouse", mouse);

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextObject(new QmlHelper(&app));
    engine.rootContext()->setContextProperty("QT_VERSION", qVersion());
    engine.addImportPath(":/");
    engine.addImportPath("./libs/qtgraphicaleffects");
    engine.addPluginPath("./libs/qtgraphicaleffects");
    engine.load(":/qml/views/MainWindow.qml");

    app.topLevelWindows().first()->installEventFilter(mouse);

    return app.exec();
}
