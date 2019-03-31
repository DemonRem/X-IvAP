/*
 * File:   MessagesActiveChatListPopup.qml
 * Author: christophe.delondre
 *
 * License: see LICENSE.xivap
 */

import QtQuick 2.7
import QtQuick.Controls 2.2

import "../controls"

import "../.." // qmldir

Popup
{
    id: uiMessagesActiveChatListPopup

    margins: 0
    padding: 0

    property int listEntryHeight: 40

    signal itemSelected(string endpointId)

    Rectangle
    {
        anchors.fill: parent

        color: "#444444"

        ListView
        {
            id: uiMessagesActiveChatListView

            anchors.fill: parent

            model: MainViewModel.p_activePrivateChatsListModel

            delegate: Rectangle
            {
                width: uiMessagesActiveChatListView.width
                height: uiMessagesActiveChatListPopup.listEntryHeight * ExternalUIStyle.scaleFactor

                color: "#444444"

                Row
                {
                    anchors.fill: parent

                    leftPadding: 10 * ExternalUIStyle.scaleFactor

                    spacing: 10 * ExternalUIStyle.scaleFactor

                    NewMessageIndicator
                    {
                        anchors.verticalCenter: parent.verticalCenter

                        width: 12 * ExternalUIStyle.scaleFactor
                        height: 12 * ExternalUIStyle.scaleFactor

                        active: model.hasUnreadMessages
                    }

                    Text
                    {
                        font: ExternalUIStyle.defaultFont
                        color: ExternalUIStyle.defaultFontColor

                        anchors.verticalCenter: parent.verticalCenter

                        text: model.endpointId
                    }
                }

                MouseArea
                {
                    anchors.fill: parent

                    onClicked:
                    {
                        uiMessagesActiveChatListPopup.itemSelected(model.endpointId);
                    }
                }
            }
        }
    }
}
