/*
 * File:   NetworkConnectViewModel.h
 * Author: christophe.delondre
 *
 * License: see LICENSE.xivap
 */

#ifndef NETWORKCONNECTVIEWMODEL_H
#define NETWORKCONNECTVIEWMODEL_H

#include <QObject>

class NetworkConnectViewModel : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString p_userId MEMBER m_userId NOTIFY dataChanged())
    Q_PROPERTY(QString p_password MEMBER m_password NOTIFY dataChanged())
    Q_PROPERTY(QString p_realName MEMBER m_realName NOTIFY dataChanged())
public:
    NetworkConnectViewModel();
    ~NetworkConnectViewModel();

    Q_INVOKABLE
    void        LoadFromConfiguration();

    Q_INVOKABLE
    bool        NetworkConnect();
    Q_INVOKABLE
    void        NetworkDisconnect();
signals:
    void        dataChanged();
private:
    QString     m_userId;
    QString     m_password;
    QString     m_realName;
};

extern NetworkConnectViewModel* sNetworkConnectViewModel;

#endif
