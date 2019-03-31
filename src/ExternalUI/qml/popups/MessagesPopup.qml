/*
 * File:   MessagesPopup.qml
 * Author: christophe.delondre
 *
 * License: see LICENSE.xivap
 */

import QtQuick 2.7
import QtQuick.Controls 2.2

import "../controls"

import "../../js/Creator.js" as Creator

import "../.." // qmldir

PopupBase
{
    id: uiMessagesPopup

    width: Creator.getMainWindow().width * 0.8
    height: Creator.getMainWindow().height * 0.75

    popupTitle: "MESSAGES"

    function sendMessageToSelectedPrivateChat()
    {
        if(uiMessagesActiveChatSendTextInput.text.length > 0)
        {
            MainViewModel.SendMessageToSelectedPrivateChat(uiMessagesActiveChatSendTextInput.text);

            uiMessagesActiveChatSendTextInput.text = "";
        }
    }

    Component.onCompleted:
    {
        MainViewModel.SetMessagesPopupVisible(true);

        uiMessagesPopup.closed.connect(function() {
            MainViewModel.SetMessagesPopupVisible(false);
        });
    }

    popupContentItem: Rectangle
    {
        anchors.fill: parent

        color: "#333333"

        Column
        {
            anchors.fill: parent
            anchors.margins: 10 * ExternalUIStyle.scaleFactor

            spacing: 10 * ExternalUIStyle.scaleFactor

            MessagesActiveChatControl
            {
                id: uiMessagesActiveChatControl

                width: parent.width
                height: 40 * ExternalUIStyle.scaleFactor

                onSelectedEndpointChanged:
                {
                    console.log("Switch to chat: " + uiMessagesActiveChatControl.selectedEndpoint);
                    MainViewModel.SelectPrivateChatByEndpointId(uiMessagesActiveChatControl.selectedEndpoint);
                }
            }

            Rectangle
            {
                // Display Text, adaptative
                width: parent.width
                height: parent.height - uiMessagesActiveChatControl.height - uiMessagesActiveChatSendRow.height - parent.spacing * 2

                border.color: "white"
                border.width: 1

                color: "transparent"

                ListView
                {
                    id: uiMessagesActiveChatListView

                    anchors.fill: parent

                    anchors.margins: 10 * ExternalUIStyle.scaleFactor

                    clip: true

                    model: MainViewModel.p_selectedPrivateChat

                    delegate: Text
                    {
                        width: uiMessagesActiveChatListView.width

                        font: ExternalUIStyle.defaultFont
                        color: ExternalUIStyle.defaultFontColor

                        text: (model.direction ? "ME> " : (MainViewModel.p_selectedPrivateChat.p_endpointId + "> ")) + model.textMessage

                        wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                    }
                }
            }

            Row
            {
                id: uiMessagesActiveChatSendRow

                width: parent.width
                height: 32 * ExternalUIStyle.scaleFactor

                spacing: 10

                XTextInput
                {
                    id: uiMessagesActiveChatSendTextInput

                    width: parent.width - uiMessagesActiveChatSendButton.width - parent.spacing
                    height: parent.height

                    Keys.onReturnPressed:
                    {
                        sendMessageToSelectedPrivateChat();
                    }

                    Keys.onEnterPressed:
                    {
                        sendMessageToSelectedPrivateChat();
                    }
                }

                XTextButton
                {
                    id: uiMessagesActiveChatSendButton

                    width: 80 * ExternalUIStyle.scaleFactor
                    height: parent.height

                    text: "SEND"

                    onClicked:
                    {
                        sendMessageToSelectedPrivateChat();
                    }
                }
            }
        }
    }
}
