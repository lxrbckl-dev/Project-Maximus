import QtQuick
import QtQuick.Layouts
import QtQuick.Shapes

Item {
    id: root
    anchors.fill: parent

    // ── Arc gauge helper function ─────────────────────────────────────────────
    // Converts a value [0..1] fraction into (startAngle, sweepAngle) for a
    // PathArc. We draw a bottom-centred arc from 210° to 330° (240° sweep).
    // Angles in QML PathArc are clockwise from the positive X-axis.

    // Arc constants
    readonly property real arcStartDeg: 210   // start at lower-left
    readonly property real arcTotalDeg: 240   // full sweep

    function arcSweep(fraction) {
        return fraction * arcTotalDeg;
    }

    // Polar → Cartesian (angle in degrees, measured CW from positive X)
    function arcPt(cx, cy, r, angleDeg) {
        var rad = (angleDeg - 90) * Math.PI / 180; // QML arcs start at top
        return Qt.point(cx + r * Math.sin(rad), cy - r * Math.cos(rad));
    }

    // ────────────────────────────────────────────────────────────────────────
    // Layout: five panels side by side, speedometer slightly wider on the left
    RowLayout {
        anchors.fill: parent
        anchors.margins: 8
        spacing: 8

        // ── 1. Speedometer ───────────────────────────────────────────────
        Item {
            Layout.fillHeight: true
            Layout.preferredWidth: parent.width * 0.26
            Layout.maximumWidth: parent.width * 0.26

            // Arc gauge: 0–180 mph, 240° sweep
            Shape {
                anchors.fill: parent

                // Background arc (dark grey track)
                ShapePath {
                    strokeColor: "#1e2535"
                    strokeWidth: 14
                    fillColor: "transparent"
                    capStyle: ShapePath.RoundCap

                    property real cx: parent.width / 2
                    property real cy: parent.height * 0.55
                    property real r:  Math.min(parent.width, parent.height) * 0.38

                    property var startPt: arcPt(cx, cy, r, arcStartDeg)
                    property var endPt:   arcPt(cx, cy, r, arcStartDeg + arcTotalDeg)

                    startX: startPt.x; startY: startPt.y
                    PathArc {
                        x: parent.endPt.x; y: parent.endPt.y
                        radiusX: parent.r; radiusY: parent.r
                        direction: PathArc.Clockwise
                        useLargeArc: true
                    }
                }

                // Active arc (cyan)
                ShapePath {
                    strokeColor: "#00d4ff"
                    strokeWidth: 14
                    fillColor: "transparent"
                    capStyle: ShapePath.RoundCap

                    property real fraction: vehicleData.vehicleSpeedMph / 180.0
                    property real cx: parent.width / 2
                    property real cy: parent.height * 0.55
                    property real r:  Math.min(parent.width, parent.height) * 0.38

                    property var startPt: arcPt(cx, cy, r, arcStartDeg)
                    property var endPt:   arcPt(cx, cy, r,
                                               arcStartDeg + arcSweep(fraction))

                    startX: startPt.x; startY: startPt.y
                    PathArc {
                        x: parent.endPt.x; y: parent.endPt.y
                        radiusX: parent.r; radiusY: parent.r
                        direction: PathArc.Clockwise
                        useLargeArc: parent.fraction > 0.5
                    }
                }
            }

            // Numeric readout
            Column {
                anchors.centerIn: parent
                anchors.verticalCenterOffset: parent.height * 0.05
                spacing: 2

                Text {
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: Math.round(vehicleData.vehicleSpeedMph)
                    font.pixelSize: 52
                    font.bold: true
                    font.family: "Helvetica Neue"
                    color: "#ffffff"
                }
                Text {
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: "MPH"
                    font.pixelSize: 14
                    font.letterSpacing: 3
                    color: "#7a8bab"
                }
            }
        }

        // ── 2. RPM gauge ─────────────────────────────────────────────────
        Item {
            Layout.fillHeight: true
            Layout.fillWidth: true

            Shape {
                anchors.fill: parent

                // Background track
                ShapePath {
                    strokeColor: "#1e2535"
                    strokeWidth: 12
                    fillColor: "transparent"
                    capStyle: ShapePath.RoundCap

                    property real cx: parent.width / 2
                    property real cy: parent.height * 0.55
                    property real r:  Math.min(parent.width, parent.height) * 0.38

                    property var startPt: arcPt(cx, cy, r, arcStartDeg)
                    property var endPt:   arcPt(cx, cy, r, arcStartDeg + arcTotalDeg)

                    startX: startPt.x; startY: startPt.y
                    PathArc {
                        x: parent.endPt.x; y: parent.endPt.y
                        radiusX: parent.r; radiusY: parent.r
                        direction: PathArc.Clockwise
                        useLargeArc: true
                    }
                }

                // Normal zone (white-ish, up to 6500 RPM)
                ShapePath {
                    strokeColor: "#c8d8ff"
                    strokeWidth: 12
                    fillColor: "transparent"
                    capStyle: ShapePath.RoundCap

                    property real fraction: Math.min(vehicleData.engineRpm, 6500.0) / 8000.0
                    property real cx: parent.width / 2
                    property real cy: parent.height * 0.55
                    property real r:  Math.min(parent.width, parent.height) * 0.38

                    property var startPt: arcPt(cx, cy, r, arcStartDeg)
                    property var endPt:   arcPt(cx, cy, r,
                                               arcStartDeg + arcSweep(fraction))

                    startX: startPt.x; startY: startPt.y
                    PathArc {
                        x: parent.endPt.x; y: parent.endPt.y
                        radiusX: parent.r; radiusY: parent.r
                        direction: PathArc.Clockwise
                        useLargeArc: parent.fraction > 0.5
                    }
                }

                // Redline zone overlay (red, 6500–8000 RPM)
                ShapePath {
                    strokeColor: "#ff2a2a"
                    strokeWidth: 12
                    fillColor: "transparent"
                    capStyle: ShapePath.RoundCap
                    visible: vehicleData.engineRpm > 6500.0

                    property real redlineFraction: (vehicleData.engineRpm - 6500.0) / 8000.0
                    property real redlineStart: 6500.0 / 8000.0
                    property real cx: parent.width / 2
                    property real cy: parent.height * 0.55
                    property real r:  Math.min(parent.width, parent.height) * 0.38

                    property var startPt: arcPt(cx, cy, r,
                                               arcStartDeg + arcSweep(redlineStart))
                    property var endPt:   arcPt(cx, cy, r,
                                               arcStartDeg + arcSweep(redlineStart + redlineFraction))

                    startX: startPt.x; startY: startPt.y
                    PathArc {
                        x: parent.endPt.x; y: parent.endPt.y
                        radiusX: parent.r; radiusY: parent.r
                        direction: PathArc.Clockwise
                        useLargeArc: false
                    }
                }
            }

            Column {
                anchors.centerIn: parent
                anchors.verticalCenterOffset: parent.height * 0.05
                spacing: 2

                Text {
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: Math.round(vehicleData.engineRpm / 100) * 100
                    font.pixelSize: 36
                    font.bold: true
                    color: vehicleData.engineRpm > 6500 ? "#ff2a2a" : "#ffffff"
                }
                Text {
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: "RPM"
                    font.pixelSize: 12
                    font.letterSpacing: 3
                    color: "#7a8bab"
                }
            }
        }

        // ── 3. Coolant temp (vertical bar) ───────────────────────────────
        Item {
            Layout.fillHeight: true
            Layout.fillWidth: true

            Column {
                anchors.centerIn: parent
                spacing: 6
                width: parent.width * 0.6

                // Bar
                Rectangle {
                    width: parent.width
                    height: parent.parent.height * 0.55
                    color: "#111827"
                    radius: 6
                    clip: true

                    // Fill rect (grows from bottom)
                    Rectangle {
                        anchors.bottom: parent.bottom
                        width: parent.width
                        height: parent.height * Math.max(0, (vehicleData.coolantTempF - 70.0) / (250.0 - 70.0))
                        radius: 6
                        color: {
                            var t = vehicleData.coolantTempF;
                            if (t > 215.0) return "#ff4040";
                            if (t > 160.0) return "#e8eeff";
                            return "#4da6ff";
                        }
                        Behavior on height { NumberAnimation { duration: 80 } }
                        Behavior on color  { ColorAnimation  { duration: 200 } }
                    }
                }

                Text {
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: "COOLANT"
                    font.pixelSize: 10
                    font.letterSpacing: 2
                    color: "#7a8bab"
                }
                Text {
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: Math.round(vehicleData.coolantTempF) + "°F"
                    font.pixelSize: 20
                    font.bold: true
                    color: vehicleData.coolantTempF > 215.0 ? "#ff4040" : "#ffffff"
                }
            }
        }

        // ── 4. Throttle position (vertical bar) ──────────────────────────
        Item {
            Layout.fillHeight: true
            Layout.fillWidth: true

            Column {
                anchors.centerIn: parent
                spacing: 6
                width: parent.width * 0.6

                Rectangle {
                    width: parent.width
                    height: parent.parent.height * 0.55
                    color: "#111827"
                    radius: 6
                    clip: true

                    Rectangle {
                        anchors.bottom: parent.bottom
                        width: parent.width
                        height: parent.height * vehicleData.throttlePosition
                        radius: 6
                        color: "#ffb84d"
                        Behavior on height { NumberAnimation { duration: 40 } }
                    }
                }

                Text {
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: "THROTTLE"
                    font.pixelSize: 10
                    font.letterSpacing: 2
                    color: "#7a8bab"
                }
                Text {
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: Math.round(vehicleData.throttlePosition * 100) + "%"
                    font.pixelSize: 20
                    font.bold: true
                    color: "#ffffff"
                }
            }
        }

        // ── 5. Fuel level (vertical bar) ─────────────────────────────────
        Item {
            Layout.fillHeight: true
            Layout.fillWidth: true

            Column {
                anchors.centerIn: parent
                spacing: 6
                width: parent.width * 0.6

                Rectangle {
                    width: parent.width
                    height: parent.parent.height * 0.55
                    color: "#111827"
                    radius: 6
                    clip: true

                    Rectangle {
                        anchors.bottom: parent.bottom
                        width: parent.width
                        height: parent.height * vehicleData.fuelLevel
                        radius: 6
                        color: {
                            var f = vehicleData.fuelLevel;
                            if (f < 0.10) return "#ff3333";
                            if (f < 0.25) return "#ffb84d";
                            return "#e8eeff";
                        }
                        Behavior on height { NumberAnimation { duration: 200 } }
                        Behavior on color  { ColorAnimation  { duration: 300 } }
                    }
                }

                Text {
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: "FUEL"
                    font.pixelSize: 10
                    font.letterSpacing: 2
                    color: "#7a8bab"
                }
                Text {
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: Math.round(vehicleData.fuelLevel * 100) + "%"
                    font.pixelSize: 20
                    font.bold: true
                    color: vehicleData.fuelLevel < 0.10 ? "#ff3333" : "#ffffff"
                }
            }
        }
    }
}
