/*
 * File:   RadioActiveIndicator.qml
 * Author: christophe.delondre
 *
 * License: see LICENSE.xivap
 */

import QtQuick 2.0

Item
{
    id: uiRadioActiveIndicator

    property bool active: false

    property int radioIndex: 1

    Canvas
    {
        id: uiRadioActiveIndicatorCanvas

        Component.onCompleted:
        {
            uiRadioActiveIndicator.activeChanged.connect(function() {
                requestPaint();
            });
        }

        anchors.fill: parent

        onPaint:
        {
            var ctx = getContext("2d");

            ctx.reset();

            ctx.fillStyle = uiRadioActiveIndicator.active ? "green" : "white";

            ctx.ellipse(0, 0, uiRadioActiveIndicatorCanvas.width, uiRadioActiveIndicatorCanvas.height);

            ctx.fill();
        }
    }

    MouseArea
    {
        anchors.fill: parent

        onClicked:
        {
            if(uiRadioActiveIndicator.radioIndex !== SimulatorData.p_radioActive)
            {
                MainViewModel.SetActiveRadio(uiRadioActiveIndicator.radioIndex);
            }
        }
    }
}
