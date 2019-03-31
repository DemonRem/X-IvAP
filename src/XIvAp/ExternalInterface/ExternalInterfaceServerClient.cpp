/* 
 * File:   ExternalInterfaceServerClient.cpp
 * Author: christophe.delondre
 *
 * License: see LICENSE.xivap
 */

#include "ExternalInterfaceServerClient.h"
#include "ExternalInterfaceServer.h"
#include "ExternalInterfaceController.h"
#include "DataInterface.h"
#include "Utility.h"

#include "XPLMUtilities.h"

#include <stdlib.h>
#include <regex>
#include <fstream>
#include "../version.h"

ExternalInterfaceServerClient::ExternalInterfaceServerClient(asio::ip::tcp::socket socket)
: m_clientSocket(std::move(socket))
, m_clientId(0)
, m_state(EXTERNALINTERFACESERVERCLIENT_STATE_CREATED)
, m_server(0)
{
    asio::ip::tcp::no_delay option(true);
    m_clientSocket.set_option(option);
    
    memset(m_requestBuffer, 0, FRAME_LENGTH);
}

ExternalInterfaceServerClient::~ExternalInterfaceServerClient()
{
    
}

asio::ip::tcp::socket& ExternalInterfaceServerClient::GetSocket()
{
    return m_clientSocket;
}

uint32_t ExternalInterfaceServerClient::GetClientId()
{
    return m_clientId;
}

void ExternalInterfaceServerClient::SetClientId(uint32_t clientId)
{
    m_clientId = clientId;
}

void ExternalInterfaceServerClient::SetServerPointer(void* ptr)
{
    m_server = ptr;
}

void ExternalInterfaceServerClient::BeginRead()
{
    auto self(shared_from_this());
    
    m_clientSocket.async_read_some(asio::buffer(m_requestBuffer, FRAME_LENGTH), [this, self](std::error_code error, std::size_t length)
    {
        if(error)
        {
            Utility::XLog("disconnect p1\n");
            
            DeleteFromServer();
            
            Utility::XLog("disconnect p2\n");
            
            CloseSocket();
            
            Utility::XLog("disconnect p3\n");
                       
            OnDisconnect();
            
            Utility::XLog("disconnect p4\n");
        }
        else
        {
            // TODO: multipart messages - too large buffer is a pain in the ass ...
            std::string recvMessage((char*)m_requestBuffer, length);
            
            Utility::XLog("Recv Message: %s", recvMessage.c_str());
                        
            if(m_state == EXTERNALINTERFACESERVERCLIENT_STATE_ACCEPTED)
            {
                // This client just came in - need to check if it is identifying as X-IvAp External Interface Client.
                if(recvMessage != "XIVAPEI_CL_HELO")
                {
                    Stop(); // intruder alert!
                    return;
                }
                
                // You need to authenticate sir.
                SendMessage("XIVAPEI_SV_LOGIN"); // TODO: watch out for pending non-authenticated connections ...
                
                m_state = EXTERNALINTERFACESERVERCLIENT_STATE_LOGIN;
            }
            else if(m_state == EXTERNALINTERFACESERVERCLIENT_STATE_LOGIN)
            {
                // Has to be authentication information? Better be.
                if(recvMessage.find("XIVAPEI_CL_LOGIN|") != 0)
                {
                    Stop(); // intruder alert! how did he get there ? ...
                    return;
                }
                
                std::vector<std::string> explodedMessage = Utility::str_explode(recvMessage, '|');
                
                if(explodedMessage.size() != 2)
                {
                    Stop(); // can't be right ...
                    return;
                }
                
                std::string loginKey = explodedMessage[1];
                
                // There we are.
                Utility::XLog("ExternalInterfaceServerClient: authentication attempt using key %s\n", loginKey.c_str());
                
                if(loginKey != "xivap")
                {
                    SendMessage("XIVAPEI_SV_LOGIN_FAILED");
                    Stop();
                    return;
                }
                
                m_state = EXTERNALINTERFACESERVERCLIENT_STATE_AUTHENTICATED;
                
                std::string heloMessage = "XIVAPEI_SV_HELO|";
                heloMessage.append(SOFTWARE_VERSION);
                heloMessage.append("|");
#ifdef IVAO
                heloMessage.append("IVAO");
#else
#ifdef XFS
                heloMessage.append("XFS");
#else
                heloMessage.append("UNK");
#endif
#endif
                
                SendMessage(heloMessage);
                
                // Broadcast a fresh set of data.
                sDataInterface->BroadcastDatum("*");
                sExternalInterfaceController->BroadcastATCList();
            }
            else if(m_state == EXTERNALINTERFACESERVERCLIENT_STATE_AUTHENTICATED)
            {
                Utility::XLog("ExternalInterfaceServerClient: received message:%s\n", recvMessage.c_str());
                
                std::vector<std::string> explodedMessage = Utility::str_explode(recvMessage, '|');
                
                if(explodedMessage[0] == "XIVAPEI_CL_NETCONNECT")
                {
                    if(explodedMessage.size() >= 6)
                    {
                        std::string callsign = explodedMessage[1];
                        std::string userId = explodedMessage[2];
                        std::string password = explodedMessage[3];
                        std::string realName = explodedMessage[4];
                        bool voice = atoi(explodedMessage[5].c_str()) == 1;
                        
                        sExternalInterfaceController->NetworkConnect(callsign, userId, password, realName, "178.62.198.250", 6809, voice, false);
                    }
                }
                else if(explodedMessage[0] == "XIVAPEI_CL_NETDISCONNECT")
                {
                    // Disconnect from network.
                    sExternalInterfaceController->NetworkDisconnect();
                }
                else if(explodedMessage[0] == "XIVAPEI_CL_XPDR_MODE_TOGGLE")
                {
                    sExternalInterfaceController->XPDRModeToggle();
                }
                else if(explodedMessage[0] == "XIVAPEI_CL_XPDR_IDENT")
                {
                    sExternalInterfaceController->XPDRIdent();
                }
                else if(explodedMessage[0] == "XIVAPEI_CL_XPDR_CODE_A")
                {
                    if(explodedMessage.size() >= 2)
                    {
                        int code = atoi(explodedMessage[1].c_str());
                        
                        sExternalInterfaceController->XPDRSetCodeA(code);
                    }
                }
                else if(explodedMessage[0] == "XIVAPEI_CL_RADIOTUNE")
                {
                    if(explodedMessage.size() >= 3)
                    {
                        int radioIndex = atoi(explodedMessage[1].c_str());
                        int freq = atoi(explodedMessage[2].c_str());
                        
                        sExternalInterfaceController->TuneRadio(radioIndex, freq);
                    }
                }
                else if(explodedMessage[0] == "XIVAPEI_CL_RADIOACTIVE")
                {
                    if(explodedMessage.size() >= 2)
                    {
                        int radioIndex = atoi(explodedMessage[1].c_str());
                        
                        sExternalInterfaceController->SetActiveRadio(radioIndex);
                    }
                }
                else if(explodedMessage[0] == "XIVAPEI_CL_TEXTMESSAGE")
                {
                    if(explodedMessage.size() >= 3)
                    {
                        std::string destination = explodedMessage[1];
                        std::string textMessage = explodedMessage[2];
                        
                        sExternalInterfaceController->SendMessageTo(destination, textMessage);
                    }
                }
                else if(explodedMessage[0] == "XIVAPEI_CL_IVAO_WX")
                {
                    if(explodedMessage.size() >= 2)
                    {
                        bool useWeather = explodedMessage[1] == "1";
                        
                        sExternalInterfaceController->SetUseIVAOWeather(useWeather);
                    }
                }
                else if(explodedMessage[0] == "XIVAPEI_CL_TAGGING")
                {
                    if(explodedMessage.size() >= 2)
                    {
                        bool useTagging = explodedMessage[1] == "1";
                        
                        sExternalInterfaceController->SetTagging(useTagging);
                    }
                }
                else if(explodedMessage[0] == "XIVAPEI_CL_DATUMREQUEST")
                {
                    if(explodedMessage.size() >= 2)
                    {
                        std::string datumId = explodedMessage[1];
                        sDataInterface->BroadcastDatum(datumId);
                    }
                }
            }
            
            BeginRead();
        }
    });
}

