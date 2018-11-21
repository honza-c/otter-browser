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

#ifndef INBOXFOLDERTREEMODEL_H
#define INBOXFOLDERTREEMODEL_H

#include <QObject>
#include <QtWidgets>
#include "useraccount.h"
#include "vmimeinboxservice.h"
#include "vmimeinboxfolder.h"
#include "inboxfoldertreeitem.h"

namespace Otter
{

class InboxFolderTreeModel : public QAbstractItemModel
{
public:
    InboxFolderTreeModel(const QList<InboxFolder> &data, QObject *parent = nullptr);
    ~InboxFolderTreeModel() override;

    QVariant data(const QModelIndex &index, int role) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &parent = QModelIndex()) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

private:
    void createFolderTreeStructure(QList<InboxFolder> data);
    int getLevel(const QString path) const;
    QIcon getIcon(const InboxFolder folder) const;
    QString getNameFromPath(const QString path) const;

    InboxFolderTreeItem *m_rootItem;
};

}

#endif // INBOXFOLDERTREEMODEL_H
