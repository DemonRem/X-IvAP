/*
 * File:   ExternalInterfaceNetworkClient.h
 * Author: christophe.delondre
 *
 * License: see LICENSE.xivap
 */

#ifndef EXTERNALINTERFACENETWORKCLIENT_H
#define EXTERNALINTERFACENETWORKCLIENT_H

#include <QObject>

#include "asio.hpp"

#define FRAME_LENGTH 1024

// Finite States
#define EXTERNALINTERFACENETWORKCLIENT_STATE_IDLE           0
#define EXTERNALINTERFACENETWORKCLIENT_STATE_CONNECTING     1
#define EXTERNALINTERFACENETWORKCLIENT_STATE_CONNECTED      2
#define EXTERNALINTERFACENETWORKCLIENT_STATE_AUTHENTICATING 3
#define EXTERNALINTERFACENETWORKCLIENT_STATE_ESTABLISHED    4

// This client handles automatic reconnection while in STARTED state.
class ExternalInterfaceNetworkClient : public QObject, public std::enable_shared_from_this<ExternalInterfaceNetworkClient>
{
    Q_OBJECT
public:
    ExternalInterfaceNetworkClient();
    ~ExternalInterfaceNetworkClient();

    int                     GetState();

    // Sets the authorization key to be sent to X-IvAp when connecting establishes.
    // Must be called prior to calling Connect.
    void                    SetAuthorizationKey(const std::string& key);

    void                    Connect(const std::string& address);
    void                    Disconnect();

    void                    SendMessage(const std::string& message);
signals:
    void                    stateChanged();
private:
    void                    BeginRead();
    void                    SetState(int state);
private:
    asio::ip::tcp::socket   m_socket;
    int                     m_state;
    std::string             m_authorizationKey;
    unsigned char           m_requestBuffer[FRAME_LENGTH];
};

extern std::shared_ptr<ExternalInterfaceNetworkClient> sExternalInterfaceNetworkClient;

#endif
