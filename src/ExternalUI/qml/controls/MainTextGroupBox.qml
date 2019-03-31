/*
 * File:   MainTextGroupBox.qml
 * Author: christophe.delondre
 *
 * License: see LICENSE.xivap
 */

import QtQuick 2.0
import QtQuick.Controls 2.2

import "../.."

XGroupBox
{
    id: uiMainTextGroupBox

    borderColor: "#666666"
    borderWidth: 2

    header: SimulatorData.p_radioActive == 1 ? (SimulatorData.p_com1name === "" ? SimulatorData.p_com1freq : SimulatorData.p_com1freq + " / " + SimulatorData.p_com1name) : (SimulatorData.p_com2name === "" ? SimulatorData.p_com2freq : SimulatorData.p_com2freq + " / " + SimulatorData.p_com2name)

    Component.onCompleted:
    {
        MainViewModel.newActiveFrequencyMessage.connect(function(message) {
            uiMainTextGroupBoxListView.positionViewAtEnd();
        });
    }

    contentItem: ListView
    {
        id: uiMainTextGroupBoxListView

        anchors.fill: parent

        anchors.margins: 15 * ExternalUIStyle.scaleFactor

        clip: true

        model: MainViewModel.p_activeFrequencyMessagesListModel

        delegate: Text
        {
            width: uiMainTextGroupBoxListView.width

            font: ExternalUIStyle.defaultFont
            color: ExternalUIStyle.defaultFontColor

            text: model.messageText

            wrapMode: Text.WordWrap
        }
    }
}
