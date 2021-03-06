/**************************************************************************
* Otter Browser: Web browser controlled by the user, not vice-versa.
* Copyright (C) 2014 Piotr Wójcik <chocimier@tlen.pl>
* Copyright (C) 2014 - 2017 Michal Dutkiewicz aka Emdek <michal@emdek.pl>
* Copyright (C) 2018 Jan Čulík <jan@culik.net>
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program. If not, see <http://www.gnu.org/licenses/>.
*
**************************************************************************/

#include "emailaccountslistmodel.h"

namespace Otter
{

int EmailAccountsListModel::rowCount(const QModelIndex &parent) const
{
    return m_data->size();
}

QVariant EmailAccountsListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
    {
        return QVariant();
    }

    if (index.row() >= m_data->size())
    {
        return QVariant();
    }

    if (role == Qt::DisplayRole)
    {
        QString result;

        result.append(m_data->at(index.row()).contactName());
        result.append(" <");
        result.append(m_data->at(index.row()).emailAddress());
        result.append(">");

        return result;
    }
    else
    {
        return QVariant();
    }
}

bool EmailAccountsListModel::removeRows(int position, int rows, const QModelIndex &parent)
{
    beginRemoveRows(parent, position, position + rows - 1);

    m_data->removeAt(position);

    endRemoveRows();

    return true;
}

bool EmailAccountsListModel::insertRows(int position, int rows, const QModelIndex &parent)
{
    beginInsertRows(parent, position, position + rows - 1);

    m_data->insert(position, m_userAccountToAdd);

    endInsertRows();

    return true;
}

void EmailAccountsListModel::addAccount(const EmailAccount account)
{
    m_userAccountToAdd = account;
    insertRows(rowCount(), 1);
}

void EmailAccountsListModel::removeAccount(const int row)
{
    removeRows(row, 1, QModelIndex());
}

void EmailAccountsListModel::replaceAccount(const EmailAccount account, const QModelIndex index)
{
    if (index.isValid() && index.row() <= rowCount())
    {
        m_data->replace(index.row(), account);
        emit dataChanged(index, index);
    }
}

}
