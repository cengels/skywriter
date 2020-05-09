pragma Singleton
import ".." as Sky

Sky.Settings {
    id: applicationSettings
    category: "application"
    property string theme
}
