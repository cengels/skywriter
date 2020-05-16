pragma Singleton
import ".." as Sky

Sky.Settings {
    id: applicationSettings
    category: "application"
    property string theme: "Default"
    property int maximumProgressIdleMinutes: 5
    property int progressAutosaveMinutes: 5
    property string dailyReset: "06:00:00"
    property int autosaveMinutes: 0
}
