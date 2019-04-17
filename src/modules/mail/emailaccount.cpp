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
    m_imapServerPort = 0;
}

EmailAccount::EmailAccount(const EmailAccount &other)
{
    m_contactName = other.m_contactName;
    m_emailAddress = other.m_emailAddress;
    m_userName = other.m_userName;
    m_password = other.m_password;
    m_smtpServerUrl = other.m_smtpServerUrl;
    m_smtpServerPort = other.m_smtpServerPort;
    m_imapServerAddress = other.m_imapServerAddress;
    m_imapServerPort = other.m_imapServerPort;

    m_imapService = other.m_imapService;
}

EmailAccount& EmailAccount::operator=(const EmailAccount &other)
{
    m_contactName = other.m_contactName;
    m_emailAddress = other.m_emailAddress;
    m_userName = other.m_userName;
    m_password = other.m_password;
    m_smtpServerUrl = other.m_smtpServerUrl;
    m_smtpServerPort = other.m_smtpServerPort;
    m_imapServerAddress = other.m_imapServerAddress;
    m_imapServerPort = other.m_imapServerPort;
    m_imapService = other.m_imapService;

    return *this;
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

QString EmailAccount::imapServerAddress() const
{
    return m_imapServerAddress;
}

void EmailAccount:: setImapServerAddress(QString incomingServerAddress)
{
    m_imapServerAddress = incomingServerAddress;
}

int EmailAccount::imapServerPort() const
{
    return m_imapServerPort;
}

void EmailAccount::setImapServerPort(int incomingServerPort)
{
    m_imapServerPort = incomingServerPort;
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
        VmimeInboxService *inboxService = new VmimeImapService();

        inboxService->setEmailAddress(settings.emailAddress);
        inboxService->setUserName(settings.userName);
        inboxService->setPassword(settings.password);
        inboxService->setServerUrl(settings.imapServerAddress);
        inboxService->setPort(settings.imapServerPort);

        return inboxService->fetchInboxFolders();
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
        VmimeInboxService *inboxService = new VmimeImapService();

        inboxService->setEmailAddress(settings.emailAddress);
        inboxService->setUserName(settings.userName);
        inboxService->setPassword(settings.password);
        inboxService->setServerUrl(settings.imapServerAddress);
        inboxService->setPort(settings.imapServerPort);

        inboxService->moveMessage(sourceFolderPath, messageId, destinationFolderPath);

        return true;
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
        imapService.setServerUrl(settings.imapServerAddress);
        imapService.setPort(settings.imapServerPort);

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

QFuture<MessageContent> EmailAccount::fetchMessageContent(QString folderPath, int uid)
{
    auto fetchMessageContentWorker = [](
            const connectionSettingsHolder settings,
            const QString folderPath,
            const int positionInFolder)
    {
        VmimeImapService imapService;

        imapService.setEmailAddress(settings.emailAddress);
        imapService.setUserName(settings.userName);
        imapService.setPassword(settings.password);
        imapService.setServerUrl(settings.imapServerAddress);
        imapService.setPort(settings.imapServerPort);

        return imapService.fetchMessageContent(folderPath, positionInFolder);
    };

    return QtConcurrent::run(fetchMessageContentWorker, getConnectionSettings(), folderPath, uid);
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
    settings.imapServerAddress = m_imapServerAddress;
    settings.imapServerPort = m_imapServerPort;

    return settings;
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
    QDebugStateSaver saver(debug);

    debug.noquote() << "\nContact name: "
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
                    << "\nIncoming server address: "
                    << account.imapServerAddress()
                    << "\nIncoming server port: "
                    << account.imapServerPort();

    return debug;
}

}
