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

#include "message.h"

namespace Otter
{

Message::Message(const Message &other)
{
    m_sender = other.m_sender;
    m_subject = other.m_subject;

    m_addressListTo = other.m_addressListTo;
    m_addressListInCopy = other.m_addressListInCopy;
    m_addressListInBlindCopy = other.m_addressListInBlindCopy;
    m_addressListReplyTo = other.m_addressListReplyTo;

    m_plainTextContent = other.m_plainTextContent;
    m_htmlContent = other.m_htmlContent;

    m_attachments = other.m_attachments;
    m_embeddedObjects = other.m_embeddedObjects;

    m_dateTime = other.m_dateTime;
}

Message& Message::operator=(const Message &other)
{
    m_sender = other.m_sender;
    m_subject = other.m_subject;

    m_addressListTo = other.m_addressListTo;
    m_addressListInCopy = other.m_addressListInCopy;
    m_addressListInBlindCopy = other.m_addressListInBlindCopy;
    m_addressListReplyTo = other.m_addressListReplyTo;

    m_plainTextContent = other.m_plainTextContent;
    m_htmlContent = other.m_htmlContent;

    m_attachments = other.m_attachments;
    m_embeddedObjects = other.m_embeddedObjects;

    m_dateTime = other.m_dateTime;

    return *this;
}

Contact Message::sender() const
{
    return m_sender;
}

void Message::setSender(const Contact sender)
{
    m_sender = sender;
}

QString Message::subject() const
{
    return m_subject;
}

void Message::setSubject(const QString subject)
{
    m_subject = subject;
}

QList<Contact> Message::addressListTo() const
{
    return m_addressListTo;
}

void Message::setAddressListTo(const QList<Contact> addressListTo)
{
    m_addressListTo = addressListTo;
}

QList<Contact> Message::addressListInCopy() const
{
    return m_addressListInCopy;
}

void Message::setAddressListInCopy(const QList<Contact> addressListInCopy)
{
    m_addressListInCopy = addressListInCopy;
}

QList<Contact> Message::addressListInBlindCopy() const
{
    return m_addressListInBlindCopy;
}

void Message::setAddressListInBlindCopy(const QList<Contact> addressListInBlindCopy)
{
    m_addressListInBlindCopy = addressListInBlindCopy;
}

QList<Contact> Message::replyTo() const
{
    return m_addressListReplyTo;
}

void Message::setReplyTo(const QList<Contact> replyTo)
{
    m_addressListReplyTo = replyTo;
}

QString Message::plainTextContent() const
{
    return m_plainTextContent;
}

void Message::setPlainTextContent(const QString plainTextContent)
{
    m_plainTextContent = plainTextContent;
}

QString Message::htmlContent() const
{
    return m_htmlContent;
}

void Message::setHtmlContent(const QString htmlContent)
{
    m_htmlContent = htmlContent;
}

QList<Attachment> Message::attachments() const
{
    return m_attachments;
}

void Message::setAttachments(const QList<Attachment> attachments)
{
    m_attachments = attachments;
}

QList<EmbeddedObject> Message::embeddedObjects() const
{
    return m_embeddedObjects;
}

void Message::setEmbeddedObjects(QList<EmbeddedObject> embeddedObjects)
{
    m_embeddedObjects = embeddedObjects;
}

QDateTime Message::dateTime() const
{
    return m_dateTime;
}

void Message::setDateTime(const QDateTime dateTime)
{
    m_dateTime = dateTime;
}

bool operator==(const Message &message1, const Message &message2)
{
    Contact m_sender;
    QString m_subject;

    QList<Contact> m_addressListTo;
    QList<Contact> m_addressListInCopy;
    QList<Contact> m_addressListInBlindCopy;
    QList<Contact> m_addressListReplyTo;

    QString m_plainTextContent;
    QString m_htmlContent;

    QList<Attachment> m_attachments;
    QList<EmbeddedObject> m_embeddedObjects;

    QDateTime m_dateTime;

    return message1.sender() == message2.sender()
            && message1.subject() == message2.subject()
            && message1.addressListTo() == message2.addressListTo()
            && message1.addressListInCopy() == message2.addressListInCopy()
            && message1.addressListInBlindCopy() == message2.addressListInBlindCopy()
            && message1.replyTo() == message2.replyTo()
            && message1.plainTextContent() == message2.plainTextContent()
            && message1.htmlContent() == message2.htmlContent()
            && message1.attachments() == message2.attachments()
            && message1.embeddedObjects() == message2.embeddedObjects()
            && message1.dateTime() == message2.dateTime();
}

QDebug operator<<(QDebug debug, const Message &message)
{
    QDebugStateSaver saver(debug);

    debug.noquote() << "Sender: "
                    << message.sender()
                    << "\nSubject: "
                    << message.subject()
                    << "\nAddressListTo: ";

    for (Contact address : message.addressListTo())
    {
        debug.noquote() << address << ", ";
    }

    debug.noquote() << "\nAddressListInCopy: ";

    for (Contact address : message.addressListInCopy())
    {
        debug.noquote() << address << ", ";
    }

    debug.noquote() << "\nAddressListInBlindCopy: ";

    for (Contact address : message.addressListInBlindCopy())
    {
        debug.noquote() << address << ", ";
    }

    debug.noquote() << "\nAddressListReplyTo: ";

    for (Contact address : message.replyTo())
    {
        debug.noquote() << address << ", ";
    }

    debug.noquote() << "\nAttachments: ";

    for (Attachment attachment : message.attachments())
    {
        debug.noquote() << attachment.name() << ", ";
    }

    debug.noquote() << "\nEmbedded objects: ";

    for (Attachment embeddedObject : message.embeddedObjects())
    {
        debug.noquote() << embeddedObject.name() << ", ";
    }

    debug.noquote() << "\nDate and time: " << message.dateTime().toString("dd.MM.yyyy HH:mm");

    return debug;
}

}
