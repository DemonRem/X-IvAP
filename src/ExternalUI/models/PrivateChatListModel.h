/*
 * File:   PrivateChatListModel.h
 * Author: christophe.delondre
 *
 * License: see LICENSE.xivap
 */

#ifndef PRIVATECHATLISTMODEL_H
#define PRIVATECHATLISTMODEL_H

#include <QAbstractListModel>
#include <stdint.h>

class PrivateChatMessage
{
public:
    bool        direction; // True = myself to endpoint, False = endpoint to myself.
    QString     textMessage;
    uint32_t    timestamp;
};

class PrivateChatListModel : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(QString p_endpointId MEMBER m_endpointId NOTIFY endpointIdChanged())
    Q_PROPERTY(bool p_hasUnreadMessages MEMBER m_hasUnreadMessages NOTIFY hasUnreadMessagesChanged())
public:
    PrivateChatListModel();
    ~PrivateChatListModel();

    enum PrivateChatListModelRoles
    {
        DirectionRole = Qt::UserRole + 1,
        TextMessageRole,
        TimestampRole
    };

    int                                 rowCount(const QModelIndex &parent) const;
    QVariant                            data(const QModelIndex &index, int role) const;
    QHash<int, QByteArray>              roleNames() const;

    QString                             GetEndpointId();
    void                                SetEndpointId(QString endpointId);

    void                                AppendMessage(bool direction, QString message);

    bool                                HasUnreadMessages();
    void                                SetHasUnreadMessages(bool hasUnreadMessages);
signals:
    void                                endpointIdChanged();
    void                                hasUnreadMessagesChanged();
private:
    QString                             m_endpointId;       // The identifier (usually callsign) of the remote endpoint.
    std::vector<PrivateChatMessage*>    m_messages;
    bool                                m_hasUnreadMessages;
};

#endif
