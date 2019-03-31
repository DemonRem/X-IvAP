/*
 * File:   NetworkConnectViewModel.cpp
 * Author: christophe.delondre
 *
 * License: see LICENSE.xivap
 */

#include "NetworkConnectViewModel.h"
#include "../network/ExternalInterfaceNetworkClient.h"
#include "../configuration/ExternalUIConfiguration.h"

#include <QDebug>

NetworkConnectViewModel* sNetworkConnectViewModel = 0;

NetworkConnectViewModel::NetworkConnectViewModel()
{

}

NetworkConnectViewModel::~NetworkConnectViewModel()
{

}

void NetworkConnectViewModel::LoadFromConfiguration()
{
    m_userId = sExternalUIConfiguration->GetLastNetworkUserId();
    m_password = sExternalUIConfiguration->GetLastNetworkPassword();
    m_realName = sExternalUIConfiguration->GetLastNetworkRealName();

    emit dataChanged();
}

bool NetworkConnectViewModel::NetworkConnect()
{
    if(!m_userId.isEmpty() && !m_password.isEmpty() && !m_realName.isEmpty())
    {
        // Save credentials to configuration.
        sExternalUIConfiguration->SetLastNetworkUserId(m_userId);
        sExternalUIConfiguration->SetLastNetworkPassword(m_password);
        sExternalUIConfiguration->SetLastNetworkRealName(m_realName);
        sExternalUIConfiguration->SaveConfiguration();

        std::string connectMessage("XIVAPEI_CL_NETCONNECT|");
        connectMessage.append("SDV1234");
        connectMessage.append("|");
        connectMessage.append(m_userId.toStdString());
        connectMessage.append("|");
        connectMessage.append(m_password.toStdString());
        connectMessage.append("|");
        connectMessage.append(m_realName.toStdString());
        connectMessage.append("|");
        connectMessage.append("0"); // No voice for now.

        sExternalInterfaceNetworkClient->SendMessage(connectMessage);

        return true;
    }

    return false;
}

void NetworkConnectViewModel::NetworkDisconnect()
{
    sExternalInterfaceNetworkClient->SendMessage("XIVAPEI_CL_NETDISCONNECT");
}
