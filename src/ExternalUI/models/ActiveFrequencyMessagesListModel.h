/*
 * File:   ActiveFrequencyMessagesListModel.h
 * Author: christophe.delondre
 *
 * License: see LICENSE.xivap
 */

#ifndef ACTIVEFREQUENCYMESSAGESLISTMODEL_H
#define ACTIVEFREQUENCYMESSAGESLISTMODEL_H

#include <QAbstractListModel>

class ActiveFrequencyMessagesListModel : public QAbstractListModel
{
    Q_OBJECT
public:
    ActiveFrequencyMessagesListModel();
    ~ActiveFrequencyMessagesListModel();

    QVariant                data(const QModelIndex &index, int role) const;
    QHash<int, QByteArray>  roleNames() const;
    int                     rowCount(const QModelIndex &parent) const;

    void                    AppendMessage(QString message);
private:
    std::vector<QString>    m_messages;
};

#endif
