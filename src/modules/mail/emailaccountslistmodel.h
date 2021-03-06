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

#ifndef EMAILACCOUNTSLISTMODEL_H
#define EMAILACCOUNTSLISTMODEL_H

#include <QObject>
#include <QAbstractListModel>
#include "emailaccount.h"

namespace Otter
{

class EmailAccountsListModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit EmailAccountsListModel(QList<EmailAccount> &data, QObject *parent = nullptr)
        : QAbstractListModel(parent), m_data(&data) {}

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;

    bool removeRows(int position, int rows, const QModelIndex &parent = QModelIndex()) override;
    bool insertRows(int position, int rows, const QModelIndex &parent = QModelIndex()) override;

    void addAccount(const EmailAccount account);
    void removeAccount(const int row);
    void replaceAccount(const EmailAccount account, const QModelIndex index);

private:

    EmailAccount m_userAccountToAdd;
    QList<EmailAccount> *m_data;
};

}

#endif // EMAILACCOUNTSLISTMODEL_H
