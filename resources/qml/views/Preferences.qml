import QtQuick 2.14
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.14
import "qrc:/qml/controls" as Sky
import "qrc:/qml/controls/dialog" as Sky
import "qrc:/qml/controls/forms" as Sky
import "qrc:/qml/controls/text" as Sky
import "qrc:/qml/controls/menu" as Sky
import Skywriter.Settings 1.0 as Settings

// Note that we cannot use ListViews in this file or the form
// will not be able to bind itself to the fields.

Sky.Dialog {
    id: root
    title: qsTr("Preferences")
    padding: 5
    width: 800
    height: 600
    minimumWidth: 450
    minimumHeight: 270
    canAccept: form.valid
    standardButtons: DialogButtonBox.Ok | DialogButtonBox.Cancel

    signal submit

    onAccepted: {
        // If we attached the fields to the accepted signal directly,
        // this would get called first before the fields.
        submit();
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
            property var setting
            property string category

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
                        id: toggle
                        anchors.right: parent.right

                        Connections {
                            target: root
                            onShown: toggle.checked = Settings.User.groups[row.category][setting.id]
                            onSubmit: Settings.User.groups[row.category][setting.id] = toggle.checked
                        }
                    }
                }

                Component {
                    id: integerSetting

                    Sky.NumberField {
                        id: numberField
                        width: settingItem.width
                        min: setting.minimum
                        max: setting.maximum
                        clamp: true

                        Connections {
                            target: root
                            onShown: numberField.text = Settings.User.groups[row.category][setting.id]
                            onSubmit: Settings.User.groups[row.category][setting.id] = numberField.value
                        }
                    }
                }

                Component {
                    id: enumSetting

                    Sky.ComboBox {
                        id: comboBox
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

                        Connections {
                            target: root
                            onShown: comboBox.currentValue = Settings.User.groups[row.category][setting.id]
                            onSubmit: Settings.User.groups[row.category][setting.id] = comboBox.currentValue
                        }
                    }
                }

                Component {
                    id: timeSetting

                    Sky.TextField {
                        id: timeField
                        width: settingItem.width

                        valid: {
                            const numbers = text.split(':')
                                .filter(x => x.length === 2)
                                .map(x => parseInt(x))
                                .filter(x => !Number.isNaN(x) && x >= 0 && x < 60);

                            return numbers.length === 3 && numbers[0] < 24;
                        }

                        Connections {
                            target: root
                            onShown: timeField.text = Settings.User.groups[row.category][setting.id]
                            onSubmit: Settings.User.groups[row.category][setting.id] = timeField.text
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

        Flickable {
            Layout.fillWidth: true
            Layout.fillHeight: true
            boundsBehavior: Flickable.StopAtBounds
            flickDeceleration: 2000
            ScrollBar.vertical: ScrollBar {}
            property string categoryId
            property string name
            property var settings

            Column {
                id: settingsList
                anchors.fill: parent
                spacing: 10

                Component.onCompleted: {
                    for (const settingKey in settings) {
                        const setting = Object.assign({}, settings[settingKey]);
                        setting.id = settingKey;
                        settingDelegate.createObject(settingsList, { setting: setting, category: categoryId });
                    }
                }
            }

            Connections {
                target: root
                onAccepted: {
                    Settings.User.groups[categoryId].sync();
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

            Component.onCompleted: {
                for (const groupKey in Settings.User.schema.groups) {
                    form.categories.push({ categoryId: groupKey, name: groupKey[0].toUpperCase() + groupKey.slice(1), settings: Settings.User.schema.groups[groupKey] });
                }

                form.categoriesChanged();
            }
        }
    }
}
