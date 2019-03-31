/* 
 * File:   DataInterface.cpp
 * Author: christophe.delondre
 *
 * License: see LICENSE.xivap
 */

#include "DataInterface.h"
#include "../xivap.h"
#include "ExternalInterface.h"

extern DataInterface* sDataInterface = 0;

DataInterface::DataInterface()
: m_watchThread(0)
, m_watchThreadContinue(false)
{
    RegisterData();
}

DataInterface::~DataInterface()
{
    
}

void DataInterface::RegisterData()
{
    RegisterDatum("NETWORK_STATUS", []() 
    {
        return xivap.online() ? "1" : "0";
    });
    
    RegisterDatum("XPDR_CODE_A", []()
    {
        return itostring(xivap.getXpdrCode()).stl_str();
    });
    
    RegisterDatum("XPDR_MODE", []()
    {
        return xivap.xpdrModus() == IdentSby ? "0" : "1";
    });
    
    RegisterDatum("RADIO_FREQ_COM1", []()
    {
        return xivap.com1freq().stl_str();
    });
    
    RegisterDatum("RADIO_FREQ_COM2", []()
    {
        return xivap.com2freq().stl_str();
    });
    
    RegisterDatum("RADIO_NAME_COM1", []()
    {
        return xivap.GetCOM1Name().stl_str();
    });
    
    RegisterDatum("RADIO_NAME_COM2", []()
    {
        return xivap.GetCOM2Name().stl_str();
    });
    
    RegisterDatum("RADIO_ACTIVE", []()
    {
        return itostring(xivap.activeRadio()).stl_str();
    });
    
    RegisterDatum("TAGGING", []()
    {
        return xivap.usingLabels() ? "1" : "0";
    });
    
    RegisterDatum("NETWORK_CALLSIGN", []()
    {
        return xivap.callsign.stl_str();
    });
    
#ifdef IVAO
    RegisterDatum("IVAO_WX", []()
    {
        return xivap.usingWeather() ? "1" : "0";
    });
#endif
}

void DataInterface::RegisterDatum(const std::string& datumId, DatumQueryCallback callback)
{
    m_data.insert(std::make_pair(datumId, callback));
}

void DataInterface::BroadcastDatum(const std::string& datumId)
{
    if(datumId.empty())
        return;
    
    if(datumId == "*")
    {
        for(std::pair<std::string, DatumQueryCallback> kp : m_data)
        {
            SendDatumValue(kp.first);
        }
    }
    else
    {
        SendDatumValue(datumId);
    }
}

void DataInterface::SendDatumValue(const std::string& datumId)
{
    if(m_data.count(datumId) > 0)
    {
        DatumQueryCallback callback = m_data[datumId];
        
        // Retrieve value.
        std::string result = callback();
        
        std::string datumMessage = "XIVAPEI_SV_DATUM|";
        datumMessage.append(datumId);
        datumMessage.append("|");
        datumMessage.append(result);
        
        pExternalInterfaceServer->BroadcastMessage(datumMessage);
    }
}

void DataInterface::StartWatchThread()
{
    if(!m_watchThread)
    {
        m_watchThread = new std::thread([this]() 
        {
            m_watchThreadContinue = true;
            
            while(m_watchThreadContinue)
            {
                // Watch over each datum.
                for(std::pair<std::string, DatumQueryCallback> kp : m_data)
                {
                    std::string datumValue = (kp.second)();
                    
                    if(m_lastData.count(kp.first) == 0)
                    {
                        m_lastData.insert(std::make_pair(kp.first, datumValue));
                        BroadcastDatum(kp.first);
                    }
                    else if(m_lastData[kp.first] != datumValue)
                    {
                        m_lastData[kp.first] = datumValue;
                        BroadcastDatum(kp.first);
                    }
                }
                
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
            }
        });
    }
}

void DataInterface::StopWatchThread()
{
    if(m_watchThread)
    {
        m_watchThreadContinue = false;
        
        m_watchThread->join();
        
        delete m_watchThread;
        m_watchThread = 0;
    }
}
