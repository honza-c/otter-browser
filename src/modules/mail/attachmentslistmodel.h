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

#ifndef ATTACHMENTSLISTMODEL_H
#define ATTACHMENTSLISTMODEL_H

#include <QObject>
#include <QtWidgets>
#include "attachment.h"

namespace Otter
{

class AttachmentsListModel : public QAbstractListModel
{
    Q_OBJECT
public:
    AttachmentsListModel(QList<Attachment> &data, QObject *parent = nullptr)
        : QAbstractListModel(parent), m_data(&data) {}

    QVariant data(const QModelIndex &index, int role) const override;
    int rowCount(const QModelIndex &index = QModelIndex()) const override;
    bool removeRows(int position, int rows, const QModelIndex &index = QModelIndex()) override;

private:
    QList<Attachment> *m_data;
};

}
#endif // ATTACHMENTSLISTMODEL_H
