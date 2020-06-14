import "qrc:/qml/controls/text" as Sky

Sky.Text {
    property int level: 2
    font.letterSpacing: 2
    font.pointSize: {
        switch (level) {
            case 1: return 15;
            case 2: return 14;
            case 3: return 13;
            case 4: return 12;
            case 5: return 11;
            case 6:
            default: return 10;
        }
    }
}
