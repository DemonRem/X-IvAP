/*
 * File:   ConnectViewModel.h
 * Author: christophe.delondre
 *
 * License: see LICENSE.xivap
 */

#ifndef CONNECTVIEWMODEL_H
#define CONNECTVIEWMODEL_H

#include <QObject>

// Finite States
#define CONNECTVIEWMODEL_STATE_INPUT            0
#define CONNECTVIEWMODEL_STATE_CONNECTING       1
#define CONNECTVIEWMODEL_STATE_CONNECTED        2
#define CONNECTVIEWMODEL_STATE_AUTHENTICATING   3
#define CONNECTVIEWMODEL_STATE_ESTABLISHED      4

class ConnectViewModel : public QObject
{
    Q_OBJECT

    Q_PROPERTY(int p_state MEMBER m_state NOTIFY stateChanged())
public:
    ConnectViewModel();
    ~ConnectViewModel();

    Q_INVOKABLE
    void        Connect(QString address, QString authorizationKey);

    Q_INVOKABLE
    void        Disconnect();
signals:
    void        stateChanged();
private slots:
    void        OnNetworkClientStateChanged();
private:
    int         m_state;
};

extern ConnectViewModel* sConnectViewModel;

#endif
