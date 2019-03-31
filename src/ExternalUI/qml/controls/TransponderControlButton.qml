/*
 * File:   TransponderControlButton.qml
 * Author: christophe.delondre
 *
 * License: see LICENSE.xivap
 */

import QtQuick 2.0

import "../.."

Item
{
    id: uiTransponderControlButton

    property bool xpdrActive: SimulatorData.p_xpdrMode == 1

    // Background
    Rectangle
    {
        anchors.fill: parent

        color: ExternalUIStyle.defaultControlBackground

        Item
        {
            anchors.left: parent.left

            width: parent.width / 2
            height: parent.height

            Text
            {
                anchors.centerIn: parent

                font: ExternalUIStyle.defaultFont

                color: ExternalUIStyle.defaultFontColor

                text: "STBY"
            }
        }

        Rectangle
        {
            anchors.right: parent.right

            width: parent.width / 2
            height: parent.height

            color: "green"

            Text
            {
                anchors.centerIn: parent

                font: ExternalUIStyle.defaultFont

                color: ExternalUIStyle.defaultFontColor

                text: "TX"
            }
        }
    }

    // Sliding Rectangle
    Rectangle
    {
        width: parent.width / 2
        height: parent.height

        x: uiTransponderControlButton.xpdrActive ? 0 : parent.width / 2

        color: "#222222"
    }

    MouseArea
    {
        anchors.fill: parent

        onClicked:
        {
            // Toggle XPDR State in X-IvAp.
            MainViewModel.ToggleTransponderMode();
        }
    }
}
