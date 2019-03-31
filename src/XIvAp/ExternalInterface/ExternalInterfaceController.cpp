/* 
 * File:   ExternalInterfaceController.cpp
 * Author: christophe.delondre
 *
 * License: see LICENSE.xivap
 */

#include "ExternalInterfaceController.h"
#include "ExternalInterface.h"
#include "../xivap.h"

ExternalInterfaceController* sExternalInterfaceController = 0;

void ExternalInterfaceController::NetworkConnect(const std::string& callsign, const std::string& userId, const std::string& password, const std::string& realName, const std::string& serverAddress, unsigned short serverPort, bool useVoice, bool isFollowMeCar)
{
    xivap.connect(callsign, userId, password, realName, true, serverAddress, itostring(serverPort), useVoice, isFollowMeCar);
}

void ExternalInterfaceController::NetworkDisconnect()
{
    xivap.disconnect();
}

void ExternalInterfaceController::XPDRModeToggle()
{
    xivap.xpdrModeToggle();
}

void ExternalInterfaceController::XPDRIdent()
{
    xivap.setXpdrIdent();
}

void ExternalInterfaceController::XPDRSetCodeA(int code)
{
    xivap.setXpdrCode(code);
}

void ExternalInterfaceController::TuneRadio(int radioIndex, int freq)
{
    xivap.tuneCom(radioIndex, freq);
}

void ExternalInterfaceController::SetActiveRadio(int radioIndex)
{
    xivap.setComActive(radioIndex);
}

void ExternalInterfaceController::BroadcastATCList()
{
    std::vector<AtcPosition> atcPositions = xivap.getAtcPositions(SortDistance);
    
    std::string atcListMessage = "XIVAPEI_SV_ATCLIST|";
    atcListMessage.append(itostring((int)atcPositions.size() - 1).stl_str());
    atcListMessage.append("|");
    
    bool first = true;
    for(AtcPosition atcPosition : atcPositions)
    {
        if(atcPosition.callsign == "UNICOM" || atcPosition.callsign == "AIR-AIR")
            continue;
        
        if(!first)
            atcListMessage.append("|");
        
        atcListMessage.append(atcPosition.callsign.stl_str());
        atcListMessage.append(",");
        atcListMessage.append(atcPosition.frequency.stl_str());
        atcListMessage.append(",");
        atcListMessage.append(atcPosition.realname.stl_str());
        atcListMessage.append(",");
        atcListMessage.append(atcPosition.vid.stl_str());
        atcListMessage.append(",");
        atcListMessage.append(itostring(atcPosition.rating).stl_str());
        
        first = false;
    }
        
    pExternalInterfaceServer->BroadcastMessage(atcListMessage);
}

void ExternalInterfaceController::SendMessageTo(const std::string& destination, const std::string& message)
{
    xivap.sendMessageTo(destination, message);
}

void ExternalInterfaceController::SetUseIVAOWeather(bool useWeather)
{
    xivap.setWeather(useWeather);
}

void ExternalInterfaceController::SetTagging(bool useTagging)
{
    xivap.setLabels(useTagging);
}
