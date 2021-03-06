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

#ifndef EMAILACCOUNTSMANAGER_H
#define EMAILACCOUNTSMANAGER_H

#include <QtCore/QObject>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include "../modules/mail/emailaccount.h"
#include "../modules/mail/databasemanager.h"

namespace Otter
{

class EmailAccountsManager final : public QObject
{
    Q_OBJECT

public:
    static void createInstance();
    static EmailAccountsManager* getInstance();
    static QList<EmailAccount>& getEmailAccounts();
    static void updateEmailAccountsConfiguration(const QList<EmailAccount> accounts);

protected:
    explicit EmailAccountsManager(QObject *parent);

    static bool loadEmailAccounts(const QString &path);
    static bool saveEmailAccounts(const QString &path);

    static QJsonObject writeToJson(const EmailAccount account);
    static EmailAccount readFromJson(const QJsonObject json);

private slots:
    void updateInboxes();

private:
    static EmailAccountsManager *m_instance;
    static DatabaseManager *m_databaseManager;
    static QList<EmailAccount> m_emailAccounts;
};

}

#endif // EMAILACCOUNTSMANAGER_H
