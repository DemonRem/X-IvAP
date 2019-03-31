/*
 * File:   main.qml
 * Author: christophe.delondre
 *
 * License: see LICENSE.xivap
 */

import QtQuick 2.9
import QtQuick.Window 2.2
import QtQuick.Controls 2.2
import QtGraphicalEffects 1.0

import "./controls"

import "../js/Creator.js" as Creator
import "../js/PopupManager.js" as PopupManager
import "../js/Utility.js" as Utility

import ".." // qmldir

ApplicationWindow
{
    id: uiMainWindow

    visible: true

    minimumWidth: 480
    maximumWidth: 480

    minimumHeight: 640
    maximumHeight: 640

    title: "X-IvAp External Interface"

    overlay.modal: Item
    {
        FastBlur
        {
            anchors.fill: parent
            source: uiMainContainer
            radius: 1
            width: 5
        }
    }

    Component.onCompleted:
    {
        Creator.setMainWindow(uiMainWindow);

        MainViewModel.alertRequested.connect(function(text) {
            PopupManager.alert(text);
        });

        MainViewModel.showMessagesPopupRequested.connect(function() {
            PopupManager.showPopup("Messages");
        });

        ExternalUIStyle.setScaleFactor(Math.min(uiMainWindow.width / uiMainWindow.minimumWidth, uiMainWindow.height / uiMainWindow.minimumHeight));

        MainViewModel.AppendActiveFrequencyMessage("Application ready.");
    }

    function sendActiveFrequencyMessage()
    {
        if(uiActiveFreqMessageTextInput.text.length > 0)
        {
            console.log("send message: " + uiActiveFreqMessageTextInput.text);

            MainViewModel.SendMessageToActiveFrequency(uiActiveFreqMessageTextInput.text);

            uiActiveFreqMessageTextInput.text = "";
        }
    }

    // Background Rectangle
    Rectangle
    {
        id: uiMainRect

        anchors.fill: parent

        color: "#333333"

        Column
        {
            anchors.fill: parent

            anchors.margins: 15 * ExternalUIStyle.scaleFactor

            spacing: 15 * ExternalUIStyle.scaleFactor

            MainTextGroupBox
            {
                width: parent.width
                height: parent.height * 0.45 - parent.spacing
            }

            Row
            {
                id: uiActiveFreqMessageSendRow

                width: parent.width
                height: parent.height * 0.05

                spacing: 10

                XTextInput
                {
                    id: uiActiveFreqMessageTextInput

                    width: parent.width - uiActiveFreqMessageSendButton.width - parent.spacing
                    height: parent.height

                    Keys.onEnterPressed:
                    {
                        sendActiveFrequencyMessage();
                    }

                    Keys.onReturnPressed:
                    {
                        sendActiveFrequencyMessage();
                    }
                }

                XTextButton
                {
                    id: uiActiveFreqMessageSendButton

                    width: 80 * ExternalUIStyle.scaleFactor
                    height: parent.height

                    text: "SEND"

                    onClicked:
                    {
                        sendActiveFrequencyMessage();
                    }
                }
            }

            XGroupBox
            {
                id: uiXPDRGroupBox

                width: parent.width
                height: parent.height * 0.15 - parent.spacing

                borderColor: "#666666"
                borderWidth: 2

                header: "XPDR"

                contentItem: Item
                {
                    anchors.fill: parent

                    Text
                    {
                        anchors.verticalCenter: parent.verticalCenter
                        x: 25 * ExternalUIStyle.scaleFactor

                        font: ExternalUIStyle.defaultFontBig

                        color: ExternalUIStyle.defaultFontColor

                        text: Utility.lpad(SimulatorData.p_xpdrCodeA, 4)
                    }

                    TransponderControlButton
                    {
                        anchors.centerIn: parent

                        width: 125 * ExternalUIStyle.scaleFactor
                        height: parent.height * 0.65
                    }

                    XTextButton
                    {
                        anchors.right: parent.right
                        anchors.rightMargin: 10 * ExternalUIStyle.scaleFactor

                        anchors.verticalCenter: parent.verticalCenter

                        width: height + 10 * ExternalUIStyle.scaleFactor
                        height: parent.height * 0.65

                        text: "ID"

                        onClicked:
                        {
                            MainViewModel.TransponderIdent();
                        }
                    }
                }
            }

            Row
            {
                id: uiRadioDisplaysRow

                width: parent.width
                height: parent.height * 0.2 - parent.spacing

                spacing: 10

                RadioDisplayControl
                {
                    width: parent.width / 2 - parent.spacing / 2
                    height: parent.height

                    radioIndex: 1
                }

                RadioDisplayControl
                {
                    width: parent.width / 2 - parent.spacing / 2
                    height: parent.height

                    radioIndex: 2
                }
            }

            Row
            {
                id: uiMainButtonsRow

                width: parent.width
                height: parent.height * 0.15 - parent.spacing

                spacing: (uiMainButtonsRow.width - (uiMainButtonsRow.height * uiMainButtonsRow.children.length)) / (uiMainButtonsRow.children.length - 1)

                XTextButton
                {
                    width: parent.height
                    height: parent.height

                    text: SimulatorData.p_networkStatus == 1 ? "ONLINE" : "OFFLINE"

                    onClicked:
                    {
                        var popup = PopupManager.showPopup(SimulatorData.p_networkStatus == 0 ? "NetworkConnect" : "NetworkDisconnect");
                    }
                }

                XTextButton
                {
                    width: parent.height
                    height: parent.height

                    text: "ATC"

                    onClicked:
                    {
                        var popup = PopupManager.showPopup("NearbyATC");
                    }
                }

                XTextButton
                {
                    width: parent.height
                    height: parent.height

                    text: "MSG"

                    onClicked:
                    {
                        PopupManager.showPopup("Messages");
                    }
                }

                XTextButton
                {
                    width: parent.height
                    height: parent.height

                    text: "STGS"

                    onClicked:
                    {
                        PopupManager.showPopup("Settings");
                    }
                }
            }
        }

        Text
        {
            width: 1
            height: 1
            text: MainViewModel.p_tickString
        }
    }

    Rectangle
    {
        id: uiOverlayConnect

        anchors.fill: parent

        color: "black"

        opacity: 0.95

        visible: ConnectViewModel.p_state !== 4

        MouseArea
        {
            // Guard to prevent clicks from reaching the main UI while overlay is visible.
            anchors.fill: parent
        }

        Column
        {
            width: parent.width
            anchors.verticalCenter: parent.verticalCenter

            spacing: 25 * ExternalUIStyle.scaleFactor

            Text
            {
                anchors.horizontalCenter: parent.horizontalCenter

                font: ExternalUIStyle.defaultFontMedium
                color: ExternalUIStyle.defaultFontColor

                text: "Connect to X-IvAp"
            }

            Item
            {
                width: parent.width * 0.75
                height: 140 * ExternalUIStyle.scaleFactor

                anchors.horizontalCenter: parent.horizontalCenter

                Column
                {
                    id: uiConnectInputsColumn

                    anchors.fill: parent

                    spacing: 10 * ExternalUIStyle.scaleFactor

                    visible: ConnectViewModel.p_state === 0

                    Text
                    {
                        font: ExternalUIStyle.defaultFont
                        color: ExternalUIStyle.defaultFontColor

                        text: "X-IvAp Simulator Address"
                    }

                    XTextInput
                    {
                        id: uiConnectAddressTextInput

                        width: parent.width
                        height: 32 * ExternalUIStyle.scaleFactor

                        text: ExternalUIConfiguration.GetLastAddress()

                        KeyNavigation.tab: uiConnectAuthorizationKeyTextInput.internalTextInput
                    }

                    Text
                    {
                        font: ExternalUIStyle.defaultFont
                        color: ExternalUIStyle.defaultFontColor

                        text: "Authorization Key"
                    }

                    XTextInput
                    {
                        id: uiConnectAuthorizationKeyTextInput

                        width: parent.width
                        height: 32 * ExternalUIStyle.scaleFactor

                        text: ExternalUIConfiguration.GetLastAuthorizationKey()

                        KeyNavigation.tab: uiConnectAddressTextInput.internalTextInput
                    }
                }

                Text
                {
                    anchors.centerIn: parent

                    visible: ConnectViewModel.p_state === 1

                    font: ExternalUIStyle.defaultFont
                    color: ExternalUIStyle.defaultFontColor

                    text: "Connecting to X-IvAp ..."
                }

                Text
                {
                    anchors.centerIn: parent

                    visible: ConnectViewModel.p_state === 2 || ConnectViewModel.p_state == 3

                    font: ExternalUIStyle.defaultFont
                    color: ExternalUIStyle.defaultFontColor

                    text: "Authenticating ..."
                }
            }

            XTextButton
            {
                width: 100 * ExternalUIStyle.scaleFactor
                height: 50 * ExternalUIStyle.scaleFactor

                anchors.horizontalCenter: parent.horizontalCenter

                text: ConnectViewModel.p_state === 0 ? "Connect" : "Cancel"

                visible: ConnectViewModel.p_state !== 4

                onClicked:
                {
                    Qt.inputMethod.hide();
                    uiMainRect.forceActiveFocus();

                    if(ConnectViewModel.p_state === 0)
                    {
                        if(!Utility.validateIPAddress(uiConnectAddressTextInput.text))
                        {
                            PopupManager.alert("Invalid IPv4 address.");
                            return;
                        }

                        if(uiConnectAuthorizationKeyTextInput.text.length < 4)
                        {
                            PopupManager.alert("Authorization Key is too short.");
                            return;
                        }

                        // Validate IP Address and Key Length
                        ConnectViewModel.Connect(uiConnectAddressTextInput.text, uiConnectAuthorizationKeyTextInput.text);
                    }
                    else if(ConnectViewModel.p_state > 0 && ConnectViewModel.p_state <= 4)
                    {
                        ConnectViewModel.Disconnect();
                    }
                }
            }
        }
    }
}
