/*
 * File:   SettingsPopup.qml
 * Author: christophe.delondre
 *
 * License: see LICENSE.xivap
 */

import QtQuick 2.7

import "../controls"

import "../.." // qmldir

PopupBase
{
    id: uiSettingsPopup

    width: 300 * ExternalUIStyle.scaleFactor
    height: 300 * ExternalUIStyle.scaleFactor

    popupTitle: "SETTINGS"

    popupContentItem: Item
    {
        anchors.fill: parent

        Column
        {
            anchors.fill: parent

            anchors.margins: 20 * ExternalUIStyle.scaleFactor

            spacing: 20 * ExternalUIStyle.scaleFactor

            XTextButton
            {
                width: parent.width
                height: 50 * ExternalUIStyle.scaleFactor

                text: "Disconnect from X-IvAp"

                onClicked:
                {
                    ConnectViewModel.Disconnect();
                    uiSettingsPopup.close();
                }
            }

            XCheckBox
            {
                visible: SimulatorData.p_softwareBranch == "IVAO"

                text: "Use IVAO Weather"

                checked: SimulatorData.p_ivaoUseWeather

                onCheckedChanged:
                {
                    if(SimulatorData.p_ivaoUseWeather !== checked)
                    {
                        MainViewModel.SetIVAOUseWeather(checked);
                    }
                }
            }

            XCheckBox
            {
                text: "Tagging (aircraft labels)"

                checked: SimulatorData.p_tagging

                onCheckedChanged:
                {
                    if(SimulatorData.p_tagging !== checked)
                    {
                        MainViewModel.SetTagging(checked);
                    }
                }
            }
        }
    }
}
