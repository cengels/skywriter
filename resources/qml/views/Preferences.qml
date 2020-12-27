import QtQuick 2.14
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.14
import "qrc:/qml/controls" as Sky
import "qrc:/qml/controls/dialog" as Sky
import "qrc:/qml/controls/forms" as Sky
import "qrc:/qml/controls/text" as Sky
import "qrc:/qml/controls/menu" as Sky
import Skywriter.Settings 1.0 as Settings

Sky.Dialog {
    id: root
    title: qsTr("Preferences")
    padding: 5
    width: 800
    height: 600
    minimumWidth: 450
    minimumHeight: 270
    standardButtons: DialogButtonBox.Ok | DialogButtonBox.Cancel

    onAccepted: {
        // ...
    }

    Component {
        id: settingDelegate

        Item {
            id: row
            anchors.leftMargin: 12
            anchors.rightMargin: 12
            anchors.left: parent.left
            anchors.right: parent.right
            height: column.height

            Column {
                id: column
                width: row.width - settingItem.width - 20
                spacing: 5

                Sky.Text {
                    width: parent.width
                    text: setting.name
                    font.pointSize: 12
                }

                Sky.Text {
                    id: description
                    width: parent.width
                    text: setting.description
                    wrapMode: Text.WordWrap
                    color: Qt.darker(palette.text, 1.25)
                }
            }

            Item {
                id: settingItem
                anchors.right: parent.right
                y: description.y
                width: 200

                Component {
                    id: booleanSetting

                    Sky.Switch {
                        objectName: setting.id
                        anchors.right: parent.right
                    }
                }

                Component {
                    id: integerSetting

                    Sky.NumberField {
                        objectName: setting.id
                        width: settingItem.width
                        min: setting.minimum
                        max: setting.maximum
                        clamp: true
                    }
                }

                Component {
                    id: enumSetting

                    Sky.ComboBox {
                        id: comboBox
                        objectName: setting.id
                        width: settingItem.width
                        items: setting.options
                        popupWidth: 350
                        textRole: "name"
                        valueRole: "value"
                        popupContent.footer: Sky.Text {
                            bottomPadding: 8
                            topPadding: 4
                            leftPadding: 6
                            rightPadding: 6
                            anchors.left: parent.left
                            anchors.right: parent.right
                            text: highlightedIndex !== -1 ? setting.options[highlightedIndex].description : null
                        }
                    }
                }

                Component {
                    id: timeSetting

                    Sky.TextField {
                        objectName: setting.id
                        width: settingItem.width

                        valid: {
                            const numbers = text.split(':')
                                .filter(x => x.length === 2)
                                .map(x => parseInt(x))
                                .filter(x => !Number.isNaN(x) && x >= 0 && x < 60);

                            return numbers.length === 3 && numbers[0] < 24;
                        }
                    }
                }
            }

            Component.onCompleted: {
                switch (setting.type) {
                    case "boolean": booleanSetting.createObject(settingItem); break;
                    case "integer": integerSetting.createObject(settingItem); break;
                    case "enum": enumSetting.createObject(settingItem); break;
                    case "time": timeSetting.createObject(settingItem); break;
                }
            }
        }
    }

    Component {
        id: groupDelegate

        ListView {
            property string categoryId
            property string name
            property var settings
            delegate: settingDelegate
            Layout.fillWidth: true
            Layout.fillHeight: true
            boundsBehavior: Flickable.StopAtBounds
            flickDeceleration: 2000
            ScrollBar.vertical: ScrollBar {}
            spacing: 10

            model: ListModel {
                id: settingModel
            }

            Component.onCompleted: {
                for (const settingKey in settings) {
                    const setting = Object.assign({}, settings[settingKey]);
                    setting.id = settingKey;
                    settingModel.append({ setting: setting });
                }
            }
        }
    }

    Sky.Form {
        id: form
        anchors.fill: parent
        property var categories: []

        Sky.TabBar {
            id: tabBar
            anchors.left: parent.left
            anchors.right: parent.right
            contentChildren: form.categories.map(category => tabDelegate.createObject(tabBar, { name: category.name }))

            Component {
                id: tabDelegate

                Sky.TabButton {
                    property string name
                    text: qsTr(name)
                }
            }
        }

        StackLayout {
            id: stackLayout
            currentIndex: tabBar.currentIndex
            anchors.left: parent.left
            anchors.right: parent.right
            height: form.height - tabBar.height
            children: form.categories.map(category => groupDelegate.createObject(stackLayout, category))
        }

        Component.onCompleted: {
            for (const groupKey in Settings.User.schema.groups) {
                form.categories.push({ categoryId: groupKey, name: groupKey[0].toUpperCase() + groupKey.slice(1), settings: Settings.User.schema.groups[groupKey] });
            }

            form.categoriesChanged();
        }
    }
}
