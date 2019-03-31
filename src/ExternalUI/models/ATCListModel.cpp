/*
 * File:   ATCListModel.cpp
 * Author: christophe.delondre
 *
 * License: see LICENSE.xivap
 */

#include "ATCListModel.h"

#include <QDebug>

ATCListModel::ATCListModel()
{
    connect(this, SIGNAL(signalAddATC(ATCListPosition*)), this, SLOT(OnAddATC(ATCListPosition*)));
    connect(this, SIGNAL(signalClear()), this, SLOT(OnClear()));
}

ATCListModel::~ATCListModel()
{

}

int ATCListModel::GetSize()
{
    return m_atcList.size();
}

int ATCListModel::rowCount(const QModelIndex &parent) const
{
    return m_atcList.size();
}

QVariant ATCListModel::data(const QModelIndex &index, int role) const
{
    if(index.row() >= m_atcList.size())
        return QVariant();

    ATCListPosition* pPosition = m_atcList[index.row()];

    if(!pPosition)
        return QVariant();

    switch(role)
    {
        case CallsignRole: return pPosition->callsign;
        case FrequencyRole: return pPosition->frequency;
        case RealNameRole: return pPosition->realName;
        case UserIdRole: return pPosition->userId;
        case RatingRole: return pPosition->rating;
    }

    return QVariant();
}

QHash<int, QByteArray> ATCListModel::roleNames() const
{
    QHash<int, QByteArray> roles;

    roles[CallsignRole] = "callsign";
    roles[FrequencyRole] = "frequency";
    roles[RealNameRole] = "realName";
    roles[UserIdRole] = "userId";
    roles[RatingRole] = "rating";

    return roles;
}

void ATCListModel::AddATC(ATCListPosition *position)
{
    qDebug() << "Add ATC: " << position->callsign << " " << position->frequency << " " << position->realName << " " << position->realName << " " << position->rating;

    emit signalAddATC(position);
}

void ATCListModel::Clear()
{
    emit signalClear();
}

void ATCListModel::OnAddATC(ATCListPosition *position)
{
    int insertIndex = m_atcList.size();

    beginInsertRows(QModelIndex(), insertIndex, insertIndex);

    m_atcList.insert(m_atcList.begin() + insertIndex, position);

    endInsertRows();

    emit sizeChanged();
}

void ATCListModel::OnClear()
{
    beginResetModel();

    m_atcList.clear();

    endResetModel();

    emit sizeChanged();
}
