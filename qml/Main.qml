import QtQuick
import QtQuick.Controls

ApplicationWindow {
    id: root
    width: 1280
    height: 360
    visible: true
    title: "Maximus"

    // Automotive cluster dark background
    background: Rectangle {
        color: "#0a0e1a"
    }

    Dashboard {
        anchors.fill: parent
    }
}
