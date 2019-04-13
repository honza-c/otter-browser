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

#include "EmailAccountsManager.h"
#include "SessionsManager.h"

namespace Otter
{

EmailAccountsManager* EmailAccountsManager::m_instance = nullptr;
DatabaseManager* EmailAccountsManager::m_databaseManager = nullptr;
QList<UserAccount> EmailAccountsManager::m_emailAccounts = QList<UserAccount>();

EmailAccountsManager::EmailAccountsManager(QObject *parent) : QObject (parent)
{
}

void EmailAccountsManager::createInstance()
{
    if (!m_instance)
    {
        m_instance = new EmailAccountsManager(QCoreApplication::instance());
        m_databaseManager = new DatabaseManager(SessionsManager::getWritableDataPath(QLatin1String("emailDatabase.sqlite")));
        loadEmailAccounts(SessionsManager::getWritableDataPath(QLatin1String("emailAccounts.json")));
    }
}

EmailAccountsManager* EmailAccountsManager::getInstance()
{
    return m_instance;
}

QList<UserAccount>& EmailAccountsManager::getEmailAccounts()
{
    return m_emailAccounts;
}

bool EmailAccountsManager::loadEmailAccounts(const QString &path)
{
    QFile file(path);

    if (!file.open(QFile::ReadOnly))
    {
        if (QFile::exists(path))
        {
            return false;
        }
    }

    QString jsonString = file.readAll();
    file.close();

    QJsonDocument jsonDocument = QJsonDocument::fromJson(jsonString.toUtf8());
    QJsonArray jsonArray = jsonDocument.array();

    for (int i = 0; i < jsonArray.size(); i++)
    {
        m_emailAccounts << readFromJson(jsonArray[i].toObject());
    }

    QList<QString> emailAddresses;

    for (UserAccount &account : m_emailAccounts)
    {
        account.initializeInbox();
        emailAddresses << account.emailAddress();
    }

    DatabaseManager::cleanUnusedDataFromDatabase(emailAddresses);

    return true;
}

bool EmailAccountsManager::saveEmailAccounts(const QString &path)
{
    if (SessionsManager::isReadOnly())
    {
        return false;
    }

    QJsonDocument jsonDocument;
    QJsonArray jsonArray;

    for (UserAccount account : m_emailAccounts)
    {
        jsonArray.append(writeToJson(account));
    }

    jsonDocument.setArray(jsonArray);


    QSaveFile file(path);

    if (!file.open(QIODevice::WriteOnly))
    {
        return false;
    }

    file.write(jsonDocument.toJson().data(), jsonDocument.toJson().length());

    return file.commit();
}

QJsonObject EmailAccountsManager::writeToJson(const UserAccount account)
{
    QJsonObject json;

    json[QLatin1String("accountName")] = account.accountName();
    json[QLatin1String("contactName")] = account.contactName();
    json[QLatin1String("emailAddress")] = account.emailAddress();
    json[QLatin1String("userName")] = account.userName();
    json[QLatin1String("password")] = account.password();
    json[QLatin1String("smtpServerUrl")] = account.smtpServerUrl();
    json[QLatin1String("smtpServerPort")] = QString::number(account.smtpServerPort());

    if (account.incomingServerType() == UserAccount::IMAP)
    {
        json[QLatin1String("incomingServerType")] = QLatin1String("IMAP");
    }
    else
    {
        json[QLatin1String("incomingServerType")] = QLatin1String("POP3");
    }

    json[QLatin1String("incomingServerAddress")] = account.incomingServerAddress();
    json[QLatin1String("incomingServerPort")] = QString::number(account.incomingServerPort());

    return json;
}

UserAccount EmailAccountsManager::readFromJson(const QJsonObject json)
{
    UserAccount account;

    account.setAccountName(json[QLatin1String("accountName")].toString());
    account.setContactName(json[QLatin1String("contactName")].toString());
    account.setEmailAddress(json[QLatin1String("emailAddress")].toString());
    account.setUserName(json[QLatin1String("userName")].toString());
    account.setPassword(json[QLatin1String("password")].toString());
    account.setSmtpServerUrl(json[QLatin1String("smtpServerUrl")].toString());
    account.setSmtpServerPort(json[QLatin1String("smtpServerPort")].toString().toInt());
    account.setIncomingServerAddress(json[QLatin1String("incomingServerAddress")].toString());
    account.setIncomingServerPort(json[QLatin1String("incomingServerPort")].toString().toInt());

    if (json[QLatin1String("incomingServerType")] == QLatin1String("IMAP"))
    {
        account.setIncomingServerType(UserAccount::IMAP);
    }
    else
    {
        account.setIncomingServerType(UserAccount::POP3);
    }

    return account;
}

void EmailAccountsManager::updateEmailAccountsConfiguration(const QList<UserAccount> accounts)
{
    if (m_instance)
    {
        m_emailAccounts = accounts;
        saveEmailAccounts(SessionsManager::getWritableDataPath(QLatin1String("emailAccounts.json")));

        QList<QString> emailAddresses;

        for (UserAccount &account : m_emailAccounts)
        {
            account.initializeInbox();
            emailAddresses << account.emailAddress();
        }

        DatabaseManager::cleanUnusedDataFromDatabase(emailAddresses);
    }
}

}
