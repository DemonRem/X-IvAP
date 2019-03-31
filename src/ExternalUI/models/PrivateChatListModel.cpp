/*
 * File:   PrivateChatListModel.cpp
 * Author: christophe.delondre
 *
 * License: see LICENSE.xivap
 */

#include "PrivateChatListModel.h"

PrivateChatListModel::PrivateChatListModel()
{
    m_hasUnreadMessages = false;
}

PrivateChatListModel::~PrivateChatListModel()
{

}

int PrivateChatListModel::rowCount(const QModelIndex &parent) const
{
    return m_messages.size();
}

QVariant PrivateChatListModel::data(const QModelIndex &index, int role) const
{
    if(index.row() >= m_messages.size())
        return QVariant();

    PrivateChatMessage* pMessage = m_messages[index.row()];

    if(!pMessage)
        return QVariant();

    switch(role)
    {
    case DirectionRole: return pMessage->direction;
    case TextMessageRole: return pMessage->textMessage;
    case TimestampRole: return pMessage->timestamp;
    }

    return QVariant();
}

QHash<int, QByteArray> PrivateChatListModel::roleNames() const
{
    QHash<int, QByteArray> roles;

    roles[DirectionRole] = "direction";
    roles[TextMessageRole] = "textMessage";
    roles[TimestampRole] = "timestamp";

    return roles;
}

QString PrivateChatListModel::GetEndpointId()
{
    return m_endpointId;
}

void PrivateChatListModel::SetEndpointId(QString endpointId)
{
    m_endpointId = endpointId;
    emit endpointIdChanged();
}

void PrivateChatListModel::AppendMessage(bool direction, QString message)
{
    PrivateChatMessage* pMessage = new PrivateChatMessage();

    pMessage->direction = direction;
    pMessage->textMessage = message;
    pMessage->timestamp = time(0);

    beginInsertRows(QModelIndex(), m_messages.size(), m_messages.size());

    m_messages.push_back(pMessage);

    endInsertRows();
}

bool PrivateChatListModel::HasUnreadMessages()
{
    return m_hasUnreadMessages;
}

void PrivateChatListModel::SetHasUnreadMessages(bool hasUnreadMessages)
{
    m_hasUnreadMessages = hasUnreadMessages;

    emit hasUnreadMessagesChanged();
}
