/*
 * File:   ActiveFrequencyMessagesListModel.cpp
 * Author: christophe.delondre
 *
 * License: see LICENSE.xivap
 */

#include "ActiveFrequencyMessagesListModel.h"

ActiveFrequencyMessagesListModel::ActiveFrequencyMessagesListModel()
{

}

ActiveFrequencyMessagesListModel::~ActiveFrequencyMessagesListModel()
{

}

void ActiveFrequencyMessagesListModel::AppendMessage(QString message)
{
    int insertIndex = m_messages.size();

    beginInsertRows(QModelIndex(), insertIndex, insertIndex);

    m_messages.insert(m_messages.begin() + insertIndex, message);

    endInsertRows();
}

int ActiveFrequencyMessagesListModel::rowCount(const QModelIndex &parent) const
{
    return m_messages.size();
}

QVariant ActiveFrequencyMessagesListModel::data(const QModelIndex &index, int role) const
{
    if(index.row() >= m_messages.size())
        return QVariant();

    if(role == Qt::UserRole + 1)
        return m_messages[index.row()];

    return QVariant();
}

QHash<int, QByteArray> ActiveFrequencyMessagesListModel::roleNames() const
{
    QHash<int, QByteArray> roles;

    roles[Qt::UserRole + 1] = "messageText";

    return roles;
}
