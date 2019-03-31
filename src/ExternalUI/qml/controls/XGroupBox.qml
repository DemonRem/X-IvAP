/*
 * File:   XGroupBox.qml
 * Author: christophe.delondre
 *
 * License: see LICENSE.xivap
 */

import QtQuick 2.0
import "../.." // qmldir

Item
{
    id: uiXGroupBox

    property real       borderWidth: 1
    property color      borderColor: "white"

    property var        contentItem: Item { }

    property string     header: "Header"

    property font       headerFont: ExternalUIStyle.defaultFont
    property color      headerFontColor: ExternalUIStyle.defaultFontColor

    Rectangle
    {
        anchors.top: parent.top

        width: 10
        height: borderWidth

        color: uiXGroupBox.borderColor
    }

    Rectangle
    {
        anchors.top: parent.top
        anchors.right: parent.right

        width: parent.width - 10 - 6 - 6 - uiXGroupBoxHeaderText.width
        height: borderWidth

        color: uiXGroupBox.borderColor
    }

    // Right Border
    Rectangle
    {
        anchors.right: parent.right

        width: borderWidth
        height: parent.height

        color: uiXGroupBox.borderColor
    }

    // Bottom Border
    Rectangle
    {
        anchors.bottom: parent.bottom

        width: parent.width
        height: borderWidth

        color: uiXGroupBox.borderColor
    }

    // Left Border
    Rectangle
    {
        anchors.left: parent.left

        width: borderWidth
        height: parent.height

        color: uiXGroupBox.borderColor
    }

    Text
    {
        id: uiXGroupBoxHeaderText

        x: 16
        y: -(height / 2)
        text: uiXGroupBox.header

        color: uiXGroupBox.headerFontColor

        font: uiXGroupBox.headerFont
    }

    Item
    {
        anchors.fill: parent
        id: uiXGroupBoxContentItem
        children: uiXGroupBox.contentItem
    }
}
