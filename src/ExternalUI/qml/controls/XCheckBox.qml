/*
 * File:   XCheckBox.qml
 * Author: christophe.delondre
 *
 * License: see LICENSE.xivap
 */

import QtQuick 2.6
import QtQuick.Controls 2.2

import "../.." // qmldir

CheckBox
{
    id: uiXCheckBox

    font: ExternalUIStyle.defaultFont
    spacing: 10

    indicator: Rectangle
    {
        anchors.verticalCenter: parent.verticalCenter

        width: 28
        height: 28

        border.width: uiXCheckBox.checked ? 1 : 2
        border.color: "white"

        radius: 2

        color: uiXCheckBox.checked ? "#666666" : "transparent"

        Canvas
        {
            anchors.centerIn: parent

            width: 12
            height: 12

            visible: uiXCheckBox.checked

            onPaint:
            {
                var ctx = getContext("2d");

                ctx.reset();

                ctx.fillStyle = "white";

                ctx.moveTo(0, 0);
                ctx.beginPath();
                ctx.ellipse(0, 0, width, height);
                ctx.closePath();

                ctx.fill();
            }
        }
    }

    contentItem: Text
    {
        anchors.verticalCenter: parent.verticalCenter
        font: ExternalUIStyle.defaultFont
        color: ExternalUIStyle.defaultFontColor
        text: uiXCheckBox.text
        //horizontalAlignment: Text.AlignHCenter
        //verticalAlignment: Text.AlignVCenter
        leftPadding: uiXCheckBox.indicator.width + uiXCheckBox.spacing
    }
}
