pragma Singleton
import "qrc:/qml/utility" as Sky

Sky.Settings {
    id: applicationSettings
    category: "application"
    property string theme: "Default (Sky)"
    property int maximumProgressIdleMinutes: 5
    property int autosaveMinutes: 0
    property string dailyReset: "06:00:00"
    property bool keepScrollWhenTyping: false
    property bool searchImmediately: false
    property bool keepBackup: true
}
