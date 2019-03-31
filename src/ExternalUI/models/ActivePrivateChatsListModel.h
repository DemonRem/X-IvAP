/*
 * File:   ActivePrivateChatsListModel.h
 * Author: christophe.delondre
 *
 * License: see LICENSE.xivap
 */

#ifndef ACTIVEPRIVATECHATSLISTMODEL_H
#define ACTIVEPRIVATECHATSLISTMODEL_H

#include <QAbstractListModel>

#include "PrivateChatListModel.h"

class ActivePrivateChatsListModel : public QAbstractListModel
{
    Q_OBJECT
public:
    ActivePrivateChatsListModel();
    ~ActivePrivateChatsListModel();

    enum ActivePrivateChatsListModelRoles
    {
        EndpointIdRole = Qt::UserRole + 1,
        PrivateChatModelRole,
        HasUnreadMessagesRole
    };

    int                                     rowCount(const QModelIndex &parent) const;
    QVariant                                data(const QModelIndex &index, int role) const;
    QHash<int, QByteArray>                  roleNames() const;

    void                                    CreateActiveChatWithEndpoint(QString endpoint);
    bool                                    HasActiveChatWithEndpoint(QString endpoint);
    PrivateChatListModel*                   GetPrivateChatModelByEndpoint(QString endpoint);
private:
    std::vector<PrivateChatListModel*>      m_activeChats;
};

#endif
