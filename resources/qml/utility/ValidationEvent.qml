import QtQml 2.14

QtObject {
    property string text: ""
    property bool accepted: true

    function reset() {
        text = "";
        accepted = true;
    }
}
