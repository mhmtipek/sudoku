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

    property alias body: bodyLoader.sourceComponent

    visible: false

    property bool showButtons: false

    property var buttons: []

    function show() {
        showAnimation.start();
    }

    function hide() {
        hideAnimation.start();
    }

    NumberAnimation {
        id: showAnimation

        target: root
        property: "opacity"
        duration: 90
        from: 0
        to: 1

        onStarted: root.visible = true
    }

    NumberAnimation {
        id: hideAnimation

        target: root
        property: "opacity"
        duration: 150
        from: 1
        to: 0

        onStopped: root.visible = false
    }

    MouseArea {
        anchors.fill: blurEffect
        onClicked: root.hide()
    }

    FastBlur {
        id: blurEffect

        x: -parent.x
        y: -parent.y
        width: root.parent.width
        height: root.parent.height
        source: parent.parent
        radius: root.parent.width * 0.1
        cached: true
        smooth: false
    }

    Rectangle {
        id: background

        anchors.fill: parent
        anchors.margins: Globals.style.shadowWidth

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
        radius: Globals.style.shadowWidth
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
                        width: root.parent.height * 0.105
                        height: root.parent.height * 0.105

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
