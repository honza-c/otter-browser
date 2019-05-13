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
QList<EmailAccount> EmailAccountsManager::m_emailAccounts = QList<EmailAccount>();

EmailAccountsManager::EmailAccountsManager(QObject *parent) : QObject (parent)
{
}

void EmailAccountsManager::createInstance()
{
    if (!m_instance)
    {
        m_instance = new EmailAccountsManager(QCoreApplication::instance());
        m_databaseManager = DatabaseManager::getInstance();
        loadEmailAccounts(SessionsManager::getWritableDataPath(QLatin1String("emailAccounts.json")));

        QTimer *updateInboxesTimer = new QTimer(m_instance);
        connect(updateInboxesTimer, SIGNAL(timeout()), m_instance, SLOT(updateInboxes()));
        updateInboxesTimer->start(15 * 60 * 1000);
    }
}

EmailAccountsManager* EmailAccountsManager::getInstance()
{
    return m_instance;
}

QList<EmailAccount>& EmailAccountsManager::getEmailAccounts()
{
    return m_emailAccounts;
}

void EmailAccountsManager::updateInboxes()
{
    QList<QString> emailAddresses;

    for (EmailAccount &account : m_emailAccounts)
    {
        account.fetchStoreContent();
        emailAddresses << account.emailAddress();
    }

    DatabaseManager::cleanUnusedDataFromDatabase(emailAddresses);
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

    for (EmailAccount &account : m_emailAccounts)
    {
        account.fetchStoreContent();
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

    for (EmailAccount account : m_emailAccounts)
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

QJsonObject EmailAccountsManager::writeToJson(const EmailAccount account)
{
    QJsonObject json;

    json[QLatin1String("contactName")] = account.contactName();
    json[QLatin1String("emailAddress")] = account.emailAddress();

    json[QLatin1String("imapUserName")] = account.imapServerUserName();
    json[QLatin1String("imapPassword")] = account.imapServerPassword();
    json[QLatin1String("imapHost")] = account.imapServerAddress();
    json[QLatin1String("imapPort")] = QString::number(account.imapServerPort());
    json[QLatin1String("isImapConnectionSecured")] = account.isImapServerConnectionEncrypted();

    json[QLatin1String("smtpUserName")] = account.smtpServerUserName();
    json[QLatin1String("smtpPassword")] = account.smtpServerPassword();
    json[QLatin1String("smtpHost")] = account.smtpServerAddress();
    json[QLatin1String("smtpPort")] = QString::number(account.smtpServerPort());
    json[QLatin1String("isSmtpConnectionSecured")] = account.isSmtpServerConnectionEncrypted();

    return json;
}

EmailAccount EmailAccountsManager::readFromJson(const QJsonObject json)
{
    EmailAccount account;

    account.setContactName(json[QLatin1String("contactName")].toString());
    account.setEmailAddress(json[QLatin1String("emailAddress")].toString());

    account.setImapServerUserName(json[QLatin1String("imapUserName")].toString());
    account.setImapServerPassword(json[QLatin1String("imapPassword")].toString());
    account.setImapServerAddress(json[QLatin1String("imapHost")].toString());
    account.setImapServerPort(json[QLatin1String("imapPort")].toString().toInt());
    account.setIsImapServerConnectionEncrypted(json[QLatin1String("isImapConnectionSecured")].toBool());

    account.setSmtpServerUserName(json[QLatin1String("smtpUserName")].toString());
    account.setSmtpServerPassword(json[QLatin1String("smtpPassword")].toString());
    account.setSmtpServerAddress(json[QLatin1String("smtpHost")].toString());
    account.setSmtpServerPort(json[QLatin1String("smtpPort")].toString().toInt());
    account.setIsSmtpServerConnectionEncrypted(json[QLatin1String("isSmtpConnectionSecured")].toBool());

    return account;
}

void EmailAccountsManager::updateEmailAccountsConfiguration(const QList<EmailAccount> accounts)
{
    if (m_instance)
    {
        m_emailAccounts = accounts;
        saveEmailAccounts(SessionsManager::getWritableDataPath(QLatin1String("emailAccounts.json")));

        QList<QString> emailAddresses;

        for (EmailAccount &account : m_emailAccounts)
        {
            account.fetchStoreContent();
            emailAddresses << account.emailAddress();
        }

        DatabaseManager::cleanUnusedDataFromDatabase(emailAddresses);
    }
}

}
