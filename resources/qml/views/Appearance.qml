import QtQuick 2.14
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.14
import QtQuick.Window 2.14
import "../controls" as Sky
import "../types" as SkyT

Sky.Dialog {
    title: qsTr("Appearance")
    padding: 5
    width: 450
    height: 270
    minimumWidth: 450
    minimumHeight: 270

    GridView {
        anchors.fill: parent
//        model: ListModel {

//            ListElement {
//                name: "SkyT.DefaultTheme.name"
//            }

//            ListElement {
//                name: "Test"
//            }

//        }

       delegate: Column {
           Rectangle { anchors.horizontalCenter: parent.horizontalCenter }
           Text { text: name; anchors.horizontalCenter: parent.horizontalCenter }
       }
    }
}
