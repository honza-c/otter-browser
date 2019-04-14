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

#ifndef INBOXFOLDERTREEITEM_H
#define INBOXFOLDERTREEITEM_H

#include <QObject>
#include <QVariant>

namespace Otter
{

class InboxFolderTreeItem
{
public:
    explicit InboxFolderTreeItem(const QList<QVariant> &data, InboxFolderTreeItem *parent = nullptr);
    ~InboxFolderTreeItem();

    void appendChild(InboxFolderTreeItem *item);
    void setParent(InboxFolderTreeItem *parent);
    InboxFolderTreeItem *child(int row);
    int childCount() const;
    int columnCount() const;
    QVariant data(int column) const;
    int row() const;
    InboxFolderTreeItem *parentItem();
    void setData(QList<QVariant> data);

signals:

public slots:

private:

    QList<InboxFolderTreeItem*> m_childItems;
    QList<QVariant> m_itemData;
    InboxFolderTreeItem *m_parentItem;
};

}

#endif // INBOXFOLDERTREEITEM_H
