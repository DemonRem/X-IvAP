/*
 * File:   ExternalInterfaceNetworkClient.cpp
 * Author: christophe.delondre
 *
 * License: see LICENSE.xivap
 */

#include "ExternalInterfaceNetworkClient.h"
#include "ASIOCore.h"
#include <regex>

#include "../viewmodels/MainViewModel.h"

#include "../Utility.h"
#include "../SimulatorData.h"

#include <QDebug>

std::shared_ptr<ExternalInterfaceNetworkClient> sExternalInterfaceNetworkClient = 0;

ExternalInterfaceNetworkClient::ExternalInterfaceNetworkClient()
: m_socket(sASIOCoreIOService)
, m_state(EXTERNALINTERFACENETWORKCLIENT_STATE_IDLE)
{
    memset(m_requestBuffer, 0, FRAME_LENGTH);
}

ExternalInterfaceNetworkClient::~ExternalInterfaceNetworkClient()
{

}

int ExternalInterfaceNetworkClient::GetState()
{
    return m_state;
}

void ExternalInterfaceNetworkClient::SetAuthorizationKey(const std::string &key)
{
    m_authorizationKey = key;
}

void ExternalInterfaceNetworkClient::Connect(const std::string &address)
{
    if(m_state == EXTERNALINTERFACENETWORKCLIENT_STATE_IDLE)
    {
        SetState(EXTERNALINTERFACENETWORKCLIENT_STATE_CONNECTING);

        auto self(shared_from_this());
        m_socket.async_connect(asio::ip::tcp::endpoint(asio::ip::address_v4::from_string(address), 4827), [this,self](std::error_code ec)
        {
            if(ec)
            {
                QString alertMessage = QString("Failed to connect, error: %1 - %2").arg(ec.value()).arg(QString::fromStdString(ec.message()));
                qDebug() << alertMessage;

                sMainViewModel->ShowAlert(alertMessage.toStdString());

                m_socket.close();

                SetState(EXTERNALINTERFACENETWORKCLIENT_STATE_IDLE);
            }
            else
            {
                qDebug() << "Connected";

                asio::ip::tcp::no_delay option(true);
                m_socket.set_option(option);

                SetState(EXTERNALINTERFACENETWORKCLIENT_STATE_CONNECTED);

                // Start reading.
                BeginRead();

                // Send HELO message.
                // TODO Send client version info here.
                SendMessage("XIVAPEI_CL_HELO");
            }
        });
    }
}

void ExternalInterfaceNetworkClient::Disconnect()
{
    if(m_state != EXTERNALINTERFACENETWORKCLIENT_STATE_IDLE && m_socket.is_open())
        m_socket.close();
}

