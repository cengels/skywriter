pragma Singleton
import "qrc:/qml/utility" as Sky

Sky.Settings {
    id: documentSettings
    category: "document"
    property string lastFile
    property int caretPosition
    property string sceneBreak: "# # #"
    property var autoReplacements: [
        ["\"", "“", "”"],
        ["'", "‘", "’"]
    ]
}
