/*
 * File:   ExternalUIStyle.qml
 * Author: christophe.delondre
 *
 * License: see LICENSE.xivap
 */

pragma Singleton
import QtQuick 2.0

QtObject
{
    property font defaultFont: Qt.font(
    {
        family: "Verdana",
        pixelSize: 16,
    })

    property font defaultFontMedium: Qt.font(
    {
        family: "Verdana",
        pixelSize: 22
    })

    property font defaultFontBig: Qt.font(
    {
        family: "Verdana",
        pixelSize: 28
    })

    property font popupHeaderFont: Qt.font(
    {
        family: "Verdana",
        pixelSize: 24
    })

    property color defaultFontColor: "white"

    property color defaultControlBackground: "#444444"

    property real scaleFactor: 1.0

    function setScaleFactor(factor)
    {
        scaleFactor = factor;

        defaultFont.pixelSize *= factor;
        defaultFontMedium.pixelSize *= factor;
        defaultFontBig.pixelSize *= factor;
        popupHeaderFont.pixelSize *= factor;
    }
}
