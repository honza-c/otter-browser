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
    account.setSmtpServerPort(json[QLatin1String("smtpServerPort")].toInt());
    account.setIncomingServerAddress(json[QLatin1String("incomingServerAddress")].toString());
    account.setIncomingServerPort(json[QLatin1String("incomingServerPort")].toInt());

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

}
