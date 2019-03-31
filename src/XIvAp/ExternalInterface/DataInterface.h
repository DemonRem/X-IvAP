/* 
 * File:   DataInterface.h
 * Author: christophe.delondre
 *
 * License: see LICENSE.xivap
 */

#ifndef DATAINTERFACE_H
#define DATAINTERFACE_H

#include <functional>
#include <map>
#include <string>
#include <thread>

typedef std::function<std::string()> DatumQueryCallback;

class DataInterface
{
public:
    DataInterface();
    ~DataInterface();
    
    // Processes an inbound request for a specific datum, or all if datumId == "*"
    void BroadcastDatum(const std::string& datumId);
    
    void StartWatchThread();
    void StopWatchThread();
private:
    void RegisterData();
    void RegisterDatum(const std::string& datumId, DatumQueryCallback callback);
    void SendDatumValue(const std::string& datumId);
private:
    std::map<std::string, DatumQueryCallback>   m_data;
    std::map<std::string, std::string>          m_lastData;
    std::thread*                                m_watchThread;
    bool                                        m_watchThreadContinue;
};

extern DataInterface* sDataInterface;

#endif /* DATAINTERFACE_H */

