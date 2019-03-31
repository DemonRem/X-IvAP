/*
 * File:   MessagesActiveChatControl.qml
 * Author: christophe.delondre
 *
 * License: see LICENSE.xivap
 */

import QtQuick 2.0

import "../../js/Creator.js" as Creator

import "../.." // qmldir

Item
{
    id: uiMessagesActiveChatControl

    property var currentPopup: null

    property var selectedEndpoint: ""

    function showPopup()
    {
        var c = Creator.createComponent("qrc:/qml/popups/MessagesActiveChatListPopup.qml");

        if(c !== null)
        {
            var popup = c.createObject(uiMessagesActiveChatControl, { "parent" : uiMessagesActiveChatControl });

            if(popup !== null)
            {
                popup.width = uiMessagesActiveChatControl.width
                popup.height = 200 * ExternalUIStyle.scaleFactor;//Math.min(32 * uiMessagesActiveChatListModel.count, 300 * ExternalUIStyle.scaleFactor);

                popup.y = uiMessagesActiveChatControl.height + 5;

                popup.itemSelected.connect(function(endpointId)
                {
                    selectedEndpoint = endpointId;
                    popup.close();
                });

                popup.open();
            }
        }
    }

    Rectangle
    {
        anchors.fill: parent

        border.color: "white"
        border.width: 1

        color: "#222222"

        radius: 2

        Text
        {
            anchors.left: parent.left
            anchors.leftMargin: 10 * ExternalUIStyle.scaleFactor

            anchors.verticalCenter: parent.verticalCenter

            font: ExternalUIStyle.defaultFont
            color: ExternalUIStyle.defaultFontColor

            text: MainViewModel.p_selectedPrivateChat.p_endpointId
        }

        // Arrow Icon, initially drawn pointing downwards, will be rotated by transform.
        Canvas
        {
            id: uiMessagesActiveChatControlArrowIcon

            anchors.right: parent.right
            anchors.rightMargin: 15 * ExternalUIStyle.scaleFactor

            anchors.verticalCenter: parent.verticalCenter

            width: 12 * ExternalUIStyle.scaleFactor
            height: 8 * ExternalUIStyle.scaleFactor

            onPaint:
            {
                var ctx = getContext("2d");

                ctx.reset();

                ctx.fillStyle = "white";

                ctx.moveTo(0, 0);
                ctx.lineTo(uiMessagesActiveChatControlArrowIcon.width, 0);
                ctx.lineTo(uiMessagesActiveChatControlArrowIcon.width / 2, uiMessagesActiveChatControlArrowIcon.height);
                ctx.lineTo(0, 0);

                ctx.fill();
            }
        }

        MouseArea
        {
            anchors.fill: parent

            onClicked:
            {
                if(currentPopup !== null)
                {
                    currentPopup.close();
                }
                else
                {
                    showPopup();
                }
            }
        }
    }
}