void ExternalInterfaceNetworkClient::BeginRead()
{
    auto self(shared_from_this());

    m_socket.async_read_some(asio::buffer(m_requestBuffer, FRAME_LENGTH), [this, self](std::error_code error, std::size_t length)
    {
        if(error)
        {
            qDebug() << "Failed to receive, error: " << error.value() << " - " << QString::fromStdString(error.message());

            m_socket.close();

            SetState(EXTERNALINTERFACENETWORKCLIENT_STATE_IDLE);
        }
        else
        {
            std::string recvString((char*)m_requestBuffer, length);

            qDebug() << "Recv: " << QString::fromStdString(recvString);

            std::vector<std::string> recvMessages = Utility::str_explode(recvString, '\n');

            std::istringstream messageStream(recvString);
            std::string recvMessage;
            while(std::getline(messageStream, recvMessage))
            {
                if(m_state == EXTERNALINTERFACENETWORKCLIENT_STATE_CONNECTED)
                {
                    if(recvMessage != "XIVAPEI_SV_LOGIN")
                    {
                        // This aint right.
                        Disconnect();
                        return;
                    }

                    qDebug() << "Sending Authentication ...";

                    // Send Login Information
                    std::string authMessage = "XIVAPEI_CL_LOGIN|";
                    authMessage.append(m_authorizationKey);

                    SetState(EXTERNALINTERFACENETWORKCLIENT_STATE_AUTHENTICATING);

                    SendMessage(authMessage);

                }
                else if(m_state == EXTERNALINTERFACENETWORKCLIENT_STATE_AUTHENTICATING)
                {
                    if(recvMessage == "XIVAP_SV_LOGIN_FAILED")
                    {
                        sMainViewModel->ShowAlert("Invalid authorization key.");
                        Disconnect();
                        return;
                    }
                    else if(recvMessage.find("XIVAPEI_SV_HELO") == 0)
                    {
                        // Read version info.
                        std::vector<std::string> explodedMessage = Utility::str_explode(recvMessage, '|');

                        if(explodedMessage.size() >= 3)
                        {
                            QString softwareVersion = QString::fromStdString(explodedMessage[1]);
                            QString softwareBranch = QString::fromStdString(explodedMessage[2]);

                            sSimulatorData->SetSoftwareInfo(softwareVersion, softwareBranch);
                        }

                        SetState(EXTERNALINTERFACENETWORKCLIENT_STATE_ESTABLISHED);
                    }
                }
                else if(m_state == EXTERNALINTERFACENETWORKCLIENT_STATE_ESTABLISHED)
                {
                    std::vector<std::string> explodedMessage = Utility::str_explode(recvMessage, '|');

                    std::string messageIdent = explodedMessage[0];

                    if(messageIdent == "XIVAPEI_SV_UIMESSAGE")
                    {
                        if(explodedMessage.size() > 1)
                        {
                            // Recompose message just in case someone sent a pipe as part of his text message.
                            std::string textMessage = explodedMessage[1];

                            if(explodedMessage.size() > 2)
                            {
                                for(int i = 2; i < explodedMessage.size(); i++)
                                {
                                    textMessage.append("|");
                                    textMessage.append(explodedMessage[i]);
                                }
                            }

                            if(textMessage.find("[prv]") == std::string::npos)
                                sMainViewModel->AppendActiveFrequencyMessage(QString::fromStdString(textMessage));
                        }
                    }
                    else if(messageIdent == "XIVAPEI_SV_TEXTMESSAGE")
                    {
                        if(explodedMessage.size() >= 4)
                        {
                            std::string source = explodedMessage[1];
                            std::string dest = explodedMessage[2];
                            std::string textMessage = explodedMessage[3];

                            sMainViewModel->ProcessInboundTextMessage(QString::fromStdString(source), QString::fromStdString(dest), QString::fromStdString(textMessage));
                        }
                    }
                    else if(messageIdent == "XIVAPEI_SV_ATCLIST")
                    {
                        if(explodedMessage.size() >= 2)
                        {
                            int nbPositions = atoi(explodedMessage[1].c_str());

                            if(explodedMessage.size() >= nbPositions + 2)
                            {
                                std::vector<ATCListPosition*> vPositions;

                                for(int i = 2; i < nbPositions + 2; i++)
                                {
                                    std::vector<std::string> explodedPosition = Utility::str_explode(explodedMessage[i], ',');

                                    if(explodedPosition.size() >= 5)
                                    {
                                        ATCListPosition* position = new ATCListPosition();

                                        position->callsign = QString::fromStdString(explodedPosition[0]);
                                        position->frequency = QString::fromStdString(explodedPosition[1]);
                                        position->realName = QString::fromStdString(explodedPosition[2]);
                                        position->userId = QString::fromStdString(explodedPosition[3]);
                                        position->rating = atoi(explodedPosition[4].c_str());

                                        vPositions.push_back(position);
                                    }
                                }

                                sMainViewModel->GetATCListModel()->Clear();
                                for(ATCListPosition* position : vPositions)
                                {
                                    sMainViewModel->GetATCListModel()->AddATC(position);
                                }
                            }
                        }
                    }
                    else if(messageIdent == "XIVAPEI_SV_DATUM")
                    {
                        // This is a datum. Extract ID and value.
                        if(explodedMessage.size() >= 3)
                        {
                            std::string datumId = explodedMessage[1];
                            std::string datumValue = explodedMessage[2];

                            // TODO move this somewhere else
                            if(datumId == "NETWORK_STATUS")
                            {
                                int value = atoi(datumValue.c_str());
                                sSimulatorData->SetNetworkStatus(value);
                            }
                            else if(datumId == "XPDR_CODE_A")
                            {
                                int value = atoi(datumValue.c_str());
                                sSimulatorData->SetXPDRCodeA(value);
                            }
                            else if(datumId == "XPDR_MODE")
                            {
                                int value = atoi(datumValue.c_str());
                                sSimulatorData->SetXPDRMode(value);
                            }
                            else if(datumId == "RADIO_FREQ_COM1")
                            {
                                sSimulatorData->SetCOM1Freq(QString::fromStdString(datumValue));
                            }
                            else if(datumId == "RADIO_NAME_COM1")
                            {
                                sSimulatorData->SetCOM1Name(QString::fromStdString(datumValue));
                            }
                            else if(datumId == "RADIO_FREQ_COM2")
                            {
                                sSimulatorData->SetCOM2Freq(QString::fromStdString(datumValue));
                            }
                            else if(datumId == "RADIO_NAME_COM2")
                            {
                                sSimulatorData->SetCOM2Name(QString::fromStdString(datumValue));
                            }
                            else if(datumId == "RADIO_ACTIVE")
                            {
                                int value = atoi(datumValue.c_str());
                                sSimulatorData->SetRadioActive(value);
                            }
                            else if(datumId == "IVAO_WX")
                            {
                                sSimulatorData->SetIVAOUseWeather(datumValue == "1");
                            }
                            else if(datumId == "TAGGING")
                            {
                                sSimulatorData->SetTagging(datumValue == "1");
                            }
                            else if(datumId == "NETWORK_CALLSIGN")
                            {
                                sSimulatorData->SetNetworkCallsign(QString::fromStdString(datumValue));
                            }
                        }
                    }
                    else
                    {
                        qDebug() << "Recv Message: " << QString::fromStdString(recvMessage);
                    }
                }
            }

            BeginRead();
        }
    });
}

void ExternalInterfaceNetworkClient::SetState(int state)
{
    m_state = state;

    emit stateChanged();
}

void ExternalInterfaceNetworkClient::SendMessage(const std::string &message)
{
    m_socket.send(asio::buffer(message.c_str(), message.length()));
}
