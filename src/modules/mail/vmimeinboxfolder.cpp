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

#include "vmimeinboxfolder.h"

namespace Otter
{

VmimeInboxFolder::VmimeInboxFolder(vmime::shared_ptr<vmime::net::folder> remoteFolder, QString emailAddress, QObject *parent)
    : QObject(parent)
{
    m_remoteFolder = remoteFolder;

    VmimeInboxFolderParser parser(m_remoteFolder);
    m_inboxFolder = parser.parse();
    m_inboxFolder.setEmailAddress(emailAddress);

    openFolder();
}

void VmimeInboxFolder::openFolder()
{
    try {
        m_remoteFolder->open(vmime::net::folder::MODE_READ_WRITE);
        m_successfullyOpened = true;
    } catch (vmime::exceptions::authentication_error e) {
        m_successfullyOpened = false;
        // qWarning() << "Failure while opening folder " << m_inboxFolder.path() << " on " << m_inboxFolder.emailAddress() << " Authentication error";
    } catch (vmime::exceptions::command_error e)
    {
        m_successfullyOpened = false;
        // qWarning() << "Failure while opening folder " << m_inboxFolder.path() << " on " << m_inboxFolder.emailAddress() << " Command error";
    }
}

VmimeInboxFolder::VmimeInboxFolder(const VmimeInboxFolder &other)
{
    m_remoteFolder = other.m_remoteFolder;
    m_inboxFolder = other.m_inboxFolder;
    m_successfullyOpened = other.m_successfullyOpened;
}

VmimeInboxFolder& VmimeInboxFolder::operator=(const VmimeInboxFolder &other)
{
    m_remoteFolder = other.m_remoteFolder;
    m_inboxFolder = other.m_inboxFolder;
    m_successfullyOpened = other.m_successfullyOpened;

    return *this;
}

InboxFolder VmimeInboxFolder::data() const
{
    return m_inboxFolder;
}

void VmimeInboxFolder::setData(const InboxFolder inboxFolder)
{
    m_inboxFolder = inboxFolder;
}

QList<MessageMetadata> VmimeInboxFolder::getMessagesMetadataFromPosition(int position) const
{
    if (!m_successfullyOpened)
    {
        return QList<MessageMetadata>();
    }

    size_t messagesCount = m_remoteFolder->getMessageCount();

    if (position == 0 && messagesCount == 1)
    {
        vmime::shared_ptr<vmime::net::message> message = m_remoteFolder->getMessage(1);

        try {
            m_remoteFolder->fetchMessage(message,
                                         vmime::net::fetchAttributes::ENVELOPE |
                                         vmime::net::fetchAttributes::SIZE |
                                         vmime::net::fetchAttributes::FLAGS |
                                         vmime::net::fetchAttributes::UID);
        }
        catch (std::exception e)
        {
            m_remoteFolder->fetchMessage(message,
                                         vmime::net::fetchAttributes::ENVELOPE |
                                         vmime::net::fetchAttributes::SIZE |
                                         vmime::net::fetchAttributes::UID);
        }

        VmimeMessageMetadataParser parser;
        MessageMetadata metadata = parser.parse(message);
        metadata.setEmailAddress(m_inboxFolder.emailAddress());
        metadata.setFolderPath(m_inboxFolder.path());

        QList<MessageMetadata> metadataList;
        metadataList << metadata;

        return metadataList;
    }
    else if (position == 0)
    {
        position++;
    }

    QList<MessageMetadata> metadataList;

    if ((static_cast<vmime::size_t>(position) < messagesCount) && (messagesCount > 0))
    {
        std::vector<vmime::shared_ptr<vmime::net::message>> messages =
                m_remoteFolder->getMessages(vmime::net::messageSet::byNumber(static_cast<vmime::size_t>(position), messagesCount));

        try
        {
            m_remoteFolder->fetchMessages(messages,
                                          vmime::net::fetchAttributes::ENVELOPE |
                                          vmime::net::fetchAttributes::FLAGS |
                                          vmime::net::fetchAttributes::SIZE |
                                          vmime::net::fetchAttributes::UID);
        }
        catch (std::exception e)
        {
            m_remoteFolder->fetchMessages(messages,
                                          vmime::net::fetchAttributes::ENVELOPE |
                                          vmime::net::fetchAttributes::SIZE |
                                          vmime::net::fetchAttributes::UID);
        }


        VmimeMessageMetadataParser parser;

        for (unsigned int i = 0; i < messages.size(); i++)
        {
            vmime::shared_ptr<vmime::net::message> message = messages[i];
            MessageMetadata metadata = parser.parse(message);
            metadata.setEmailAddress(m_inboxFolder.emailAddress());
            metadata.setFolderPath(m_inboxFolder.path());

            metadataList.push_back(metadata);
        }
    }

    return metadataList;
}

}
