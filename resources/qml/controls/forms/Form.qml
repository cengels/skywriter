import QtQuick
import QtQuick.Controls

/**
  * A wrapper for Column. The Form will implicitly connect itself to any
  * descendants that possess a [valid] attribute. If all contained valid
  * attributes are true, the form's own [valid] attribute will also be true.
  */
Column {
    id: root
    anchors.fill: parent
    spacing: 15
    readonly property alias valid: internal.valid
    property bool autoInitialize: true

    function initialize() {
        if (internal.bound) {
            throw new Error('Attempted to initialize form, but form was already initialized!');
        }

        internal.bound = true;
        internal.bind(this);
        internal.valid = internal.validatableItems.every(x => x.valid);
    }

    Component.onCompleted: {
        if (autoInitialize) {
            initialize();
        }
    }

    QtObject {
        id: internal
        property var validatableItems: []
        property bool valid
        property bool bound: false

        function bind(target) {
            if (target.valid != null && target !== root) {
                internal.validatableItems.push(target);
                target.validChanged.connect(() => {
                    if (!target.valid) {
                        internal.valid = false;
                    } else {
                        internal.valid = internal.validatableItems.every(x => x.valid);
                    }
                });
            }

            if (target.children != null) {
                for (const i in target.children) {
                    bind(target.children[i]);
                }
            }
        }
    }
}