void ExternalInterfaceServerClient::SendMessage(const std::string& message)
{
    std::string socketMessage = message;
    socketMessage.append("\n");
    
    Utility::XLog("Sending message length %d contents %s\n", socketMessage.length(), socketMessage.c_str());
    
    auto self(shared_from_this());
    asio::async_write(m_clientSocket, asio::buffer(socketMessage.c_str(), socketMessage.length()), [this, self](std::error_code ec, std::size_t len)
    {
        if(ec)
        {
            Utility::XLog("ExternalInterfaceServerClient: failed to send data, error was: %s\n", ec.message().c_str());
        }
        
        Utility::XLog("ExternalInterfaceServerClient: sent %d bytes\n", len);
    });
}

void ExternalInterfaceServerClient::OnDisconnect()
{
    Utility::XLog("ExternalInterfaceServerClient: disconnect, id = %d\n", m_clientId);
    
    m_state = EXTERNALINTERFACESERVERCLIENT_STATE_CLOSED;
    
    // do cleaning and notifying work here.
}

void ExternalInterfaceServerClient::DeleteFromServer()
{
    ((ExternalInterfaceServer*)m_server)->OnClientDisconnect(m_clientId);
}

void ExternalInterfaceServerClient::Start()
{
    m_state = EXTERNALINTERFACESERVERCLIENT_STATE_ACCEPTED;
    
    BeginRead();
}

void ExternalInterfaceServerClient::Stop()
{
    CloseSocket();
}

void ExternalInterfaceServerClient::CloseSocket()
{
    if(m_clientSocket.is_open())
    {
        Utility::XLog("closing socket\n");
        std::error_code err;
        m_clientSocket.close(err);
        if(err)
        {
            Utility::XLog("socket close error: %s\n", err.message().c_str());
        }
        Utility::XLog("socket closed\n");
    }
}