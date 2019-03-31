/*
 * File:   XButton.qml
 * Author: christophe.delondre
 *
 * License: see LICENSE.xivap
 */

import QtQuick 2.0

import "../.."

Item
{
    id: uiXButton

    property color backgroundColor: ExternalUIStyle.defaultControlBackground

    property bool active: false

    property var contentItem: Item { }

    signal clicked()

    Rectangle
    {
        anchors.fill: parent

        color: uiXButton.backgroundColor

        Item
        {
            anchors.fill: parent
            children: uiXButton.contentItem
        }
    }

    MouseArea
    {
        anchors.fill: parent

        onPressed:
        {
            uiXButton.active = true;
        }

        onReleased:
        {
            uiXButton.active = false;

            uiXButton.clicked();
        }
    }
}
