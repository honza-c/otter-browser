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

    m_isImapServerConnectionEncrypted = false;
    m_isSmtpServerConnectionEncrypted = false;
}

EmailAccount::EmailAccount(const EmailAccount &other)
{
    m_contactName = other.m_contactName;
    m_emailAddress = other.m_emailAddress;

    m_imapServerUserName = other.m_imapServerUserName;
    m_imapServerPassword = other.m_imapServerPassword;
    m_imapServerAddress = other.m_imapServerAddress;
    m_imapServerPort = other.m_imapServerPort;
    m_isImapServerConnectionEncrypted = other.m_isImapServerConnectionEncrypted;

    m_smtpServerUserName = other.m_smtpServerUserName;
    m_smtpServerPassword = other.m_smtpServerPassword;
    m_smtpServerAddress = other.m_smtpServerAddress;
    m_smtpServerPort = other.m_smtpServerPort;
    m_isSmtpServerConnectionEncrypted = other.m_isSmtpServerConnectionEncrypted;
}

EmailAccount& EmailAccount::operator=(const EmailAccount &other)
{
    m_contactName = other.m_contactName;
    m_emailAddress = other.m_emailAddress;

    m_imapServerUserName = other.m_imapServerUserName;
    m_imapServerPassword = other.m_imapServerPassword;
    m_imapServerAddress = other.m_imapServerAddress;
    m_imapServerPort = other.m_imapServerPort;
    m_isImapServerConnectionEncrypted = other.m_isImapServerConnectionEncrypted;

    m_smtpServerUserName = other.m_smtpServerUserName;
    m_smtpServerPassword = other.m_smtpServerPassword;
    m_smtpServerAddress = other.m_smtpServerAddress;
    m_smtpServerPort = other.m_smtpServerPort;
    m_isSmtpServerConnectionEncrypted = other.m_isSmtpServerConnectionEncrypted;

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

QString EmailAccount::imapServerUserName() const
{
    return m_imapServerUserName;
}

void EmailAccount::setImapServerUserName(const QString imapServerUserName)
{
    m_imapServerUserName = imapServerUserName;
}

QString EmailAccount::imapServerPassword() const
{
    return m_imapServerPassword;
}

void EmailAccount::setImapServerPassword(QString imapServerPassword)
{
    m_imapServerPassword = imapServerPassword;
}

QString EmailAccount::imapServerAddress() const
{
    return m_imapServerAddress;
}

void EmailAccount:: setImapServerAddress(QString imapServerAddress)
{
    m_imapServerAddress = imapServerAddress;
}

int EmailAccount::imapServerPort() const
{
    return m_imapServerPort;
}

void EmailAccount::setImapServerPort(int imapServerPort)
{
    m_imapServerPort = imapServerPort;
}

bool EmailAccount::isImapServerConnectionEncrypted() const
{
    return m_isImapServerConnectionEncrypted;
}

void EmailAccount::setIsImapServerConnectionEncrypted(const bool isImapServerConnectionSecured)
{
    m_isImapServerConnectionEncrypted = isImapServerConnectionSecured;
}

QString EmailAccount::smtpServerUserName() const
{
    return m_smtpServerUserName;
}

void EmailAccount::setSmtpServerUserName(const QString smtpServerUserName)
{
    m_smtpServerUserName = smtpServerUserName;
}

QString EmailAccount::smtpServerPassword() const
{
    return m_smtpServerPassword;
}

void EmailAccount::setSmtpServerPassword(const QString smtpServerPassword)
{
    m_smtpServerPassword = smtpServerPassword;
}

QString EmailAccount::smtpServerAddress() const
{
    return m_smtpServerAddress;
}

void EmailAccount::setSmtpServerAddress(QString smtpServerUrl)
{
    m_smtpServerAddress = smtpServerUrl;
}

int EmailAccount::smtpServerPort() const
{
    return m_smtpServerPort;
}

void EmailAccount::setSmtpServerPort(int smtpServerPort)
{
    m_smtpServerPort = smtpServerPort;
}

bool EmailAccount::isSmtpServerConnectionEncrypted() const
{
    return m_isSmtpServerConnectionEncrypted;
}

void EmailAccount::setIsSmtpServerConnectionEncrypted(const bool isSmtpServerConnectionSecured)
{
    m_isSmtpServerConnectionEncrypted = isSmtpServerConnectionSecured;
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

    QList<InboxFolder> newFolders;

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
            newFolders << folder;
        }
    }

    if (!newFolders.empty())
    {
        DatabaseManager::addFoldersToDatabase(newFolders);
    }
}

