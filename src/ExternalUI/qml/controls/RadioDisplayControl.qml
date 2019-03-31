/*
 * File:   RadioDisplayControl.qml
 * Author: christophe.delondre
 *
 * License: see LICENSE.xivap
 */

import QtQuick 2.7

import "../.."

import "../../js/PopupManager.js" as PopupManager

XGroupBox
{
    id: uiRadioDisplayControl

    property int radioIndex: 1

    header: "COM " + radioIndex

    contentItem: Item
    {
        anchors.fill: parent

        Column
        {
            id: uiRadioDisplayControlColumn

            anchors.verticalCenter: parent.verticalCenter

            spacing: 5 * ExternalUIStyle.scaleFactor
            leftPadding: 15 * ExternalUIStyle.scaleFactor

            Text
            {
                font: ExternalUIStyle.defaultFontBig

                color: ExternalUIStyle.defaultFontColor

                text: uiRadioDisplayControl.radioIndex == 1 ? SimulatorData.p_com1freq : SimulatorData.p_com2freq
            }

            Text
            {
                font: ExternalUIStyle.defaultFont

                color: ExternalUIStyle.defaultFontColor

                text: uiRadioDisplayControl.radioIndex == 1 ? SimulatorData.p_com1name : SimulatorData.p_com2name
            }
        }

        MouseArea
        {
            anchors.fill: uiRadioDisplayControlColumn

            onClicked:
            {
                var popup = PopupManager.showPopup("QuickFrequencyChange");
                popup.radioIndex = uiRadioDisplayControl.radioIndex;
            }
        }

        RadioActiveIndicator
        {
            anchors.verticalCenter: parent.verticalCenter

            anchors.right: parent.right
            anchors.rightMargin: 20 * ExternalUIStyle.scaleFactor

            width: 28 * ExternalUIStyle.scaleFactor
            height: 28 * ExternalUIStyle.scaleFactor

            active: uiRadioDisplayControl.radioIndex === SimulatorData.p_radioActive

            radioIndex: uiRadioDisplayControl.radioIndex
        }
    }
}
