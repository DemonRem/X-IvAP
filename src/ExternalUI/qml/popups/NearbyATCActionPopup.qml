/*
 * File:   NearbyATCActionPopup.qml
 * Author: christophe.delondre
 *
 * License: see LICENSE.xivap
 */

import QtQuick 2.7
import QtQuick.Controls 2.2

import "../.." // qmldir

Popup
{
    id: uiNearbyATCActionPopup

    width: 150 * ExternalUIStyle.scaleFactor
    height: 150 * ExternalUIStyle.scaleFactor

    margins: 0
    padding: 0

    property string stationId: ""
    property string stationFreq: ""

    property var nearbyATCPopup: null

    Item
    {
        anchors.fill: parent

        ListModel
        {
            id: uiNearbyATCActionListModel

            ListElement
            {
                actionId: "MESSAGE"
                actionName: "MESSAGE"
            }

            ListElement
            {
                actionId: "TUNE_1"
                actionName: "TUNE COM 1"
            }

            ListElement
            {
                actionId: "TUNE_2"
                actionName: "TUNE COM 2"
            }
        }

        ListView
        {
            id: uiNearbyATCActionListView

            anchors.fill: parent

            clip: true

            model: uiNearbyATCActionListModel

            delegate: Rectangle
            {
                width: uiNearbyATCActionListView.width
                height: 50 * ExternalUIStyle.scaleFactor

                color: "#222222"

                Text
                {
                    anchors.centerIn: parent

                    font: ExternalUIStyle.defaultFont
                    color: ExternalUIStyle.defaultFontColor

                    text: model.actionName
                }

                MouseArea
                {
                    anchors.fill: parent

                    onClicked:
                    {
                        if(model.actionId === "TUNE_1")
                        {
                            MainViewModel.TuneRadioToFrequency(1, stationFreq);
                        }
                        else if(model.actionId === "TUNE_2")
                        {
                            MainViewModel.TuneRadioToFrequency(2, stationFreq);
                        }

                        uiNearbyATCActionPopup.close();

                        if(nearbyATCPopup !== null)
                            nearbyATCPopup.close();
                    }
                }
            }
        }
    }
}
