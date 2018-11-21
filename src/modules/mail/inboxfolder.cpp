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

#include "inboxfolder.h"

namespace Otter
{

InboxFolder::InboxFolder(const InboxFolder &other)
{
    m_path = other.m_path;
    m_emailAddress = other.m_emailAddress;
    m_isAllMessages = other.m_isAllMessages;
    m_isArchive = other.m_isArchive;
    m_isDrafts = other.m_isDrafts;
    m_isFlagged = other.m_isFlagged;
    m_isImportant = other.m_isImportant;
    m_isJunk = other.m_isJunk;
    m_isSent = other.m_isSent;
    m_isTrash = other.m_isTrash;
    m_hasChildren = other.m_hasChildren;
}

InboxFolder& InboxFolder::operator=(const InboxFolder &other)
{
    m_path = other.m_path;
    m_emailAddress = other.m_emailAddress;
    m_isAllMessages = other.m_isAllMessages;
    m_isArchive = other.m_isArchive;
    m_isDrafts = other.m_isDrafts;
    m_isFlagged = other.m_isFlagged;
    m_isImportant = other.m_isImportant;
    m_isJunk = other.m_isJunk;
    m_isSent = other.m_isSent;
    m_isTrash = other.m_isTrash;
    m_hasChildren = other.m_hasChildren;

    return *this;
}

QString InboxFolder::path() const
{
    return m_path;
}

void InboxFolder::setPath(const QString path)
{
    m_path = path;
}

QString InboxFolder::emailAddress() const
{
    return m_emailAddress;
}

void InboxFolder::setEmailAddress(const QString emailAddress)
{
    m_emailAddress = emailAddress;
}

bool InboxFolder::isAllMessages() const
{
    return m_isAllMessages;
}

void InboxFolder::setIsAllMessages(const bool isAllMessages)
{
    m_isAllMessages = isAllMessages;
}

bool InboxFolder::isArchive() const
{
    return m_isArchive;
}

void InboxFolder::setIsArchive(const bool isArchive)
{
    m_isArchive = isArchive;
}

bool InboxFolder::isDrafts() const
{
    return m_isDrafts;
}

void InboxFolder::setIsDrafts(const bool isDrafts)
{
    m_isDrafts = isDrafts;
}

bool InboxFolder::isFlagged() const
{
    return m_isFlagged;
}

void InboxFolder::setIsFlagged(const bool isFlagged)
{
    m_isFlagged = isFlagged;
}

bool InboxFolder::isImportant() const
{
    return m_isImportant;
}

void InboxFolder::setIsImportant(const bool isImportant)
{
    m_isImportant = isImportant;
}

bool InboxFolder::isJunk() const
{
    return m_isJunk;
}

void InboxFolder::setIsJunk(const bool isJunk)
{
    m_isJunk = isJunk;
}

bool InboxFolder::isSent() const
{
    return m_isSent;
}

void InboxFolder::setIsSent(const bool isSent)
{
    m_isSent = isSent;
}

bool InboxFolder::isTrash() const
{
    return m_isTrash;
}

void InboxFolder::setIsTrash(const bool isTrash)
{
    m_isTrash = isTrash;
}

bool InboxFolder::hasChildren() const
{
    return m_hasChildren;
}

void InboxFolder::setHasChildren(const bool hasChildren)
{
    m_hasChildren = hasChildren;
}

QIcon InboxFolder::getIcon() const
{
    if (isArchive())
    {
        return QIcon::fromTheme("mail-read");
    }
    else if (isDrafts())
    {
        return QIcon::fromTheme("mail-read");
    }
    else if (isFlagged())
    {
        return QIcon::fromTheme("emblem-mail");
    }
    else if (isImportant())
    {
        return QIcon::fromTheme("mail-mark-important");
    }
    else if (isJunk())
    {
        return QIcon::fromTheme("mail-mark-junk");
    }
    else if (isTrash())
    {
        return QIcon::fromTheme("user-trash");
    }
    else if (isSent())
    {
        return QIcon::fromTheme("mail-send");
    }
    else if (path() == "/")
    {
        return QIcon::fromTheme("mail-send");
    }
    else
    {
        return QIcon::fromTheme("folder");
    }
}

QDebug operator<<(QDebug debug, const InboxFolder &inboxFolder)
{
    QDebugStateSaver saver(debug);

    debug.noquote() << "Email address: "
                    << inboxFolder.emailAddress()
                    << "\nPath: "
                    << inboxFolder.path()
                    << "\nIsAllMessages: "
                    << inboxFolder.isAllMessages()
                    << "\nIsArchive: "
                    << inboxFolder.isArchive()
                    << "\nIsDrafts: "
                    << inboxFolder.isDrafts()
                    << "\nIsFlagged: "
                    << inboxFolder.isFlagged()
                    << "\nIsImportant: "
                    << inboxFolder.isImportant()
                    << "\nIsJunk: "
                    << inboxFolder.isJunk()
                    << "\nIsSent: "
                    << inboxFolder.isSent()
                    << "\nIsTrash: "
                    << inboxFolder.isTrash()
                    << "\nHasChildren: "
                    << inboxFolder.hasChildren();

    return debug;
}

}
