/*
 * File:   ConnectViewModel.cpp
 * Author: christophe.delondre
 *
 * License: see LICENSE.xivap
 */

#include "ConnectViewModel.h"

#include "../configuration/ExternalUIConfiguration.h"
#include "../network/ExternalInterfaceNetworkClient.h"

#include <QDebug>

ConnectViewModel* sConnectViewModel = 0;

ConnectViewModel::ConnectViewModel()
: m_state(CONNECTVIEWMODEL_STATE_INPUT)
{
    connect(sExternalInterfaceNetworkClient.get(), SIGNAL(stateChanged()), this, SLOT(OnNetworkClientStateChanged()));
}

ConnectViewModel::~ConnectViewModel()
{

}

void ConnectViewModel::Connect(QString address, QString authorizationKey)
{
    if(m_state == CONNECTVIEWMODEL_STATE_INPUT)
    {
        m_state = CONNECTVIEWMODEL_STATE_CONNECTING;
        emit stateChanged();

        qDebug() << "Connect to " << address << " using " << authorizationKey;

        sExternalUIConfiguration->SetLastAddress(address);
        sExternalUIConfiguration->SetLastAuthorizationKey(authorizationKey);
        sExternalUIConfiguration->SaveConfiguration();

        sExternalInterfaceNetworkClient->SetAuthorizationKey(authorizationKey.toStdString());
        sExternalInterfaceNetworkClient->Connect(address.toStdString());
    }
}

void ConnectViewModel::Disconnect()
{
    if(m_state != CONNECTVIEWMODEL_STATE_INPUT)
    {
        sExternalInterfaceNetworkClient->Disconnect();

        m_state = CONNECTVIEWMODEL_STATE_INPUT;
        emit stateChanged();
    }
}

void ConnectViewModel::OnNetworkClientStateChanged()
{
    switch(sExternalInterfaceNetworkClient->GetState())
    {
    case EXTERNALINTERFACENETWORKCLIENT_STATE_IDLE:
        m_state = CONNECTVIEWMODEL_STATE_INPUT;
        break;
    case EXTERNALINTERFACENETWORKCLIENT_STATE_CONNECTING:
        m_state = CONNECTVIEWMODEL_STATE_CONNECTING;
        break;
    case EXTERNALINTERFACENETWORKCLIENT_STATE_CONNECTED:
        m_state = CONNECTVIEWMODEL_STATE_CONNECTED;
        break;
    case EXTERNALINTERFACENETWORKCLIENT_STATE_AUTHENTICATING:
        m_state = CONNECTVIEWMODEL_STATE_CONNECTING;
        break;
    case EXTERNALINTERFACENETWORKCLIENT_STATE_ESTABLISHED:
        m_state = CONNECTVIEWMODEL_STATE_ESTABLISHED;
        break;
    }

    emit stateChanged();
}
