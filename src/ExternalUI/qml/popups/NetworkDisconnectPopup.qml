/*
 * File:   NetworkDisconnectPopup.qml
 * Author: christophe.delondre
 *
 * License: see LICENSE.xivap
 */

import QtQuick 2.7

import "../controls"

import "../../js/Creator.js" as Creator

import "../.."

PopupBase
{
    id: uiNetworkDisconnectPopup

    width: Creator.getMainWindow().width * 0.85
    height: 200 * ExternalUIStyle.scaleFactor

    popupTitle: "DISCONNECT FROM NETWORK"

    popupContentItem: Item
    {
        anchors.fill: parent
        anchors.margins: 20

        Text
        {
            anchors.horizontalCenter: parent.horizontalCenter

            font: ExternalUIStyle.defaultFont
            color: ExternalUIStyle.defaultFontColor

            text: "Are you sure you want to disconnect\nfrom the network?"
        }

        XTextButton
        {
            anchors.bottom: parent.bottom

            width: parent.width / 2 - 5 * ExternalUIStyle.scaleFactor
            height: 45 * ExternalUIStyle.scaleFactor

            text: "No"

            onClicked:
            {
                uiNetworkDisconnectPopup.close();
            }
        }

        XTextButton
        {
            anchors.bottom: parent.bottom
            anchors.right: parent.right

            width: parent.width / 2 - 5 * ExternalUIStyle.scaleFactor
            height: 45 * ExternalUIStyle.scaleFactor

            text: "Yes"

            onClicked:
            {
                NetworkConnectViewModel.NetworkDisconnect();
                uiNetworkDisconnectPopup.close();
            }
        }
    }
}