QFuture<QList<InboxFolder>> EmailAccount::fetchInboxFoldersThread()
{
    auto fetchInboxFoldersWorker = [](
            const connectionSettingsHolder settings)
    {
        VmimeImapService imapService;

        imapService.setEmailAddress(settings.emailAddress);
        imapService.setUserName(settings.imapServerUserName);
        imapService.setPassword(settings.imapServerPassword);
        imapService.setServerUrl(settings.imapServerAddress);
        imapService.setPort(settings.imapServerPort);
        imapService.setIsConnectionEncrypted(settings.isImapServerConnectionSecured);

        return imapService.fetchInboxFolders();
    };

    return QtConcurrent::run(fetchInboxFoldersWorker, getConnectionSettings());
}

void EmailAccount::fetchStoreContent()
{
    QFuture<QList<InboxFolder>> future = fetchInboxFoldersThread();

    QFutureWatcher<QList<InboxFolder>> *watcher = new QFutureWatcher<QList<InboxFolder>>();

    connect(watcher, &QFutureWatcher<QList<InboxFolder>>::finished, [=](){
        if (future.result() != QList<InboxFolder>())
        {
            updateFolderStructureInDatabase(future.result());
            fetchMessagesMetadata();
        }
    });

    watcher->setFuture(future);
}

void EmailAccount::fetchMessagesMetadata()
{
    QFuture<QList<MessageMetadata>> future = fetchMessagesMetadataThread();
    QFutureWatcher<QList<MessageMetadata>> *watcher = new QFutureWatcher<QList<MessageMetadata>>();

    connect(watcher, &QFutureWatcher<QList<MessageMetadata>>::finished, [=]()
    {
        if (future.result() != QList<MessageMetadata>())
        {
            DatabaseManager::updateMessageMetadata(future.result(), m_emailAddress);
        }
    });

    watcher->setFuture(future);
}

QFuture<QList<MessageMetadata>> EmailAccount::fetchMessagesMetadataThread()
{
    auto fetchMessageMetadataWorker = [](const connectionSettingsHolder settings)
    {
        VmimeImapService imapService;

        imapService.setEmailAddress(settings.emailAddress);
        imapService.setUserName(settings.imapServerUserName);
        imapService.setPassword(settings.imapServerPassword);
        imapService.setServerUrl(settings.imapServerAddress);
        imapService.setPort(settings.imapServerPort);
        imapService.setIsConnectionEncrypted(settings.isImapServerConnectionSecured);

        return imapService.fetchMessagesMetadata();
    };

    return QtConcurrent::run(fetchMessageMetadataWorker, getConnectionSettings());
}

bool EmailAccount::sendMessage(Message message) const
{
    VmimeSmtpService smtpService;
    connectionSettingsHolder settings = getConnectionSettings();

    smtpService.setEmailAddress(settings.emailAddress);
    smtpService.setUserName(settings.smtpServerUserName);
    smtpService.setPassword(settings.smtpServerPassword);
    smtpService.setServerUrl(settings.smtpServerAddress);
    smtpService.setPort(settings.smtpServerPort);
    smtpService.setIsConnectionEncrypted(settings.isSmtpServerConnectionSecured);

    return smtpService.sendMessage(message);
}

void EmailAccount::fetchMessageContent(const QString folderPath, const int positionInFolder)
{
    QFuture<MessageContent> future = fetchMessageContentThread(folderPath, positionInFolder);
    QFutureWatcher<MessageContent> *watcher = new QFutureWatcher<MessageContent>();

    QString emailAddress = m_emailAddress;

    connect(watcher, &QFutureWatcher<MessageContent>::finished, [=](){
        if (future.result() != MessageContent())
        {
            updateMessageContentInDatabase(emailAddress, folderPath, positionInFolder, future.result());
        }
    });

    watcher->setFuture(future);
}

