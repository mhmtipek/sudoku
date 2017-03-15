import QtQuick 2.0
import QtGraphicalEffects 1.0
import "Extra"

Item {
    id: root

    width: parent.width
    height: parent.height

    property alias text: messageText.text

    Timer {
        id: showTimer
        interval: 400
        repeat: false
        onTriggered: showAnimation.start()
    }

    function show() {
        //showTimer.start();
        showAnimation.start();
    }

    function hide() {
        hideAnimation.start();
    }

    NumberAnimation {
        id: showAnimation

        target: root
        property: "opacity"
        duration: 150
        from: 0
        to: 1

        onStarted: root.visible = true
    }

    NumberAnimation {
        id: hideAnimation

        target: root
        property: "opacity"
        duration: 400
        from: 1
        to: 0

        onStopped: root.visible = false
    }

    FastBlur {
        width: root.width
        height: root.height
        source: parent.parent
        radius: root.width * 0.1
        cached: true
        smooth: false
    }

    Rectangle {
        id: messageBox

        anchors.centerIn: parent
        width: root.width * 0.9
        height: root.height * 0.1
        color: Globals.style.colorPalette.backgroundColor
        radius: width * 0.02
        visible: false

        Text {
            id: messageText

            anchors.centerIn: parent

            font.pointSize: 28
            color: Globals.style.colorPalette.textColor
            wrapMode: Text.WordWrap
        }

        NumberAnimation {
            id: showMessageBoxAnimation

            target: messageBox
            property: "opacity"
            duration: 80
            from: 0
            to: 1

            onStarted: messageBox.visible = true
        }
    }

    DropShadow {
        anchors.fill: messageBox

        fast: true
        cached: true
        color: Globals.style.colorPalette.shadowColor
        radius: Globals.style.shadowWidth
        source: messageBox
        spread: 0.2
        samples: 32
        visible: messageBox.visible
    }

    MouseArea {
        anchors.fill: parent
        enabled: root.visible
        preventStealing: true
        onPressed: mouse.accepted = true
        onReleased: mouse.accepted = true
        onClicked: mouse.accepted = true
        onDoubleClicked: mouse.accepted = true
    }

    Timer {
        id: showMessageBoxTimer
        repeat: false
        interval: 150
        onTriggered: showMessageBoxAnimation.start()
    }

    onVisibleChanged: {
        if (visible)
            showMessageBoxTimer.running = true;
    }
}
