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

#include "emailaccount.h"

namespace Otter
{

EmailAccount::EmailAccount(QObject *parent)
    : QObject (parent)
{
    m_smtpServerPort = 0;
    m_incomingServerPort = 0;
}

EmailAccount::EmailAccount(const EmailAccount &other)
{
    m_accountName = other.m_accountName;
    m_contactName = other.m_contactName;
    m_emailAddress = other.m_emailAddress;
    m_userName = other.m_userName;
    m_password = other.m_password;

    m_smtpServerUrl = other.m_smtpServerUrl;
    m_smtpServerPort = other.m_smtpServerPort;

    m_incomingServerType = other.m_incomingServerType;
    m_incomingServerAddress = other.m_incomingServerAddress;
    m_incomingServerPort = other.m_incomingServerPort;

    m_inboxService = other.m_inboxService;
}

EmailAccount& EmailAccount::operator=(const EmailAccount &other)
{
    m_accountName = other.m_accountName;
    m_contactName = other.m_contactName;
    m_emailAddress = other.m_emailAddress;
    m_userName = other.m_userName;
    m_password = other.m_password;

    m_smtpServerUrl = other.m_smtpServerUrl;
    m_smtpServerPort = other.m_smtpServerPort;

    m_incomingServerType = other.m_incomingServerType;
    m_incomingServerAddress = other.m_incomingServerAddress;
    m_incomingServerPort = other.m_incomingServerPort;

    m_inboxService = other.m_inboxService;

    return *this;
}

QString EmailAccount::accountName() const
{
    return m_accountName;
}

void EmailAccount::setAccountName(QString accountName)
{
    m_accountName = accountName;
}

QString EmailAccount:: contactName() const
{
    return m_contactName;
}

void EmailAccount::setContactName(QString userName)
{
    m_contactName = userName;
}

QString EmailAccount::emailAddress() const
{
    return m_emailAddress;
}

void EmailAccount::setEmailAddress(QString emailAddress)
{
    m_emailAddress = emailAddress;
}

QString EmailAccount::userName() const
{
    return m_userName;
}

void EmailAccount::setUserName(const QString userName)
{
    m_userName = userName;
}

QString EmailAccount::password() const
{
    return m_password;
}

void EmailAccount::setPassword(QString password)
{
    m_password = password;
}

QString EmailAccount::smtpServerUrl() const
{
    return m_smtpServerUrl;
}

void EmailAccount::setSmtpServerUrl(QString smtpServerUrl)
{
    m_smtpServerUrl = smtpServerUrl;
}

int EmailAccount::smtpServerPort() const
{
    return m_smtpServerPort;
}

void EmailAccount::setSmtpServerPort(int smtpServerPort)
{
    m_smtpServerPort = smtpServerPort;
}

EmailAccount::IncomingServerType EmailAccount::incomingServerType() const
{
    return m_incomingServerType;
}

void EmailAccount::setIncomingServerType(IncomingServerType type)
{
    m_incomingServerType = type;
}

QString EmailAccount::incomingServerAddress() const
{
    return m_incomingServerAddress;
}

void EmailAccount:: setIncomingServerAddress(QString incomingServerAddress)
{
    m_incomingServerAddress = incomingServerAddress;
}

int EmailAccount::incomingServerPort() const
{
    return m_incomingServerPort;
}

void EmailAccount::setIncomingServerPort(int incomingServerPort)
{
    m_incomingServerPort = incomingServerPort;
}

void EmailAccount::updateFolderStructureInDatabase(QList<InboxFolder> folders)
{
    QStringList folderPathsFromDatabase = DatabaseManager::getFoldersPathForAccount(m_emailAddress);

    for (QString path : folderPathsFromDatabase)
    {
        bool found = false;

        for (InboxFolder folder : folders)
        {
            if (folder.path() == path)
            {
                found = true;
                break;
            }
        }

        if (!found)
        {
            DatabaseManager::deleteFolderFromDatabase(m_emailAddress, path);
        }
    }

    for (InboxFolder folder : folders)
    {
        bool found = false;

        for (QString path : folderPathsFromDatabase)
        {
            if (folder.path() == path)
            {
                found = true;
                break;
            }
        }

        if (!found)
        {
            DatabaseManager::addFolderToDatabase(folder);
        }
    }
}

void EmailAccount::initializeInbox()
{
    fetchStoreContent();
}

QFuture<QList<InboxFolder>> EmailAccount::fetchInboxFolders()
{
    auto fetchInboxFoldersWorker = [](
            const connectionSettingsHolder settings)
    {
        VmimeInboxService *inboxService = nullptr;

        if (settings.incomingServerType == EmailAccount::IMAP)
        {
            inboxService = new VmimeImapService();
        }
        else if (settings.incomingServerType == EmailAccount::POP3)
        {
            inboxService = new VmimePop3Service();
        }

        if (inboxService != nullptr)
        {
            inboxService->setEmailAddress(settings.emailAddress);
            inboxService->setUserName(settings.userName);
            inboxService->setPassword(settings.password);
            inboxService->setServerUrl(settings.incomingServerAddress);
            inboxService->setPort(settings.incomingServerPort);

            return inboxService->fetchInboxFolders();
        }
        else
        {
            return QList<InboxFolder>();
        }
    };

    return QtConcurrent::run(fetchInboxFoldersWorker, getConnectionSettings());
}

QFuture<bool> EmailAccount::moveMessageThread(const QString sourceFolderPath, const int messageId, const QString destinationFolderPath)
{
    auto moveMessageWorker = [](
            const connectionSettingsHolder settings,
            const QString sourceFolderPath,
            const int messageId,
            const QString destinationFolderPath)
    {
        VmimeInboxService *inboxService = nullptr;

        if (settings.incomingServerType == EmailAccount::IMAP)
        {
            inboxService = new VmimeImapService();
        }
        else if (settings.incomingServerType == EmailAccount::POP3)
        {
            inboxService = new VmimePop3Service();
        }

        if (inboxService != nullptr)
        {
            inboxService->setEmailAddress(settings.emailAddress);
            inboxService->setUserName(settings.userName);
            inboxService->setPassword(settings.password);
            inboxService->setServerUrl(settings.incomingServerAddress);
            inboxService->setPort(settings.incomingServerPort);

            inboxService->moveMessage(sourceFolderPath, messageId, destinationFolderPath);

            return true;
        }
        return false;
    };

    return QtConcurrent::run(moveMessageWorker, getConnectionSettings(), sourceFolderPath, messageId, destinationFolderPath);
}

void EmailAccount::moveMessage(const QString sourceFolderPath, const int messageId, const QString destinationFolderPath)
{
    QFuture<bool> future = moveMessageThread(sourceFolderPath, messageId, destinationFolderPath);
    QFutureWatcher<bool> *watcher = new QFutureWatcher<bool>();

    connect(watcher, &QFutureWatcher<bool>::finished, [=](){
        qWarning() << "Presun zprav dobehl se stavem: " << future.result();
    });

    watcher->setFuture(future);
}

void EmailAccount::fetchStoreContent()
{
    QFuture<QList<InboxFolder>> future = fetchInboxFolders();

    QFutureWatcher<QList<InboxFolder>> *watcher = new QFutureWatcher<QList<InboxFolder>>();

    connect(watcher, &QFutureWatcher<QList<InboxFolder>>::finished, [=](){
        updateFolderStructureInDatabase(future.result());
        fetchMessageMetadata();
    });

    watcher->setFuture(future);
}

void EmailAccount::fetchMessageMetadata()
{
    QFuture<QList<MessageMetadata>> future = fetchMessagesMetadata();
    QFutureWatcher<QList<MessageMetadata>> *watcher = new QFutureWatcher<QList<MessageMetadata>>();

    connect(watcher, &QFutureWatcher<QList<MessageMetadata>>::finished, [=]()
    {
        DatabaseManager::updateMessageMetadata(future.result(), m_emailAddress);
    });

    watcher->setFuture(future);
}

QFuture<QList<MessageMetadata>> EmailAccount::fetchMessagesMetadata()
{
    auto fetchMessageMetadataWorker = [](const connectionSettingsHolder settings)
    {
        VmimeImapService imapService;

        imapService.setEmailAddress(settings.emailAddress);
        imapService.setUserName(settings.userName);
        imapService.setPassword(settings.password);
        imapService.setServerUrl(settings.incomingServerAddress);
        imapService.setPort(settings.incomingServerPort);

        return imapService.fetchMessagesMetadata();
    };

    return QtConcurrent::run(fetchMessageMetadataWorker, getConnectionSettings());
}

void EmailAccount::sendMessage(Message message) const
{
    auto sendMessageWorker = [](
            const connectionSettingsHolder settings,
            const Message message)
    {
        VmimeSmtpService smtpService;

        smtpService.setEmailAddress(settings.emailAddress);
        smtpService.setUserName(settings.userName);
        smtpService.setPassword(settings.password);
        smtpService.setServerUrl(settings.smtpServerUrl);
        smtpService.setPort(settings.smtpServerPort);

        smtpService.sendMessage(message);
    };

    QtConcurrent::run(sendMessageWorker, getConnectionSettings(),message);
}

void EmailAccount::fetchMissingMessageContent(const QString folderPath, const int positionInFolder)
{
    QFuture<MessageContent> future = fetchMessageContent(folderPath, positionInFolder);
    QFutureWatcher<MessageContent> *watcher = new QFutureWatcher<MessageContent>();

    QString emailAddress = m_emailAddress;

    connect(watcher, &QFutureWatcher<MessageContent>::finished, [=](){
        updateMessageContentInDatabase(emailAddress, folderPath, positionInFolder, future.result());
    });

    watcher->setFuture(future);
}

QFuture<MessageContent> EmailAccount::fetchMessageContent(QString folderPath, int positionInFolder)
{
    auto fetchMessageContentWorker = [](
            const connectionSettingsHolder settings,
            const QString folderPath,
            const int positionInFolder)
    {
        VmimeInboxService *inboxService = nullptr;

        if (settings.incomingServerType == EmailAccount::IMAP)
        {
            inboxService = new VmimeImapService();
        }
        else if (settings.incomingServerType == EmailAccount::POP3)
        {
            inboxService = new VmimePop3Service();
        }

        if (inboxService != nullptr)
        {
            inboxService->setEmailAddress(settings.emailAddress);
            inboxService->setUserName(settings.userName);
            inboxService->setPassword(settings.password);
            inboxService->setServerUrl(settings.incomingServerAddress);
            inboxService->setPort(settings.incomingServerPort);

            return inboxService->fetchMessageContent(folderPath, positionInFolder);
        }
        else
        {
            return MessageContent();
        }
    };

    return QtConcurrent::run(fetchMessageContentWorker, getConnectionSettings(), folderPath, positionInFolder);
}

EmailAccount::connectionSettingsHolder EmailAccount::getConnectionSettings() const
{
    connectionSettingsHolder settings;

    settings.contactName = m_contactName;
    settings.userName = m_userName;
    settings.emailAddress = m_emailAddress;
    settings.password = m_password;
    settings.smtpServerUrl = m_smtpServerUrl;
    settings.smtpServerPort = m_smtpServerPort;
    settings.incomingServerType = m_incomingServerType;
    settings.incomingServerAddress = m_incomingServerAddress;
    settings.incomingServerPort = m_incomingServerPort;

    return settings;
}

QFuture<QList<MessageMetadata>> EmailAccount::fetchMessagesMetadata(QMap<QString, int> folderPathsWithMessagesCountsInDb)
{
    auto fetchMessageMetadataWorker = [](
            const connectionSettingsHolder settings,
            const QMap<QString, int> folderPathsWithMessagesCountInDb)
    {
        VmimeInboxService *inboxService = nullptr;

        if (settings.incomingServerType == EmailAccount::IMAP)
        {
            inboxService = new VmimeImapService();
        }
        else if (settings.incomingServerType == EmailAccount::POP3)
        {
            inboxService = new VmimePop3Service();
        }

        if (inboxService != nullptr)
        {
            inboxService->setEmailAddress(settings.emailAddress);
            inboxService->setUserName(settings.userName);
            inboxService->setPassword(settings.password);
            inboxService->setServerUrl(settings.incomingServerAddress);
            inboxService->setPort(settings.incomingServerPort);

            return inboxService->fetchMessagesMetadata(folderPathsWithMessagesCountInDb);
        }
        else
        {
            return QList<MessageMetadata>();
        }
    };

    return QtConcurrent::run(fetchMessageMetadataWorker, getConnectionSettings(), folderPathsWithMessagesCountsInDb);
}

void EmailAccount::updateMessageContentInDatabase(const QString emailAddress, const QString folderPath, const int positionInFolder, MessageContent messageContent)
{
    if (messageContent != MessageContent())
    {
        int folderId = DatabaseManager::getFolderId(emailAddress, folderPath);
        int messageId = DatabaseManager::getMessageId(folderId, positionInFolder);

        DatabaseManager::updateHtmlContent(messageId, messageContent.htmlContent());
        DatabaseManager::updatePlainTextContent(messageId, messageContent.plainTextContent());
        DatabaseManager::updateAttachmentsContent(messageId, messageContent.attachments());
        DatabaseManager::updateEmbeddedObjectsContent(messageId, messageContent.embeddedObjects());
        DatabaseManager::updateRecipients(messageId, messageContent.recipients());
        DatabaseManager::updateCopyRecipients(messageId, messageContent.copyRecipients());

        emit messageContentFetched(messageId);
    }
}

QDebug operator<<(QDebug debug, const EmailAccount &account)
{
    QString incomingServerType;

    if (account.incomingServerType() == EmailAccount::IMAP)
    {
        incomingServerType = "IMAP";
    }
    else
    {
        incomingServerType = "POP3";
    }

    QDebugStateSaver saver(debug);

    debug.noquote() << "Account name: "
                    << account.accountName()
                    << "\nContact name: "
                    << account.contactName()
                    << "\nEmail address: "
                    << account.emailAddress()
                    << "\nUser name: "
                    << account.userName()
                    << "\nPassword: "
                    << account.password()
                    << "\nSMTP server URL: "
                    << account.smtpServerUrl()
                    << "\nSMTP server port: "
                    << account.smtpServerPort()
                    << "\nIncoming server type: "
                    << incomingServerType
                    << "\nIncoming server address: "
                    << account.incomingServerAddress()
                    << "\nIncoming server port: "
                    << account.incomingServerPort();

    return debug;
}

}
