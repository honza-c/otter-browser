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

#include "messagemetadatasqltablemodel.h"

namespace Otter
{

QVariant MessageMetadataSqlTableModel::data(const QModelIndex &index, int role) const
{
    QModelIndex isSeenIndex = this->index(index.row(), 3, QModelIndex());
    bool isSeen = QSqlTableModel::data(isSeenIndex, Qt::DisplayRole).toInt() == 1;

    if (role == Qt::FontRole && !isSeen)
    {
        QFont font;
        font.setBold(true);
        return font;
    }
    else if (index.column() == 5 && role == Qt::DisplayRole)
    {
        QString timeData = QSqlTableModel::data(index, role).toString();
        return QDateTime::fromSecsSinceEpoch(timeData.toLong()).toString("dd.MM.yyyy HH:mm");
    }
    else if (index.column() == 6 && role == Qt::DisplayRole)
    {
        QString jsonString = QSqlTableModel::data(index, role).toString();
        Contact sender = Contact::contactFromJson(jsonString);

        if (sender.name() != QString())
        {
            return sender.name();
        }
        else
        {
            return sender.emailAddress();
        }
    }
    else
    {
        return QSqlTableModel::data(index, role);
    }
}

void MessageMetadataSqlTableModel::onTableHeaderClicked(int column)
{
    static bool ascending = false;

    if (ascending)
    {
        ascending = false;
        this->sort(column, Qt::DescendingOrder);
    }
    else
    {
        ascending = true;
        this->sort(column, Qt::AscendingOrder);
    }
}

void MessageMetadataSqlTableModel::currentRowChanged(const QModelIndex &index, const QModelIndex &previous)
{
    QModelIndex isSeenIndex = this->index(index.row(), 3, QModelIndex());

    if (QSqlTableModel::data(isSeenIndex, Qt::DisplayRole).toInt() == 0)
    {
        QSqlTableModel::setData(isSeenIndex, 1);
    }
}

QVariant MessageMetadataSqlTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal)
    {
        switch (section)
        {
        case 0:
            return "ID";
        case 5:
            return "Date";
        case 6:
            return "Sender";
        case 7:
            return "Size";
        case 8:
            return "Subject";
        default:
            return QVariant();
        }
    }

    return QVariant();
}

}
