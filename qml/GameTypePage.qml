import QtQuick 2.0

Rectangle {
    id: root

    color: Qt.lighter("orange")

    signal gameRequested(string type)

    function gameTypeName(type) {
        if (type === "standard")
            return qsTr("Standard");
        if (type === "irregular")
            return qsTr("Irregular");

        return "-";
    }

    Column {
        anchors.centerIn: parent

        Repeater {
            model: GameControl.gameTypes.length

            Text {
                text: GameControl.gameTypes[index]
                font.pointSize: 24
                font.weight: Font.Bold
                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        root.gameRequested(GameControl.gameTypes[index]);
                    }
                }
            }
        }
    }
}

