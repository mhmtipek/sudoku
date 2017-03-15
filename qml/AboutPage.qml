import QtQuick 2.0
import "Extra"

Rectangle {
    id: root

    signal backRequested

    color: "#B2DFDB"

    property int margin: width * 0.05

    Text {
        id: authorTitleText

        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.margins: root.margin

        text: qsTr("Author")

        font.pointSize: 24
        font.weight: Font.Bold
        color: Globals.style.colorPalette.titleColor
    }

    Keys.onBackPressed: {
        root.backRequested();
    }

    Flow {
        id: authorContent

        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: authorTitleText.bottom
        anchors.margins: root.margin

        Text {
            text: qsTr("This app is developed by Mehmet Ipek.")

            width: parent.width
            font.pointSize: 14
            color: Globals.style.colorPalette.textColor
            wrapMode: Text.WordWrap
        }

        Text {
            text: qsTr("See:") + " <a href='mhmtipek.com'>mhmtipek.com</a>"
            font.pointSize: 14
            linkColor: Globals.style.colorPalette.linkColor
        }

        Text {
            width: parent.width

            text: "<br>" + qsTr("Source code is hosted at github and licensed under MIT.")
            font.pointSize: 14
            wrapMode: Text.WordWrap
        }

        Text {
            text: qsTr("See:") + " <a href='https://github.com/mhmtipek'>https://github.com/mhmtipek</a>"
            font.pointSize: 14
            linkColor: Globals.style.colorPalette.linkColor
        }
    }

    Text {
        id: imagesTitleText

        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: authorContent.bottom
        anchors.margins: root.margin

        text: qsTr("Images")

        font.pointSize: 24
        font.weight: Font.Bold
        color: Globals.style.colorPalette.titleColor
    }

    Flow {
        id: imagesContent

        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: imagesTitleText.bottom
        anchors.margins: root.margin

        Text {
            text: qsTr("Images used in this app are provided by resources below. I thank all of them for licensing their work free use. "
                        + "I made some changes on these images. For example: Changing color or rounding corners.")

            width: parent.width
            font.pointSize: 14
            color: Globals.style.colorPalette.textColor
            wrapMode: Text.WordWrap
        }

        Text {
            text: qsTr("See:") + " <a href='https://www.iconfinder.com/yanlu'>https://www.iconfinder.com/yanlu</a>"
            font.pointSize: 14
            linkColor: Globals.style.colorPalette.linkColor
        }

        Text {
            text: qsTr("See:") + " <a href='https://www.iconfinder.com/tmthymllr'>https://www.iconfinder.com/tmthymllr</a>"
            font.pointSize: 14
            linkColor: Globals.style.colorPalette.linkColor
        }

        Text {
            text: qsTr("See:") + " <a href='https://www.iconfinder.com/nastu_bol'>https://www.iconfinder.com/nastu_bol</a>"
            font.pointSize: 14
            linkColor: Globals.style.colorPalette.linkColor
        }

        Text {
            text: qsTr("See:") + " <a href='https://design.google.com/'>https://design.google.com/</a>"
            font.pointSize: 14
            linkColor: Globals.style.colorPalette.linkColor
            wrapMode: Text.WrapAnywhere
        }

        Text {
            text: qsTr("See:") + " <a href='http://www.wpzoom.com/'>http://www.wpzoom.com/</a>"
            font.pointSize: 14
            linkColor: Globals.style.colorPalette.linkColor
        }
    }

    CircularButton {
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.margins: root.width * 0.05

        width: root.width * 0.18
        height: width

        imageUrl: "qrc:/back.png"

        onClicked: {
            root.backRequested();
        }
    }
}
