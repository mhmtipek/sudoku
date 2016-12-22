import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQml.Models 2.2

Item {
    id: root

    property alias model: pv.model
    property alias delegate: pv.delegate

    property alias dotsVisible: dots.visible

    property int currentIndex

    onCurrentIndexChanged: {
        if (pv._passCurrentIndexChange)
            pv._passCurrentIndexChange = false;
        else
            pv.currentIndex = pv.count - 1 + root.currentIndex;
    }

    property Component dotDelegate: Component {
        Rectangle {
            width: root.width * 0.02
            height: width

            radius: width / 2

            color: root.currentIndex === index ? "#000000"
                                               : "#7f7f7f"
        }
    }

    PathView {
        id: pv

        clip: true
        snapMode: PathView.SnapOneItem
        interactive: model.count > 1
        offset: 1.0

        property bool _passCurrentIndexChange: false

        Component.onCompleted: {
            pv.currentIndex = model.count - 1 + root.currentIndex;

            // Added this line because pv is not drawn on screen when app starts
            pv.anchors.fill = root;

            pv.onCurrentIndexChanged.connect(handleCurrentIndexChanged);
        }

        function handleCurrentIndexChanged() {
            _passCurrentIndexChange = true;
            root.currentIndex = (pv.currentIndex + 1) % pv.count;
        }

        path: Path {
            startX: pv.model.count > 1 ? -pv.width / 2
                                       : pv.width / 2
            startY: pv.height / 2
            PathLine {
                x: pv.model.count > 1 ? pv.model.count * pv.width - pv.width / 2
                                      : pv.width * 1.5;
                y: pv.height / 2
            }
        }
    }

    Row {
        id: dots

        anchors.bottom: parent.bottom
        anchors.bottomMargin: parent.height * 0.13
        anchors.horizontalCenter: parent.horizontalCenter

        spacing: parent.width * 0.025

        Repeater {
            id: dotsRepeater
            model: pv.count
            delegate: dotDelegate
        }
    }
}
