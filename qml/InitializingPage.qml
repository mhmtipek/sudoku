import QtQuick 2.0
import QtGraphicalEffects 1.0
import "Extra"

Item {
    id: root

    width: parent.width
    height: parent.height

    signal finished()

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

    Component.onCompleted: {
        blurSource.sourceItem = parent;
        GameControl.creatingInitialTableFinished.connect(function() {
            root.hide();
        });
    }

    ShaderEffectSource {
        id: blurSource
        width: parent.width
        height: parent.height
    }

    GaussianBlur {
        width: root.width
        height: root.height

        source: blurSource
        radius: root.width * 0.1
        samples: root.width * 0.2
        cached: true
    }

    Rectangle {
        id: messageBox

        anchors.centerIn: parent
        width: root.width * 0.9
        height: root.height * 0.1
        visible: false
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

        Text {
            anchors.centerIn: parent

            text: GameControl.initialTableCreationProgressText + "..."
            font.pointSize: 28
            font.weight: Font.Bold
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

        NumberAnimation {
            target: messageBox
            property: "opacity"
            duration: 200
            from: 1
            to: 0

            onStopped: messageBox.visible = false
        }

        DropShadow {
            anchors.fill: messageBox
            anchors.margins: -parent.height * 0.2

            fast: false
            cached: false
            color: Globals.style.colorPalette.shadowColor
            radius: root.width * 0.018
            source: messageBox
            spread: 0.2
            samples: 32
        }
    }

    Timer {
        id: showMessageBoxTimer
        repeat: false
        interval: 50
        onTriggered: showMessageBoxAnimation.start()
    }

    onVisibleChanged: {
        if (visible)
            showMessageBoxTimer.running = true;
    }
}


