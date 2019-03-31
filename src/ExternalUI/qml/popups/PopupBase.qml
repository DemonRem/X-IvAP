/*
 * File:   PopupBase.qml
 * Author: christophe.delondre
 *
 * License: see LICENSE.xivap
 */

import QtQuick 2.7
import QtQuick.Controls 2.2

import "../.."

Popup
{
    id: uiPopupBase

    property var popupContentItem: Item { }

    property string popupTitle: ""

    margins: 0
    padding: 0

    modal: true

    contentItem: Item
    {
        // Background
        Rectangle
        {
            anchors.fill: parent

            color: "#333333"

            radius: 5

            border.color: "white"
            border.width: 1

            clip: true

            Column
            {
                anchors.fill: parent

                anchors.leftMargin: 1
                anchors.rightMargin: 1
                anchors.topMargin: 1

                // Header
                Rectangle
                {
                    id: uiPopupBaseHeaderRect

                    width: parent.width
                    height: 50 * ExternalUIStyle.scaleFactor

                    color: "#222222"

                    Text
                    {
                        anchors.centerIn: parent

                        font: ExternalUIStyle.popupHeaderFont
                        color: ExternalUIStyle.defaultFontColor

                        text: uiPopupBase.popupTitle
                    }
                }

                Item
                {
                    id: uiPopupBaseContentItem

                    width: parent.width
                    height: parent.height - uiPopupBaseHeaderRect.height - 2

                    children: uiPopupBase.popupContentItem
                }
            }
        }
    }

}

