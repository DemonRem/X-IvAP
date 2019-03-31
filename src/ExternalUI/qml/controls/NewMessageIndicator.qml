/*
 * File:   NewMessageIndicator.qml
 * Author: christophe.delondre
 *
 * License: see LICENSE.xivap
 */

import QtQuick 2.0

Item
{
    id: uiNewMessageIndicator

    property bool active: false

    Canvas
    {
        id: uiNewMessageIndicatorCanvas

        anchors.fill: parent

        onPaint:
        {
            var ctx = getContext("2d");

            ctx.reset();

            ctx.fillStyle = uiNewMessageIndicator.active ? "cyan" : "white";

            ctx.ellipse(0, 0, uiNewMessageIndicatorCanvas.width, uiNewMessageIndicatorCanvas.height);

            ctx.fill();
        }
    }
}
