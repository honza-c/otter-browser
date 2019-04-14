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

#include "inboxfoldertreeitem.h"

namespace Otter
{

InboxFolderTreeItem::InboxFolderTreeItem(const QList<QVariant> &data, InboxFolderTreeItem *parent)
{
    m_parentItem = parent;
    m_itemData = data;
}

InboxFolderTreeItem::~InboxFolderTreeItem()
{
    qDeleteAll(m_childItems);
}

void InboxFolderTreeItem::appendChild(InboxFolderTreeItem *item)
{
    m_childItems.append(item);
}

void InboxFolderTreeItem::setParent(InboxFolderTreeItem *parent)
{
    m_parentItem = parent;
}

InboxFolderTreeItem *InboxFolderTreeItem::child(int row)
{
    return m_childItems.value(row);
}

int InboxFolderTreeItem::childCount() const
{
    return m_childItems.count();
}

int InboxFolderTreeItem::row() const
{
    if (m_parentItem)
    {
        return m_parentItem->m_childItems.indexOf(const_cast<InboxFolderTreeItem*>(this));
    }

    return 0;
}

int InboxFolderTreeItem::columnCount() const
{
    return m_itemData.count();
}

QVariant InboxFolderTreeItem::data(int column) const
{
    return m_itemData.value(column);
}

InboxFolderTreeItem *InboxFolderTreeItem::parentItem()
{
    return m_parentItem;
}

void InboxFolderTreeItem::setData(QList<QVariant> data)
{
    m_itemData = data;
}

}
