/*
 * File:   ActivePrivateChatsListModel.cpp
 * Author: christophe.delondre
 *
 * License: see LICENSE.xivap
 */

#include "ActivePrivateChatsListModel.h"

ActivePrivateChatsListModel::ActivePrivateChatsListModel()
{

}

ActivePrivateChatsListModel::~ActivePrivateChatsListModel()
{

}

int ActivePrivateChatsListModel::rowCount(const QModelIndex &parent) const
{
    return m_activeChats.size();
}

QVariant ActivePrivateChatsListModel::data(const QModelIndex &index, int role) const
{
    if(index.row() >= m_activeChats.size())
        return QVariant();

    PrivateChatListModel* pModel = m_activeChats[index.row()];

    if(!pModel)
        return QVariant();

    switch(role)
    {
    case EndpointIdRole: return pModel->GetEndpointId();
    case PrivateChatModelRole: return QVariant::fromValue(pModel);
    case HasUnreadMessagesRole: return pModel->HasUnreadMessages();
    }

    return QVariant();
}

QHash<int, QByteArray> ActivePrivateChatsListModel::roleNames() const
{
    QHash<int, QByteArray> roles;

    roles[EndpointIdRole] = "endpointId";
    roles[PrivateChatModelRole] = "privateChatModel";
    roles[HasUnreadMessagesRole] = "hasUnreadMessages";

    return roles;
}

void ActivePrivateChatsListModel::CreateActiveChatWithEndpoint(QString endpoint)
{
    if(!HasActiveChatWithEndpoint(endpoint))
    {
        PrivateChatListModel* newChat = new PrivateChatListModel();

        newChat->SetEndpointId(endpoint);

        beginInsertRows(QModelIndex(), m_activeChats.size(), m_activeChats.size());

        m_activeChats.push_back(newChat);

        endInsertRows();
    }
}

bool ActivePrivateChatsListModel::HasActiveChatWithEndpoint(QString endpoint)
{
    bool found = false;

    for(PrivateChatListModel* pModel : m_activeChats)
    {
        if(pModel->GetEndpointId() == endpoint)
        {
            found = true;
            break;
        }
    }

    return found;
}

PrivateChatListModel* ActivePrivateChatsListModel::GetPrivateChatModelByEndpoint(QString endpoint)
{
    for(PrivateChatListModel* pModel : m_activeChats)
    {
        if(pModel->GetEndpointId() == endpoint)
            return pModel;
    }

    return 0;
}
