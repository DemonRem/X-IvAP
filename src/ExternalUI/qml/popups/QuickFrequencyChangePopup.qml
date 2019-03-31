/*
 * File:   QuickFrequencyChangePopup.qml
 * Author: christophe.delondre
 *
 * License: see LICENSE.xivap
 */

import QtQuick 2.7

import "../controls"

import "../../js/Creator.js" as Creator

import "../.." // qmldir

PopupBase
{
    id: uiQuickFrequencyChangePopup

    width: Creator.getMainWindow().width * 0.4
    height: (55 * ExternalUIStyle.scaleFactor) + MainViewModel.p_atcListModel.p_size * (60 * ExternalUIStyle.scaleFactor)

    popupTitle: "TUNE COM" + radioIndex

    property int radioIndex: 1

    popupContentItem: Item
    {
        anchors.fill: parent

        ListView
        {
            id: uiQuickFrequencyChangeListView

            anchors.fill: parent

            model: MainViewModel.p_atcListModel

            clip: true

            delegate: Rectangle
            {
                width: uiQuickFrequencyChangeListView.width
                height: 60 * ExternalUIStyle.scaleFactor

                color: index % 2 == 0 ? "#444444" : "#222222"

                Column
                {
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.verticalCenter: parent.verticalCenter

                    Text
                    {
                        anchors.horizontalCenter: parent.horizontalCenter

                        font: ExternalUIStyle.defaultFontMedium
                        color: ExternalUIStyle.defaultFontColor
                        text: model.frequency
                    }

                    Text
                    {
                        anchors.horizontalCenter: parent.horizontalCenter

                        font: ExternalUIStyle.defaultFont
                        color: ExternalUIStyle.defaultFontColor
                        text: model.callsign
                    }
                }

                MouseArea
                {
                    anchors.fill: parent

                    onClicked:
                    {
                        MainViewModel.TuneRadioToFrequency(uiQuickFrequencyChangePopup.radioIndex, model.frequency);
                        uiQuickFrequencyChangePopup.close();
                    }
                }
            }
        }
    }
}
