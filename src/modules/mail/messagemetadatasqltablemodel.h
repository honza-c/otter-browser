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

#ifndef MESSAGEMETADATASQLTABLEMODEL_H
#define MESSAGEMETADATASQLTABLEMODEL_H

#include <QObject>
#include <QSqlTableModel>
#include <QDateTime>
#include <QFont>
#include <QDataStream>
#include "contact.h"

namespace Otter
{

class MessageMetadataSqlTableModel : public QSqlTableModel
{
    Q_OBJECT
public:
    QVariant data(const QModelIndex &index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
public slots:
    void onTableHeaderClicked(int);
    void currentRowChanged(const QModelIndex &, const QModelIndex &);
};

}

#endif // MESSAGEMETADATASQLTABLEMODEL_H
