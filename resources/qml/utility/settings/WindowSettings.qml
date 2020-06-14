pragma Singleton
import ".." as Sky

Sky.Settings {
    id: windowSettings
    category: "window"
    property int x
    property int y
    property int width
    property int height
    property int visibility
}
