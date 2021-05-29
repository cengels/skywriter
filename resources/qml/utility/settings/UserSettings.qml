pragma Singleton
import "qrc:/qml/utility" as Sky
import QtQml

QtObject {
    id: userSettings

    readonly property var schema: {
        const xhr = new XMLHttpRequest();
        xhr.open("GET", "qrc:/json/user-settings.json", false);
        xhr.send();

        if (xhr.status !== 200) {
            throw new Error('Failed to load settings schema.');
        }

        return JSON.parse(xhr.responseText);
    }

    property var groups: {
        const object = {};

        const typeMap = {
            "boolean": "bool",
            "string": "string",
            "time": "string",
            "integer": "int",
            "enum": "string"
        };

        function createSettingsGroup(key, group) {
            // We could import Sky.Settings as a component using Qt.createComponent()
            // here, but then we wouldn't be able to define additional properties
            // on the created instances.
            // This is important so that consumers can bind to them.
            let properties = "";

            for (const settingKey in group) {
                const setting = group[settingKey];
                properties += "property %1 %2: %3\n".arg(typeMap[setting.type])
                                                    .arg(settingKey)
                                                    .arg(typeof setting.default === 'string' ? "\"%1\"".arg(setting.default.toString()) : setting.default.toString());
            }

            return Qt.createQmlObject("import \"qrc:/qml/utility\" as Sky
                                       Sky.Settings {
                                           category: '%1'
                                           %2
                                       }".arg(key).arg(properties), userSettings, "UserSettings/%1".arg(key));
        }

        for (const key in schema.groups) {
            Object.defineProperty(object, key, {
                value: createSettingsGroup(key, schema.groups[key]),
                writable: false,
                enumerable: true,
                configurable: false
            });
        }

        return object;
    }
}
