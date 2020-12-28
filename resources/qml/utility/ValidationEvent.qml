import QtQml

QtObject {
    property string text: ""
    property bool accepted: true

    function reset() {
        text = "";
        accepted = true;
    }
}
