/*
 * File:   XTextInput.qml
 * Author: christophe.delondre
 *
 * License: see LICENSE.xivap
 */

import QtQuick 2.0
import QtQuick.Controls 2.2

import "../.."

Item
{
    id: uiXTextInput

    property color backgroundColor: ExternalUIStyle.defaultControlBackground

    property font font: ExternalUIStyle.defaultFont
    property color fontColor: ExternalUIStyle.defaultFontColor

    property var echoMode: TextInput.Normal

    property alias text: uiXTextInputInternalInput.text
    property alias internalTextInput: uiXTextInputInternalInput

    Rectangle
    {
        anchors.fill: parent

        color: uiXTextInput.backgroundColor

        TextInput
        {
            id: uiXTextInputInternalInput

            x: 5
            width: parent.width - 10

            anchors.verticalCenter: parent.verticalCenter

            echoMode: uiXTextInput.echoMode

            font: uiXTextInput.font
            color: uiXTextInput.fontColor
        }
    }
}
