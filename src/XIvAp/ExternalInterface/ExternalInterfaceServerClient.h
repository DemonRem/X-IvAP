/* 
 * File:   ExternalInterfaceServerClient.h
 * Author: christophe.delondre
 *
 * License: see LICENSE.xivap
 */

#ifndef EXTERNALINTERFACESERVERCLIENT_H
#define EXTERNALINTERFACESERVERCLIENT_H

#include "asio.hpp"

#include <memory>

#define FRAME_LENGTH 1024

// Finite states
#define EXTERNALINTERFACESERVERCLIENT_STATE_CREATED         0       // The object was created but no socket is attached to it.
#define EXTERNALINTERFACESERVERCLIENT_STATE_ACCEPTED        1       // The client connection has been accepted and processed.
#define EXTERNALINTERFACESERVERCLIENT_STATE_LOGIN           2       // The remote client has identified as X-IvAp External Interface client
                                                                    // and we are waiting for authentication information.
#define EXTERNALINTERFACESERVERCLIENT_STATE_AUTHENTICATED   3       // Authentication information has been received and accepted. Client is authenticated.
#define EXTERNALINTERFACESERVERCLIENT_STATE_CLOSED          4       // Client has been closed and is waiting for trash collection.

class ExternalInterfaceServerClient : public std::enable_shared_from_this<ExternalInterfaceServerClient>
{
public:
    ExternalInterfaceServerClient(asio::ip::tcp::socket socket);
    ~ExternalInterfaceServerClient();
    
    asio::ip::tcp::socket&  GetSocket();
    void                    Start();
    void                    Stop();
    
    uint32_t                GetClientId();
    void                    SetClientId(uint32_t clientId);
    
    void                    SetServerPointer(void* ptr);
    void                    DeleteFromServer();
    
    void                    SendMessage(const std::string& message);
private:
    void                    BeginRead();
    void                    OnDisconnect();
    void                    CloseSocket();
private:
    asio::ip::tcp::socket   m_clientSocket;
    uint32_t                m_clientId;
    uint8_t                 m_state;
    unsigned char           m_requestBuffer[FRAME_LENGTH];
    void*                   m_server;
};

#endif /* EXTERNALINTERFACESERVERCLIENT_H */

