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

#include "messagemetadata.h"

namespace Otter
{

MessageMetadata::MessageMetadata(const MessageMetadata &other)
{
    m_emailAddress = other.m_emailAddress;
    m_folderPath = other.m_folderPath;
    m_uid = other.m_uid;
    m_size = other.m_size;
    m_dateTime = other.m_dateTime;
    m_from = other.m_from;
    m_replyTo = other.m_replyTo;
    m_subject = other.m_subject;
    m_isDeleted = other.m_isDeleted;
    m_isDraft = other.m_isDraft;
    m_isRecent = other.m_isRecent;
    m_isReplied = other.m_isReplied;
    m_isSeen = other.m_isSeen;
}

MessageMetadata& MessageMetadata::operator=(const MessageMetadata &other)
{
    m_emailAddress = other.m_emailAddress;
    m_folderPath = other.m_folderPath;
    m_uid = other.m_uid;
    m_size = other.m_size;
    m_dateTime = other.m_dateTime;
    m_from = other.m_from;
    m_replyTo = other.m_replyTo;
    m_subject = other.m_subject;
    m_isDeleted = other.m_isDeleted;
    m_isDraft = other.m_isDraft;
    m_isRecent = other.m_isRecent;
    m_isReplied = other.m_isReplied;
    m_isSeen = other.m_isSeen;

    return *this;
}

QString MessageMetadata::emailAddress() const
{
    return m_emailAddress;
}

void MessageMetadata::setEmailAddress(const QString emailAddress)
{
    m_emailAddress = emailAddress;
}

QString MessageMetadata::folderPath() const
{
    return m_folderPath;
}

void MessageMetadata::setFolderPath(const QString folderPath)
{
    m_folderPath = folderPath;
}

unsigned long MessageMetadata::uid() const
{
    return m_uid;
}

void MessageMetadata::setUid(const unsigned long messageId)
{
    m_uid = messageId;
}

unsigned long MessageMetadata::size() const
{
    return m_size;
}

void MessageMetadata::setSize(const unsigned long size)
{
    m_size = size;
}

QDateTime MessageMetadata::dateTime() const
{
    return m_dateTime;
}

void MessageMetadata::setDateTime(const QDateTime dateTime)
{
    m_dateTime = dateTime;
}

Contact MessageMetadata::from() const
{
    return m_from;
}

void MessageMetadata::setFrom(const Contact from)
{
    m_from = from;
}

QList<Contact> MessageMetadata::replyTo() const
{
    return m_replyTo;
}

void MessageMetadata::setReplyTo(const QList<Contact> replyTo)
{
    m_replyTo = replyTo;
}

QString MessageMetadata::subject() const
{
    return m_subject;
}

void MessageMetadata::setSubject(const QString subject)
{
    m_subject = subject;
}

bool MessageMetadata::isDeleted() const
{
    return m_isDeleted;
}

void MessageMetadata::setIsDeleted(const bool isDeleted)
{
    m_isDeleted = isDeleted;
}

bool MessageMetadata::isDraft() const
{
    return m_isDraft;
}

void MessageMetadata::setIsDraft(const bool isDraft)
{
    m_isDraft = isDraft;
}

bool MessageMetadata::isRecent() const
{
    return m_isRecent;
}

void MessageMetadata::setIsRecent(const bool isRecent)
{
    m_isRecent = isRecent;
}

bool MessageMetadata::isReplied() const
{
    return m_isReplied;
}

void MessageMetadata::setIsReplied(const bool isReplied)
{
    m_isReplied = isReplied;
}

bool MessageMetadata::isSeen() const
{
    return m_isSeen;
}

void MessageMetadata::setIsSeen(const bool isSeen)
{
    m_isSeen = isSeen;
}

QDebug operator<<(QDebug debug, const MessageMetadata &metadata)
{
    QDebugStateSaver saver(debug);

    debug.noquote() << "Email address: "
                    << metadata.emailAddress()
                    << "\nFolder path: "
                    << metadata.folderPath()
                    << "UID: "
                    << metadata.uid()
                    << "\nSize: "
                    << metadata.size()
                    << "\nDate and time: "
                    << metadata.dateTime().toString("")
                    << "\nFrom: "
                    << metadata.from()
                    << "\nReply-To: "
                    << metadata.replyTo()
                    << "\nSubject: "
                    << metadata.subject()
                    << "\nIs deleted: "
                    << metadata.isDeleted()
                    << "\nIs draft: "
                    << metadata.isDraft()
                    << "\nIs recent: "
                    << metadata.isRecent()
                    << "\nIs replied: "
                    << metadata.isReplied()
                    << "\nIs seen: "
                    << metadata.isSeen();

    return debug;
}

bool operator==(const MessageMetadata first, const MessageMetadata second)
{
    return first.emailAddress() == second.emailAddress()
            && first.folderPath() == second.folderPath()
            && first.uid() == second.uid()
            && first.size() == second.size()
            && first.dateTime() == second.dateTime()
            && first.from() == second.from()
            && first.replyTo() == second.replyTo()
            && first.subject() == second.subject()
            && first.isDeleted() == second.isDeleted()
            && first.isDraft() == second.isDraft()
            && first.isRecent() == second.isRecent()
            && first.isReplied() == second.isReplied()
            && first.isSeen() == second.isSeen();
}

}
