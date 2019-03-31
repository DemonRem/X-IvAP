/* 
 * File:   ExternalInterfaceController.h
 * Author: christophe.delondre
 *
 * License: see LICENSE.xivap
 */

#ifndef EXTERNALINTERFACECONTROLLER_H
#define EXTERNALINTERFACECONTROLLER_H

#include <string>

class ExternalInterfaceController
{
public:
    void NetworkConnect(const std::string& callsign, const std::string& userId, const std::string& password, const std::string& realName,
            const std::string& serverAddress, unsigned short serverPort, bool useVoice, bool isFollowMeCar);
    void NetworkDisconnect();
    
    void XPDRModeToggle();
    void XPDRIdent();
    void XPDRSetCodeA(int code);
    
    void TuneRadio(int radioIndex, int freq);
    void SetActiveRadio(int radioIndex);
    
    void SendMessageTo(const std::string& destination, const std::string& message);
    
    void SetUseIVAOWeather(bool useWeather);
    
    void SetTagging(bool useTagging);
    
    void BroadcastATCList();
};

extern ExternalInterfaceController* sExternalInterfaceController;

#endif /* EXTERNALINTERFACECONTROLLER_H */

