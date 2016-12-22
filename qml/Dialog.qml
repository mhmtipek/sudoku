import QtQuick 2.0
import QtGraphicalEffects 1.0
import "Extra" 1.0

Item {
    id: root

    anchors.centerIn: parent

    width: parent.width * 0.9
    height: parent.height * 0.4

    property alias headerVisible: header.visible
    property alias title: titleText.text
    property alias text: bodyLoader.bodyText

    property real shadowWidth: parent.width * 0.018

    property alias body: bodyLoader.sourceComponent

    visible: false

    property bool showButtons: false

    property var buttons: []

    function show() {
        root.visible = true;
    }

    function hide() {
        root.visible = false;
    }

    Rectangle {
        x: -parent.x
        y: -parent.y
        width: parent.parent.width
        height: parent.parent.height

        color: Globals.style.colorPalette.shadowColor
        opacity: 0.4

        MouseArea {
            anchors.fill: parent
            propagateComposedEvents: false
            onPressed: mouse.accepted = true
            onReleased: mouse.accepted = true
            onDoubleClicked: mouse.accepted = true
            onPositionChanged: mouse.accepted = true
            onPressAndHold: mouse.accepted = true
            onClicked: {
                root.visible = false;
                mouse.accepted = true;
            }
        }
    }

    Rectangle {
        id: background

        anchors.fill: parent
        anchors.margins: shadowWidth

        color: Globals.style.colorPalette.backgroundColor

        radius: width * 0.02

        MouseArea {
            anchors.fill: parent
            enabled: root.visible
            preventStealing: true
            onPressed: mouse.accepted = true
            onReleased: mouse.accepted = true
            onClicked: mouse.accepted = true
            onDoubleClicked: mouse.accepted = true
        }
    }

    DropShadow {
        anchors.fill: background

        fast: false
        cached: false
        color: Globals.style.colorPalette.shadowColor
        radius: root.shadowWidth
        source: background
        spread: 0.2
        samples: 32
    }

    Column {
        anchors.centerIn: background

        Item {
            id: header

            width: background.width
            height: width * 0.2

            Text {
                id: titleText

                anchors.verticalCenter: parent.verticalCenter
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.leftMargin: root.width * 0.05
                anchors.rightMargin: root.width * 0.05

                font.pointSize: 32
                font.weight: Font.Black
                color: Globals.style.colorPalette.titleColor
            }

            Rectangle {
                id: headerLine

                anchors.left: parent.left
                anchors.right: parent.right
                anchors.bottom: parent.bottom
                anchors.leftMargin: parent.width * 0.05
                anchors.rightMargin: parent.width * 0.05
                height: parent.height * 0.05

                radius: height / 2
                visible: header.visible
                color: Globals.style.colorPalette.lineColor
            }
        }

        Loader {
            id: bodyLoader

            property int bodyWidth: background.width
            property int bodyHeight: background.height - header.height

            property string bodyText

            sourceComponent: defaultBody
        }
    }

    Component {
        id: defaultBody

        Item {
            width: bodyWidth
            height: bodyHeight

            property int childrenSpacing: height * 0.08

            Text {
                anchors.left: parent.left
                anchors.leftMargin: parent.width * 0.08
                anchors.right: parent.right
                anchors.rightMargin: parent.width * 0.08
                anchors.top: parent.top
                anchors.topMargin: parent.height * 0.08
                height: parent.height - (buttonsRow.height + parent.childrenSpacing)

                horizontalAlignment: Text.AlignLeft
                verticalAlignment: Text.AlignTop

                font.pointSize: 22
                wrapMode: Text.WordWrap
                elide: Text.ElideRight
                color: Globals.style.colorPalette.textColor

                text: bodyLoader.bodyText
            }

            Row {
                id: buttonsRow

                anchors.right: parent.right
                anchors.rightMargin: parent.childrenSpacing
                anchors.bottom: parent.bottom
                anchors.bottomMargin: parent.childrenSpacing

                spacing: parent.childrenSpacing / 2

                Repeater {
                    model: root.buttons
                    delegate: CircularButton {
                        width: root.parent.height * 0.1
                        height: root.parent.height * 0.1

                        imageUrl: root.buttons[index].imageUrl
                        color: Globals.style.colorPalette.darkButtonColor
                        borderColor: Globals.style.colorPalette.darkButtonColor
                        colorWhenPressed: Globals.style.colorPalette.darkButtonColorPressed

                        onClicked: root.buttons[index].onClicked();
                    }
                }
            }
        }
    }
}
