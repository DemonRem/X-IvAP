/*
 * File:   NearbyATCPopup.qml
 * Author: christophe.delondre
 *
 * License: see LICENSE.xivap
 */

import QtQuick 2.7

import "../controls"

import "../../js/Creator.js" as Creator

import "../.." // qmldir

PopupBase
{
    id: uiNearbyATCPopup

    width: Creator.getMainWindow().width * 0.8
    height: Creator.getMainWindow().height * 0.75

    popupTitle: "NEARBY ATC STATIONS"

    function showActionPopup(sender, stationId, stationFreq)
    {
        var c = Creator.createComponent("qrc:/qml/popups/NearbyATCActionPopup.qml");

        if(c !== null)
        {
            var popup = c.createObject(sender, { "parent" : sender });

            if(popup !== null)
            {
                popup.stationId = stationId;
                popup.stationFreq = stationFreq;
                popup.nearbyATCPopup = uiNearbyATCPopup;

                popup.x = -(popup.width - sender.width)

                popup.open();
            }
        }
    }

    popupContentItem: Item
    {
        anchors.fill: parent

        ListView
        {
            id: uiNearbyATCListView

            anchors.fill: parent

            model: MainViewModel.p_atcListModel

            clip: true

            delegate: Rectangle
            {
                width: uiNearbyATCListView.width
                height: 60 * ExternalUIStyle.scaleFactor

                color: index % 2 == 0 ? "#444444" : "#222222"

                Column
                {
                    anchors.left: parent.left
                    anchors.leftMargin: 10 * ExternalUIStyle.scaleFactor

                    anchors.verticalCenter: parent.verticalCenter

                    Text
                    {
                        font: ExternalUIStyle.defaultFontMedium
                        color: ExternalUIStyle.defaultFontColor
                        text: model.frequency
                    }

                    Text
                    {
                        font: ExternalUIStyle.defaultFont
                        color: ExternalUIStyle.defaultFontColor
                        text: model.callsign
                    }
                }

                XTextButton
                {
                    id: uiNearbyATCActionButton

                    anchors.right: parent.right
                    anchors.rightMargin: 10 * ExternalUIStyle.scaleFactor

                    anchors.verticalCenter: parent.verticalCenter

                    backgroundColor: "#555555"

                    width: height
                    height: parent.height * 0.75

                    text: "->"

                    onClicked:
                    {
                        showActionPopup(uiNearbyATCActionButton, model.callsign, model.frequency);
                    }
                }
            }
        }
    }
}
