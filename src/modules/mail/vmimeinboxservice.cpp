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

#include "vmimeinboxservice.h"
#include "../../core/NotificationsManager.h"

namespace Otter
{

QList<InboxFolder> VmimeInboxService::fetchInboxFolders()
{
    if (!initializeStore())
    {
        return QList<InboxFolder>();
    }

    QList<InboxFolder> folders;

    try {
        vmime::shared_ptr<vmime::net::folder> rootFolder = m_store->getRootFolder();
        std::vector<vmime::shared_ptr<vmime::net::folder>> subFolders = rootFolder->getFolders(true);
        subFolders.push_back(rootFolder);

        for (vmime::shared_ptr<vmime::net::folder> folder : subFolders)
        {
            VmimeInboxFolderParser parser(folder);
            InboxFolder inboxFolder = parser.parse();
            inboxFolder.setEmailAddress(QString(m_emailAddress.c_str()));

            folders.push_back(inboxFolder);
        }

        m_store->disconnect();
    }
    catch (vmime::exception e)
    {
        QString notificationText = m_emailAddress.c_str();
        notificationText.append(": Failed to fetch IMAP folders: ");
        notificationText.append(e.what());

        qWarning() << notificationText;

        Notification *notification(NotificationsManager::createNotification(NotificationsManager::UpdateAvailableEvent, notificationText));
    }

    return folders;
}

QList<MessageMetadata> VmimeInboxService::fetchMessagesMetadata()
{
    if (!initializeStore())
    {
        return QList<MessageMetadata>();
    }

    QList<MessageMetadata> metadata;
    QList<VmimeInboxFolder> inboxFolders;

    try {
        vmime::shared_ptr<vmime::net::folder> rootFolder = m_store->getRootFolder();
        std::vector<vmime::shared_ptr<vmime::net::folder>> subFolders = rootFolder->getFolders(true);
        subFolders.push_back(rootFolder);

        for (vmime::shared_ptr<vmime::net::folder> folder : subFolders)
        {
            VmimeInboxFolder inboxFolder = VmimeInboxFolder(folder, QString(m_emailAddress.c_str()));
            metadata.append(inboxFolder.getMessagesMetadata());
        }

        m_store->disconnect();
    }
    catch (vmime::exception e)
    {
        QString notificationText = m_emailAddress.c_str();
        notificationText.append(": Failed to fetch metadata about messages: ");
        notificationText.append(e.what());

        qWarning() << notificationText;

        Notification *notification(NotificationsManager::createNotification(NotificationsManager::UpdateAvailableEvent, notificationText));
    }

    return metadata;
}

MessageContent VmimeInboxService::fetchMessageContent(QString folderPath, int uid)
{
    if (!initializeStore())
    {
        return MessageContent();
    }

    try {
        vmime::shared_ptr<vmime::net::folder> rootFolder = m_store->getRootFolder();
        std::vector<vmime::shared_ptr<vmime::net::folder>> folders = rootFolder->getFolders(true);
        folders.push_back(rootFolder);

        for (vmime::shared_ptr<vmime::net::folder> folder : folders)
        {
            if (QString(folder->getFullPath().toString("/", vmime::charsets::UTF_8).c_str()) == folderPath.right(folderPath.length() - 1))
            {
                try
                {
                    folder->open(vmime::net::folder::MODE_READ_ONLY);
                }
                catch (vmime::exceptions::authentication_error e) {

                }
                catch (vmime::exceptions::command_error e)
                {

                }

                vmime::shared_ptr<vmime::net::message> message = folder->getMessages(vmime::net::messageSet::byUID(static_cast<vmime::size_t>(uid))).at(0);

                try
                {
                    folder->fetchMessage(message, vmime::net::fetchAttributes::STRUCTURE);
                }
                catch (std::exception e)
                {

                }

                vmime::shared_ptr<vmime::message> parsedMessage = message->getParsedMessage();

                VmimeMessageContentParser contentParser(parsedMessage);
                MessageContent messageContent;

                messageContent.setHtmlContent(contentParser.parseHtmlContent());
                messageContent.setPlainTextContent(contentParser.parsePlaintextContent());
                messageContent.setAttachments(contentParser.parseAttachments());
                messageContent.setEmbeddedObjects(contentParser.parseEmbeddedObjects());
                messageContent.setRecipients(contentParser.parseRecipients());
                messageContent.setCopyRecipients(contentParser.parseInCopyRecipients());
                messageContent.setBlindCopyRecipients(contentParser.parseBlindCopyRecipients());

                m_store->disconnect();

                return messageContent;
            }
        }
    }
    catch (vmime::exception e)
    {
        QString notificationText = m_emailAddress.c_str();
        notificationText.append(": Failed to fetch message content: ");
        notificationText.append(e.what());

        qWarning() << notificationText;

        Notification *notification(NotificationsManager::createNotification(NotificationsManager::UpdateAvailableEvent, notificationText));

        return MessageContent();
    }

    return MessageContent();
}

bool VmimeInboxService::initializeStore()
{
    try
    {
        vmime::utility::url url(getStoreUrl().toStdString());
        m_store = m_session->getStore(url);
    }
    catch (vmime::exception e)
    {
        QString notificationText = m_emailAddress.c_str();
        notificationText.append(": Failed to connect create IMAP store: ");
        notificationText.append(e.what());

        qWarning() << notificationText;

        Notification *notification(NotificationsManager::createNotification(NotificationsManager::UpdateAvailableEvent, notificationText));
        return false;
    }

    if (m_isConnectionEncrypted)
    {
        m_store->setCertificateVerifier(m_certificateVerifier);
    }

    int attempts = 0;
    QString errorMessage;
    bool succesfullyConnected = false;

    while (attempts < 10)
    {
        try
        {
            m_store->connect();
            succesfullyConnected = true;
            break;
        }
        catch (vmime::exception e)
        {
            errorMessage = e.what();
            attempts++;
        }
    }

    if (succesfullyConnected)
    {
        return true;
    }
    else
    {
        QString notificationText = m_emailAddress.c_str();
        notificationText.append(": Failed to connect to server: ");
        notificationText.append(errorMessage);

        qWarning() << notificationText;

        Notification *notification(NotificationsManager::createNotification(NotificationsManager::UpdateAvailableEvent, notificationText));
        return false;
    }
}

long VmimeInboxService::moveMessage(const int uid, const QString oldPath, const QString newPath)
{
    long result = copyMessage(uid, oldPath, newPath);

    if (result > 0)
    {
        QString pathToDelete = oldPath;
        pathToDelete.remove(0,1);

        deleteMessage(uid, pathToDelete);
    }

    return result;
}

bool VmimeInboxService::renameFolder(const QString originalFolderPath, const QString renamedFolderPath)
{
    if (initializeStore())
    {
        try {
            vmime::shared_ptr<vmime::net::folder> folder = m_store->getFolder(vmime::net::folder::path(originalFolderPath.toStdString()));
            folder->rename(vmime::net::folder::path(renamedFolderPath.toStdString()));

            m_store->disconnect();

            return true;
        }
        catch (vmime::exception e)
        {
            QString notificationText = m_emailAddress.c_str();
            notificationText.append(": Failed to rename IMAP folder: ");
            notificationText.append(e.what());

            qWarning() << notificationText;

            Notification *notification(NotificationsManager::createNotification(NotificationsManager::UpdateAvailableEvent, notificationText));
        }
    }

    return false;
}

bool VmimeInboxService::deleteFolder(const QString folderPath)
{
    if (initializeStore())
    {
        try {
            QString path = folderPath;
            path.remove(0,1);

            vmime::shared_ptr<vmime::net::folder> folder = m_store->getFolder(vmime::net::folder::path(path.toStdString()));
            folder->destroy();

            m_store->disconnect();

            return true;
        }
        catch (vmime::exception e)
        {
            QString notificationText = m_emailAddress.c_str();
            notificationText.append(": Failed to delete IMAP folder: ");
            notificationText.append(e.what());

            qWarning() << notificationText;

            Notification *notification(NotificationsManager::createNotification(NotificationsManager::UpdateAvailableEvent, notificationText));
        }
    }

    return false;
}

bool VmimeInboxService::deleteMessage(const int uid, QString folderPath)
{
    if (initializeStore())
    {
        try {
            vmime::shared_ptr<vmime::net::folder> folder = m_store->getFolder(vmime::net::folder::path(folderPath.toStdString()));
            folder->open(vmime::net::folder::MODE_READ_WRITE);
            folder->deleteMessages(vmime::net::messageSet::byUID(static_cast<vmime::size_t>(uid)));

            folder->close(false);
            m_store->disconnect();

            return true;
        }
        catch (vmime::exception e)
        {
            QString notificationText = m_emailAddress.c_str();
            notificationText.append(": Failed to delete e-mail message: ");
            notificationText.append(e.what());

            qWarning() << notificationText;

            Notification *notification(NotificationsManager::createNotification(NotificationsManager::UpdateAvailableEvent, notificationText));
        }
    }

    return false;
}

bool VmimeInboxService::createFolder(const QString folderPath)
{
    if (initializeStore())
    {
        try {
            vmime::shared_ptr<vmime::net::folder> folder = m_store->getFolder(vmime::net::folder::path(folderPath.toStdString()));

            vmime::net::folderAttributes attributes = vmime::net::folderAttributes();
            attributes.setType(vmime::net::folderAttributes::Types::TYPE_CONTAINS_MESSAGES);
            attributes.setSpecialUse(vmime::net::folderAttributes::SpecialUses::SPECIALUSE_NONE);

            folder->create(attributes);

            m_store->disconnect();

            return true;
        }
        catch (vmime::exception e)
        {
            QString notificationText = m_emailAddress.c_str();
            notificationText.append(": Failed to create IMAP folder: ");
            notificationText.append(e.what());

            qWarning() << notificationText;

            Notification *notification(NotificationsManager::createNotification(NotificationsManager::UpdateAvailableEvent, notificationText));
        }
    }

    return false;
}

long VmimeInboxService::copyMessage(const int uid, const QString oldPath, const QString newPath)
{
    if (initializeStore())
    {
        try
        {
            QString sourcePath = oldPath;
            sourcePath = sourcePath.remove(0, 1);

            QString destinationPath = newPath;
            destinationPath = destinationPath.remove(0, 1);

            vmime::shared_ptr<vmime::net::folder> sourceFolder = m_store->getFolder(vmime::net::folder::path(sourcePath.toStdString()));
            vmime::shared_ptr<vmime::net::folder> destinationFolder = m_store->getFolder(vmime::net::folder::path(destinationPath.toStdString()));

            sourceFolder->open(vmime::net::folder::MODE_READ_WRITE);
            destinationFolder->open(vmime::net::folder::MODE_READ_WRITE);

            vmime::shared_ptr<vmime::net::message> message = sourceFolder->getMessages(vmime::net::messageSet::byUID(static_cast<vmime::size_t>(uid))).at(0);
            vmime::net::messageSet set = sourceFolder->copyMessages(destinationFolder->getFullPath(), vmime::net::messageSet::byNumber(message->getNumber()));

            const vmime::net::messageRange& range = set.getRangeAt(0);
            const vmime::net::message::uid copiedUid = dynamic_cast<const vmime::net::UIDMessageRange&>(range).getFirst();

            sourceFolder->close(false);
            destinationFolder->close(false);

            m_store->disconnect();

            return atol(static_cast<std::string>(copiedUid).c_str());
        }
        catch (vmime::exception e)
        {
            QString notificationText = m_emailAddress.c_str();
            notificationText.append(": Failed to copy email message: ");
            notificationText.append(e.what());

            qWarning() << notificationText;

            Notification *notification(NotificationsManager::createNotification(NotificationsManager::UpdateAvailableEvent, notificationText));
        }
    }

    return -1;
}

bool VmimeInboxService::setMessageAsSeen(const int uid, const QString folderPath)
{
    if (initializeStore())
    {
        try
        {
            QString path = folderPath;
            path = path.remove(0, 1);

            vmime::shared_ptr<vmime::net::folder> folder = m_store->getFolder(vmime::net::folder::path(path.toStdString()));
            folder->open(vmime::net::folder::MODE_READ_WRITE);
            vmime::shared_ptr <vmime::net::message> msg = folder->getMessages(vmime::net::messageSet::byUID(static_cast<vmime::size_t>(uid))).at(0);

            folder->fetchMessage(msg, vmime::net::fetchAttributes::FLAGS);

            auto flags = msg->getFlags();
            flags = flags | vmime::net::message::FLAG_SEEN;

            msg->setFlags(flags, vmime::net::message::FLAG_MODE_SET);

            folder->close(false);
            m_store->disconnect();

            return true;
        }
        catch (vmime::exception e)
        {
            QString notificationText = m_emailAddress.c_str();
            notificationText.append(": Failed to set message as seen on server: ");
            notificationText.append(e.what());

            qWarning() << notificationText;

            Notification *notification(NotificationsManager::createNotification(NotificationsManager::UpdateAvailableEvent, notificationText));
        }
    }

    return false;
}

bool VmimeInboxService::setMessageAsUnseen(const int uid, const QString folderPath)
{
    if (initializeStore())
    {
        try
        {
            QString path = folderPath;
            path = path.remove(0, 1);

            vmime::shared_ptr<vmime::net::folder> folder = m_store->getFolder(vmime::net::folder::path(path.toStdString()));
            folder->open(vmime::net::folder::MODE_READ_WRITE);
            vmime::shared_ptr <vmime::net::message> msg = folder->getMessages(vmime::net::messageSet::byUID(static_cast<vmime::size_t>(uid))).at(0);

            folder->fetchMessage(msg, vmime::net::fetchAttributes::FLAGS);

            auto flags = msg->getFlags();
            flags &= ~vmime::net::message::FLAG_SEEN;
            msg->setFlags(flags, vmime::net::message::FLAG_MODE_SET);

            folder->close(false);
            m_store->disconnect();

            return true;
        }
        catch (vmime::exception e)
        {
            QString notificationText = m_emailAddress.c_str();
            notificationText.append(": Failed to set message as unseen on server: ");
            notificationText.append(e.what());

            qWarning() << notificationText;

            Notification *notification(NotificationsManager::createNotification(NotificationsManager::UpdateAvailableEvent, notificationText));
        }
    }

    return false;
}

}
