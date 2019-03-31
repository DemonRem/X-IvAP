/* 
 * File:   ExternalInterfaceServer.cpp
 * Author: christophe.delondre
 *
 * License: see LICENSE.xivap
 */

#include "ExternalInterfaceServer.h"

#include "ASIOCore.h"
#include "XPLMUtilities.h"
#include "Utility.h"

ExternalInterfaceServer::ExternalInterfaceServer(const char* address, int port)
: m_acceptor(sASIOCoreIOService, asio::ip::tcp::endpoint(asio::ip::address_v4::from_string(address), port))
, m_clientSocket(sASIOCoreIOService)
, m_state(EXTERNALINTERFACESERVER_STATE_IDLE)
{
    srand(time(0));
}

ExternalInterfaceServer::~ExternalInterfaceServer()
{
    
}

void ExternalInterfaceServer::Start()
{
    if(m_state == EXTERNALINTERFACESERVER_STATE_IDLE)
    {
        BeginAccept();
        
        m_state = EXTERNALINTERFACESERVER_STATE_STARTED;
    }
}

void ExternalInterfaceServer::Stop()
{
    if(m_state == EXTERNALINTERFACESERVER_STATE_STARTED)
    {
        m_acceptor.close();
        
        // Flush the clients.
        m_clientListMutex.lock();
        
        for(std::pair<uint32_t, std::shared_ptr<ExternalInterfaceServerClient> > kp : m_clientList)
        {
            kp.second->Stop();
        }
        
        m_clientList.clear();
        
        m_clientListMutex.unlock();
        
        m_state = EXTERNALINTERFACESERVER_STATE_IDLE;
    }
}

void ExternalInterfaceServer::BeginAccept()
{
    m_acceptor.async_accept(m_clientSocket, [this](std::error_code error)
    {
        if(error)
        {
            Utility::XLog("ExternalInterfaceServer: failed to accept inbound connection, error was: %s\n", error.message().c_str());
        }
        else
        {
            std::shared_ptr<ExternalInterfaceServerClient> newClient = std::make_shared<ExternalInterfaceServerClient>(std::move(m_clientSocket));
            
            Utility::XLog("AcceptLock");
            m_clientListMutex.lock();
            Utility::XLog("AcceptedLockAcquired");
            
            // Generate a client ID.
            uint32_t newClientId = rand();
            while(m_clientList.count(newClientId) > 0)
                newClientId = rand(); // TIP: there is a leak here. find it and explain it, and you will get a beer.
            
            newClient->SetClientId(newClientId);
            newClient->SetServerPointer((void*)this);
            
            m_clientList.insert(std::make_pair(newClientId, newClient));
            
            m_clientListMutex.unlock();
            Utility::XLog("AcceptUnlock");
            
            Utility::XLog("ExternalInterfaceServer: accepted new connection, id = %d.\n", newClientId);
            
            newClient->Start(); // TODO: send this to the thread it deserves (pkt queue).
            
            BeginAccept();
        }
    });
}

void ExternalInterfaceServer::OnClientDisconnect(uint32_t clientId)
{
    Utility::XLog("DisconnectLock");
    m_clientListMutex.lock();
    Utility::XLog("DisconnectLockAcquired");
    
    EISClientList_t::iterator it = m_clientList.find(clientId);
    
    if(it != m_clientList.end())
    {
        m_clientList.erase(it);
    }
    
    m_clientListMutex.unlock();
    Utility::XLog("DisconnectUnlock");
}

void ExternalInterfaceServer::BroadcastMessage(const std::string& message)
{
    Utility::XLog("BroadcastLock");
    m_clientListMutex.lock();
    Utility::XLog("BroadcastLockAcquired");
    
    for(std::pair<uint32_t, std::shared_ptr<ExternalInterfaceServerClient> > kp : m_clientList)
    {
        kp.second->SendMessage(message);
    }
    
    m_clientListMutex.unlock();
    Utility::XLog("BroadcastUnlock");
}