QFuture<MessageContent> EmailAccount::fetchMessageContentThread(QString folderPath, int uid)
{
    auto fetchMessageContentWorker = [](
            const connectionSettingsHolder settings,
            const QString folderPath,
            const int positionInFolder)
    {
        VmimeImapService imapService;

        imapService.setEmailAddress(settings.emailAddress);
        imapService.setUserName(settings.imapServerUserName);
        imapService.setPassword(settings.imapServerPassword);
        imapService.setServerUrl(settings.imapServerAddress);
        imapService.setPort(settings.imapServerPort);
        imapService.setIsConnectionEncrypted(settings.isImapServerConnectionSecured);

        return imapService.fetchMessageContent(folderPath, positionInFolder);
    };

    return QtConcurrent::run(fetchMessageContentWorker, getConnectionSettings(), folderPath, uid);
}

EmailAccount::connectionSettingsHolder EmailAccount::getConnectionSettings() const
{
    connectionSettingsHolder settings;

    settings.contactName = m_contactName;
    settings.emailAddress = m_emailAddress;

    settings.imapServerUserName = m_imapServerUserName;
    settings.imapServerPassword = m_imapServerPassword;
    settings.imapServerAddress = m_imapServerAddress;
    settings.imapServerPort = m_imapServerPort;
    settings.isImapServerConnectionSecured = m_isImapServerConnectionEncrypted;

    settings.smtpServerUserName = m_smtpServerUserName;
    settings.smtpServerPassword = m_smtpServerPassword;
    settings.smtpServerAddress = m_smtpServerAddress;
    settings.smtpServerPort = m_smtpServerPort;
    settings.isSmtpServerConnectionSecured = m_isSmtpServerConnectionEncrypted;

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

QFuture<bool> EmailAccount::deleteMessageThread(const int uid, const QString folderPath)
{
    auto deleteMessageWorker = [](
            const connectionSettingsHolder settings,
            const int uid,
            const QString folderPath)
    {
        VmimeImapService imapService;

        imapService.setEmailAddress(settings.emailAddress);
        imapService.setUserName(settings.imapServerUserName);
        imapService.setPassword(settings.imapServerPassword);
        imapService.setServerUrl(settings.imapServerAddress);
        imapService.setPort(settings.imapServerPort);
        imapService.setIsConnectionEncrypted(settings.isImapServerConnectionSecured);

        return imapService.deleteMessage(uid, folderPath);
    };

    return QtConcurrent::run(deleteMessageWorker, getConnectionSettings(), uid, folderPath);
}

void EmailAccount::deleteMessage(const int uid, const int folderId)
{
    QString folderPath = DatabaseManager::getFolderPath(folderId);
    folderPath = folderPath.remove(0, 1);

    QFuture<bool> future = deleteMessageThread(uid, folderPath);
    QFutureWatcher<bool> *watcher = new QFutureWatcher<bool>();

    connect(watcher, &QFutureWatcher<bool>::finished, [=]()
    {
        if (future.result())
        {
            DatabaseManager::deleteMessageFromDatabase(static_cast<unsigned long>(uid), m_emailAddress);
        }
    });

    watcher->setFuture(future);
}

void EmailAccount::renameFolder(const QString originalFolderPath, const QString renamedFolderPath)
{
    QFuture<bool> future = renameFolderThread(originalFolderPath, renamedFolderPath);
    QFutureWatcher<bool> *watcher = new QFutureWatcher<bool>();

    connect(watcher, &QFutureWatcher<void>::finished, [=]()
    {
        if (future.result())
        {
            DatabaseManager::renameFolder(m_emailAddress, originalFolderPath, renamedFolderPath);
        }
    });

    watcher->setFuture(future);
}

QFuture<bool> EmailAccount::renameFolderThread(const QString originalFolderPath, const QString renamedFolderPath)
{
    auto renameFolderWorker = [](
            const connectionSettingsHolder settings,
            const QString originalFolderPath,
            const QString renamedFolderPath)
    {
        VmimeImapService imapService;

        imapService.setEmailAddress(settings.emailAddress);
        imapService.setUserName(settings.imapServerUserName);
        imapService.setPassword(settings.imapServerPassword);
        imapService.setServerUrl(settings.imapServerAddress);
        imapService.setPort(settings.imapServerPort);
        imapService.setIsConnectionEncrypted(settings.isImapServerConnectionSecured);

        return imapService.renameFolder(originalFolderPath, renamedFolderPath);
    };

    return QtConcurrent::run(renameFolderWorker, getConnectionSettings(), originalFolderPath, renamedFolderPath);
}

void EmailAccount::deleteFolder(const QString folderPath)
{
    QFuture<bool> future = deleteFolderThread(folderPath);
    QFutureWatcher<bool> *watcher = new QFutureWatcher<bool>();

    connect(watcher, &QFutureWatcher<bool>::finished, [=]()
    {
        if (future.result())
        {
            DatabaseManager::deleteFolderFromDatabase(m_emailAddress, folderPath);
        }
    });

    watcher->setFuture(future);
}

QFuture<bool> EmailAccount::deleteFolderThread(const QString folderPath)
{
    auto deleteFolderWorker = [](
            const connectionSettingsHolder settings,
            const QString folderPath)
    {
        VmimeImapService imapService;

        imapService.setEmailAddress(settings.emailAddress);
        imapService.setUserName(settings.imapServerUserName);
        imapService.setPassword(settings.imapServerPassword);
        imapService.setServerUrl(settings.imapServerAddress);
        imapService.setPort(settings.imapServerPort);
        imapService.setIsConnectionEncrypted(settings.isImapServerConnectionSecured);

        return imapService.deleteFolder(folderPath);
    };

    return QtConcurrent::run(deleteFolderWorker, getConnectionSettings(), folderPath);
}

void EmailAccount::createFolder(const QString folderPath)
{
    QFuture<bool> future = createFolderThread(folderPath);
    QFutureWatcher<bool> *watcher = new QFutureWatcher<bool>();

    connect(watcher, &QFutureWatcher<bool>::finished, [=]()
    {
        if (future.result())
        {
            InboxFolder folder;

            folder.setPath("/" + folderPath);
            folder.setEmailAddress(m_emailAddress);
            folder.setIsAllMessages(false);
            folder.setIsArchive(false);
            folder.setIsDrafts(false);
            folder.setIsFlagged(false);
            folder.setIsImportant(false);
            folder.setIsJunk(false);
            folder.setIsSent(false);
            folder.setIsTrash(false);
            folder.setHasChildren(false);

            QList<InboxFolder> folders;
            folders << folder;

            DatabaseManager::addFoldersToDatabase(folders);
        }
    });

    watcher->setFuture(future);
}

QFuture<bool> EmailAccount::createFolderThread(const QString folderPath)
{
    auto createFolderWorker = [](
            const connectionSettingsHolder settings,
            const QString folderPath)
    {
        VmimeImapService imapService;

        imapService.setEmailAddress(settings.emailAddress);
        imapService.setUserName(settings.imapServerUserName);
        imapService.setPassword(settings.imapServerPassword);
        imapService.setServerUrl(settings.imapServerAddress);
        imapService.setPort(settings.imapServerPort);
        imapService.setIsConnectionEncrypted(settings.isImapServerConnectionSecured);

        return imapService.createFolder(folderPath);
    };

    return QtConcurrent::run(createFolderWorker, getConnectionSettings(), folderPath);
}

void EmailAccount::copyMessage(const int uid, const QString oldPath, const QString newPath)
{
    QFuture<long> future = copyMessageThread(uid, oldPath, newPath);
    QFutureWatcher<long> *watcher = new QFutureWatcher<long>();

    connect(watcher, &QFutureWatcher<long>::finished, [=]()
    {   
        long copiedUid = future.result();

        if (copiedUid > 0)
        {
            DatabaseManager::copyMessage(m_emailAddress, oldPath, newPath, static_cast<unsigned long>(uid), static_cast<unsigned long>(copiedUid));
        }
    });

    watcher->setFuture(future);
}

QFuture<long> EmailAccount::copyMessageThread(const int uid, const QString oldPath, const QString newPath)
{
    auto copyMessageWorker = [](
            const connectionSettingsHolder settings,
            const int uid,
            const QString oldPath,
            const QString newPath)
    {
        VmimeImapService imapService;

        imapService.setEmailAddress(settings.emailAddress);
        imapService.setUserName(settings.imapServerUserName);
        imapService.setPassword(settings.imapServerPassword);
        imapService.setServerUrl(settings.imapServerAddress);
        imapService.setPort(settings.imapServerPort);
        imapService.setIsConnectionEncrypted(settings.isImapServerConnectionSecured);

        return imapService.copyMessage(uid, oldPath, newPath);
    };

    return QtConcurrent::run(copyMessageWorker, getConnectionSettings(), uid, oldPath, newPath);
}

QFuture<long> EmailAccount::moveMessageThread(const int uid, const QString oldPath, const QString newPath)
{
    auto moveMessageWorker = [](
            const connectionSettingsHolder settings,
            const int uid,
            const QString oldPath,
            const QString newPath)
    {
        VmimeImapService imapService;

        imapService.setEmailAddress(settings.emailAddress);
        imapService.setUserName(settings.imapServerUserName);
        imapService.setPassword(settings.imapServerPassword);
        imapService.setServerUrl(settings.imapServerAddress);
        imapService.setPort(settings.imapServerPort);
        imapService.setIsConnectionEncrypted(settings.isImapServerConnectionSecured);

        return imapService.moveMessage(uid, oldPath, newPath);
    };

    return QtConcurrent::run(moveMessageWorker, getConnectionSettings(), uid, oldPath, newPath);
}

void EmailAccount::moveMessage(const int uid, const QString oldPath, const QString newPath)
{
    QFuture<long> future = moveMessageThread(uid, oldPath, newPath);
    QFutureWatcher<long> *watcher = new QFutureWatcher<long>();

    connect(watcher, &QFutureWatcher<long>::finished, [=](){
        long movedUid = future.result();

        if (movedUid > 0)
        {
            DatabaseManager::moveMessage(m_emailAddress, oldPath, newPath, static_cast<unsigned long>(uid), static_cast<unsigned long>(movedUid));
        }
    });

    watcher->setFuture(future);
}

void EmailAccount::setMessageAsSeen(const int uid)
{
    QString folderPath = DatabaseManager::getFolderPath(m_emailAddress, static_cast<unsigned long>(uid));

    QFuture<bool> future = setMessageAsSeenThread(uid, folderPath);
    QFutureWatcher<bool> *watcher = new QFutureWatcher<bool>();


    connect(watcher, &QFutureWatcher<bool>::finished, [=]()
    {
        if (future.result())
        {
            DatabaseManager::setMessageAsSeen(static_cast<unsigned long>(uid), m_emailAddress);
        }
    });

    watcher->setFuture(future);
}

QFuture<bool> EmailAccount::setMessageAsSeenThread(const int uid, const QString folderPath)
{
    auto setMessageAsSeenWorker = [](
            const connectionSettingsHolder settings,
            const int uid,
            const QString folderPath)
    {
        VmimeImapService imapService;

        imapService.setEmailAddress(settings.emailAddress);
        imapService.setUserName(settings.imapServerUserName);
        imapService.setPassword(settings.imapServerPassword);
        imapService.setServerUrl(settings.imapServerAddress);
        imapService.setPort(settings.imapServerPort);
        imapService.setIsConnectionEncrypted(settings.isImapServerConnectionSecured);

        return imapService.setMessageAsSeen(uid, folderPath);
    };

    return QtConcurrent::run(setMessageAsSeenWorker, getConnectionSettings(), uid, folderPath);
}

QDebug operator<<(QDebug debug, const EmailAccount &account)
{
    QDebugStateSaver saver(debug);

    debug.noquote() << "\nContact name: "
                    << account.contactName()
                    << "\nEmail address: "
                    << account.emailAddress()

                    << "\nIMAP server user name: "
                    << account.imapServerUserName()
                    << "\nIMAP server password: "
                    << account.imapServerPassword()
                    << "\nIncoming server address: "
                    << account.imapServerAddress()
                    << "\nIncoming server port: "
                    << account.imapServerPort()
                    << "\nIs IMAP server connection secured: "
                    << account.isImapServerConnectionEncrypted()

                    << "\nSMTP server user name: "
                    << account.smtpServerUserName()
                    << "\nSMTP server password: "
                    << account.smtpServerPassword()
                    << "\nSMTP server address: "
                    << account.smtpServerAddress()
                    << "\nSMTP server port: "
                    << account.smtpServerPort()
                    << "\nIs SMTP server connection secured: "
                    << account.isSmtpServerConnectionEncrypted();

    return debug;
}

}
