/*
 * File:   ATCListModel.h
 * Author: christophe.delondre
 *
 * License: see LICENSE.xivap
 */

#ifndef ATCLISTMODEL_H
#define ATCLISTMODEL_H

#include <QAbstractListModel>

class ATCListPosition
{
public:
    QString     callsign;
    QString     frequency;
    QString     realName;
    QString     userId;
    int         rating;
};

class ATCListModel : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(int p_size READ GetSize() NOTIFY sizeChanged())
public:
    ATCListModel();
    ~ATCListModel();

    enum ATCListModelRoles
    {
        CallsignRole = Qt::UserRole + 1,
        FrequencyRole,
        RealNameRole,
        UserIdRole,
        RatingRole
    };

    QVariant data(const QModelIndex &index, int role) const;
    int     rowCount(const QModelIndex &parent) const;
    QHash<int, QByteArray> roleNames() const;

    void    AddATC(ATCListPosition* position);
    void    Clear();
    int     GetSize();
signals:
    void    signalAddATC(ATCListPosition*);
    void    signalClear();
    void    sizeChanged();
private slots:
    void    OnAddATC(ATCListPosition* position);
    void    OnClear();
private:
    std::vector<ATCListPosition*>  m_atcList;
};

#endif
