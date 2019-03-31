/*
 * File:   ExternalUIConfiguration.cpp
 * Author: christophe.delondre
 *
 * License: see LICENSE.xivap
 */

#include "ExternalUIConfiguration.h"

ExternalUIConfiguration* sExternalUIConfiguration = 0;

ExternalUIConfiguration::ExternalUIConfiguration()
: m_qSettings("X-IvAp", "X-IvAp External Interface")
{

}

ExternalUIConfiguration::~ExternalUIConfiguration()
{

}

void ExternalUIConfiguration::LoadConfiguration()
{
    m_lastAddress = m_qSettings.value("xivap/address").toString();
    m_lastAuthorizationKey = m_qSettings.value("xivap/key").toString();

    m_lastNetworkUserId = m_qSettings.value("network/user_id").toString();
    m_lastNetworkPassword = m_qSettings.value("network/password").toString();
    m_lastNetworkRealName = m_qSettings.value("network/real_name").toString();
}

void ExternalUIConfiguration::SaveConfiguration()
{
    m_qSettings.setValue("xivap/address", m_lastAddress);
    m_qSettings.setValue("xivap/key", m_lastAuthorizationKey);

    m_qSettings.setValue("network/user_id", m_lastNetworkUserId);
    m_qSettings.setValue("network/password", m_lastNetworkPassword);
    m_qSettings.setValue("network/real_name", m_lastNetworkRealName);
}

QString ExternalUIConfiguration::GetLastAddress()
{
    return m_lastAddress;
}

QString ExternalUIConfiguration::GetLastAuthorizationKey()
{
    return m_lastAuthorizationKey;
}

QString ExternalUIConfiguration::GetLastNetworkUserId()
{
    return m_lastNetworkUserId;
}

QString ExternalUIConfiguration::GetLastNetworkPassword()
{
    return m_lastNetworkPassword;
}

QString ExternalUIConfiguration::GetLastNetworkRealName()
{
    return m_lastNetworkRealName;
}

void ExternalUIConfiguration::SetLastAddress(QString address)
{
    m_lastAddress = address;
}

void ExternalUIConfiguration::SetLastAuthorizationKey(QString key)
{
    m_lastAuthorizationKey = key;
}

void ExternalUIConfiguration::SetLastNetworkUserId(QString userId)
{
    m_lastNetworkUserId = userId;
}

void ExternalUIConfiguration::SetLastNetworkPassword(QString password)
{
    m_lastNetworkPassword = password;
}

void ExternalUIConfiguration::SetLastNetworkRealName(QString realName)
{
    m_lastNetworkRealName = realName;
}
