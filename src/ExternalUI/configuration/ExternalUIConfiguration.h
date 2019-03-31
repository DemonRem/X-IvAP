/*
 * File:   ExternalUIConfiguration.h
 * Author: christophe.delondre
 *
 * License: see LICENSE.xivap
 */

#ifndef EXTERNALUICONFIGURATION_H
#define EXTERNALUICONFIGURATION_H

#include <QSettings>

class ExternalUIConfiguration : public QObject
{
    Q_OBJECT
public:
    ExternalUIConfiguration();
    ~ExternalUIConfiguration();

    void            LoadConfiguration();
    void            SaveConfiguration();

    Q_INVOKABLE
    QString         GetLastAddress();
    Q_INVOKABLE
    QString         GetLastAuthorizationKey();

    Q_INVOKABLE
    void            SetLastAddress(QString address);
    Q_INVOKABLE
    void            SetLastAuthorizationKey(QString key);

    Q_INVOKABLE
    QString         GetLastNetworkUserId();
    Q_INVOKABLE
    void            SetLastNetworkUserId(QString userId);

    Q_INVOKABLE
    QString         GetLastNetworkPassword();
    Q_INVOKABLE
    void            SetLastNetworkPassword(QString password);

    Q_INVOKABLE
    QString         GetLastNetworkRealName();
    Q_INVOKABLE
    void            SetLastNetworkRealName(QString realName);
private:
    QSettings       m_qSettings;
private:
    QString         m_lastAddress;
    QString         m_lastAuthorizationKey;
    QString         m_lastNetworkUserId;
    QString         m_lastNetworkPassword;
    QString         m_lastNetworkRealName;
};

extern ExternalUIConfiguration* sExternalUIConfiguration;

#endif
