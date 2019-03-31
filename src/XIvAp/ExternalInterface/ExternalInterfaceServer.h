/* 
 * File:   ExternalInterfaceServer.h
 * Author: christophe.delondre
 *
 * License: see LICENSE.xivap
 */

#ifndef EXTERNALINTERFACESERVER_H
#define EXTERNALINTERFACESERVER_H

#include "asio.hpp"

#include "ExternalInterfaceServerClient.h"

#include <map>
#include <mutex>

#define EXTERNALINTERFACESERVER_STATE_IDLE       0
#define EXTERNALINTERFACESERVER_STATE_STARTED    1

typedef std::map<uint32_t, std::shared_ptr<ExternalInterfaceServerClient> > EISClientList_t;

class ExternalInterfaceServer
{
public:
    ExternalInterfaceServer(const char* address, int port);
    ~ExternalInterfaceServer();
    
    void                    Start();
    void                    Stop();
    
    void                    OnClientDisconnect(uint32_t clientId);
    
    void                    BroadcastMessage(const std::string& message);
private:
    void                    BeginAccept();
private:
    int                     m_state;
    asio::ip::tcp::acceptor m_acceptor;
    asio::ip::tcp::socket   m_clientSocket;
    EISClientList_t         m_clientList;
    std::mutex              m_clientListMutex;
};

#endif /* EXTERNALINTERFACESERVER_H */

