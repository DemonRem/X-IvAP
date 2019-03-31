/*
 * File:   NetworkConnectPopup.qml
 * Author: christophe.delondre
 *
 * License: see LICENSE.xivap
 */

import QtQuick 2.7
import QtQuick.Controls 2.2

import "../controls"

import "../.." // qmldir

import "../../js/Creator.js" as Creator

PopupBase
{
    id: uiNetworkConnectPopup

    width: Creator.getMainWindow().width * 0.8
    height: 280 * ExternalUIStyle.scaleFactor

    popupTitle: "CONNECT TO NETWORK"

    property real subControlWidth: uiNetworkConnectPopup.width * 0.45

    Component.onCompleted:
    {
        NetworkConnectViewModel.LoadFromConfiguration();
    }

    popupContentItem: Item
    {
        anchors.fill: parent
        anchors.margins: 10 * ExternalUIStyle.scaleFactor

        Column
        {
            anchors.fill: parent

            spacing: 10 * ExternalUIStyle.scaleFactor

            // Row: VID
            Item
            {
                width: parent.width
                height: 32 * ExternalUIStyle.scaleFactor

                Text
                {
                    leftPadding: 10 * ExternalUIStyle.scaleFactor

                    anchors.verticalCenter: parent.verticalCenter

                    font: ExternalUIStyle.defaultFont
                    color: ExternalUIStyle.defaultFontColor

                    text: "VID/PID"
                }

                XTextInput
                {
                    id: uiConnectUserIdTextInput

                    anchors.right: parent.right

                    width: uiNetworkConnectPopup.subControlWidth
                    height: parent.height

                    text: NetworkConnectViewModel.p_userId
                }

                Binding
                {
                    target: NetworkConnectViewModel
                    property: "p_userId"
                    value: uiConnectUserIdTextInput.text
                }
            }

            // Row: Password
            Item
            {
                width: parent.width
                height: 32 * ExternalUIStyle.scaleFactor

                Text
                {
                    leftPadding: 10 * ExternalUIStyle.scaleFactor

                    anchors.verticalCenter: parent.verticalCenter

                    font: ExternalUIStyle.defaultFont
                    color: ExternalUIStyle.defaultFontColor

                    text: "Password"
                }

                XTextInput
                {
                    id: uiConnectPasswordTextInput

                    anchors.right: parent.right

                    width: uiNetworkConnectPopup.subControlWidth
                    height: parent.height

                    echoMode: TextInput.Password

                    text: NetworkConnectViewModel.p_password
                }

                Binding
                {
                    target: NetworkConnectViewModel
                    property: "p_password"
                    value: uiConnectPasswordTextInput.text
                }
            }

            // Row: Real Name
            Item
            {
                width: parent.width
                height: 32 * ExternalUIStyle.scaleFactor

                Text
                {
                    leftPadding: 10 * ExternalUIStyle.scaleFactor

                    anchors.verticalCenter: parent.verticalCenter

                    font: ExternalUIStyle.defaultFont
                    color: ExternalUIStyle.defaultFontColor

                    text: "Real Name"
                }

                XTextInput
                {
                    id: uiConnectRealNameTextInput

                    anchors.right: parent.right

                    width: uiNetworkConnectPopup.subControlWidth
                    height: parent.height

                    text: NetworkConnectViewModel.p_realName

                    Binding
                    {
                        target: NetworkConnectViewModel
                        property: "p_realName"
                        value: uiConnectRealNameTextInput.text
                    }
                }
            }

            // Row: Voice?
            Item
            {
                width: parent.width
                height: 32 * ExternalUIStyle.scaleFactor

                Text
                {
                    leftPadding: 10 * ExternalUIStyle.scaleFactor

                    anchors.verticalCenter: parent.verticalCenter

                    font: ExternalUIStyle.defaultFont
                    color: ExternalUIStyle.defaultFontColor

                    text: "Voice ?"
                }

                XCheckBox
                {
                    id: uiNetworkConnectUseVoiceCheckbox

                    anchors.right: parent.right

                    width: uiNetworkConnectPopup.subControlWidth
                    height: parent.height

                    font: ExternalUIStyle.defaultFont

                    text: "Use Voice"

                    contentItem: Text
                    {
                        anchors.verticalCenter: parent.verticalCenter
                        font: uiNetworkConnectUseVoiceCheckbox.font
                        color: ExternalUIStyle.defaultFontColor
                        text: uiNetworkConnectUseVoiceCheckbox.text
                        //horizontalAlignment: Text.AlignHCenter
                        //verticalAlignment: Text.AlignVCenter
                        leftPadding: uiNetworkConnectUseVoiceCheckbox.indicator.width + uiNetworkConnectUseVoiceCheckbox.spacing
                    }
                }
            }

            Item
            {
                width: parent.width
                height: 38 * ExternalUIStyle.scaleFactor

                XTextButton
                {
                    anchors.right: parent.right

                    width: uiNetworkConnectPopup.subControlWidth
                    height: parent.height

                    text: "CONNECT"

                    onClicked:
                    {
                        if(NetworkConnectViewModel.NetworkConnect())
                        {
                            uiNetworkConnectPopup.close();
                        }
                    }
                }
            }
        }
    }
}
