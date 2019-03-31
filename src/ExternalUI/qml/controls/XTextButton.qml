/*
 * File:   XTextButton.qml
 * Author: christophe.delondre
 *
 * License: see LICENSE.xivap
 */

import QtQuick 2.0

import "../.."

XButton
{
    id: uiXTextButton

    property font font: ExternalUIStyle.defaultFont
    property color fontColor: ExternalUIStyle.defaultFontColor

    property string text: ""

    contentItem: Text
    {
        anchors.centerIn: parent

        font: uiXTextButton.font

        color: uiXTextButton.fontColor

        text: uiXTextButton.text
    }
}
