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

#include "inboxfoldertreemodel.h"

namespace Otter
{

InboxFolderTreeModel::InboxFolderTreeModel(const QList<InboxFolder> &data, QObject *parent) : QAbstractItemModel(parent)
{
    QList<QVariant> captionData;
    captionData << "Folders";
    m_rootItem = new InboxFolderTreeItem(captionData);

    createFolderTreeStructure(data);
}

InboxFolderTreeModel::~InboxFolderTreeModel()
{
    delete m_rootItem;
}

void InboxFolderTreeModel::createFolderTreeStructure(QList<InboxFolder> data)
{
    QList<QList<InboxFolder>> inboxFoldersPerAccount;

    QList<InboxFolder> *inboxFoldersForCurrentAccount = new QList<InboxFolder>();

    for (int i = 0; i < data.size(); i++)
    {
        if (inboxFoldersForCurrentAccount->size() == 0)
        {
            inboxFoldersForCurrentAccount->push_back(data.at(i));
        }
        else if (inboxFoldersForCurrentAccount->first().emailAddress() == data.at(i).emailAddress())
        {
            inboxFoldersForCurrentAccount->push_back(data.at(i));
        }
        else
        {
            inboxFoldersPerAccount.push_back(*inboxFoldersForCurrentAccount);
            inboxFoldersForCurrentAccount = new QList<InboxFolder>();
            inboxFoldersForCurrentAccount->push_back(data.at(i));
        }
    }

    if (inboxFoldersForCurrentAccount->size() > 0)
    {
        inboxFoldersPerAccount.push_back(*inboxFoldersForCurrentAccount);
    }

    for (QList<InboxFolder> inboxFoldersForCurrentAccount : inboxFoldersPerAccount)
    {
        QList<QVariant> rootItemData;
        InboxFolder rootFolder = inboxFoldersForCurrentAccount.at(0);
        QIcon rootFolderIcon = rootFolder.getIcon();
        rootItemData << rootFolder.emailAddress() << rootFolderIcon << 0;

        InboxFolderTreeItem *rootItem = new InboxFolderTreeItem(rootItemData);

        int level = 0;
        InboxFolderTreeItem *previousItem = rootItem;

        for (int i = 1; i < inboxFoldersForCurrentAccount.length(); i++)
        {
            InboxFolder folder = inboxFoldersForCurrentAccount.at(i);

            if (folder.path() == "/")
            {
                continue;
            }

            int currentLevel = getLevel(folder.path());
            QString name = getNameFromPath(folder.path());
            QIcon icon = folder.getIcon();

            QList<QVariant> folderData;
            folderData << name << icon << DatabaseManager::getCountOfUnreadMessagesForFolder(folder.emailAddress(), folder.path());

            if (currentLevel == level + 1)
            {
                level++;
                InboxFolderTreeItem *item = new InboxFolderTreeItem(folderData, previousItem);
                previousItem->appendChild(item);
                previousItem = item;
            }
            else
            {
                while (currentLevel != level + 1)
                {
                    level--;
                    previousItem = previousItem->parentItem();
                }

                level++;
                InboxFolderTreeItem *item = new InboxFolderTreeItem(folderData, previousItem);
                previousItem->appendChild(item);
                previousItem = item;
            }
        }

        rootItem->setParent(m_rootItem);
        m_rootItem->appendChild(rootItem);
    }
}

int InboxFolderTreeModel::getLevel(const QString path) const
{
    return path.count('/');
}

QString InboxFolderTreeModel::getNameFromPath(const QString path) const
{
    return path.split('/').last();
}

QModelIndex InboxFolderTreeModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent))
    {
        return QModelIndex();
    }

    InboxFolderTreeItem *parentItem;

    if (!parent.isValid())
    {
        parentItem = m_rootItem;
    }
    else
    {
        parentItem = static_cast<InboxFolderTreeItem*>(parent.internalPointer());
    }

    InboxFolderTreeItem *childItem = parentItem->child(row);

    if (childItem)
    {
        return createIndex(row, column, childItem);
    }
    else
    {
        return QModelIndex();
    }
}

QModelIndex InboxFolderTreeModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
    {
        return QModelIndex();
    }

    InboxFolderTreeItem *childItem = static_cast<InboxFolderTreeItem*>(index.internalPointer());
    InboxFolderTreeItem *parentItem = childItem->parentItem();

    if (parentItem == m_rootItem)
    {
        return QModelIndex();
    }

    return createIndex(parentItem->row(), 0, parentItem);
}

int InboxFolderTreeModel::rowCount(const QModelIndex &parent) const
{
    InboxFolderTreeItem *parentItem;

    if (parent.column() > 0)
    {
        return 0;
    }

    if (!parent.isValid())
    {
        parentItem = m_rootItem;
    }
    else
    {
        parentItem = static_cast<InboxFolderTreeItem*>(parent.internalPointer());
    }

    return parentItem->childCount();
}

int InboxFolderTreeModel::columnCount(const QModelIndex &parent) const
{
    return 1;
}

QVariant InboxFolderTreeModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
    {
        return QVariant();
    }

    InboxFolderTreeItem *item = static_cast<InboxFolderTreeItem*>(index.internalPointer());

    if (role == Qt::DisplayRole)
    {
        int unreadMessagesCount = item->data(2).toInt();

        if (unreadMessagesCount > 0)
        {
            QString data;
            data.append(item->data(0).toString());
            data.append(" (");
            data.append(QString::number(unreadMessagesCount));
            data.append(")");

            return data;
        }
        else
        {
            return item->data(0);
        }
    }
    else if (role == Qt::DecorationRole)
    {
        return item->data(1);
    }
    else if (role == Qt::FontRole && item->data(2).toInt() > 0)
    {
        QFont font;
        font.setBold(true);
        return font;
    }
    else if (role == Qt::FontRole && item->parentItem() != nullptr)
    {
        if (item->parentItem()->parentItem() == nullptr)
        {
            QFont font;
            font.setBold(true);
            return font;
        }
        else
        {
            return QFont();
        }
    }
    else
    {
        return QVariant();
    }
}

Qt::ItemFlags InboxFolderTreeModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
    {
        return nullptr;
    }

    return QAbstractItemModel::flags(index);
}

}
