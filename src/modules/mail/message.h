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

#ifndef MESSAGE_H
#define MESSAGE_H

#include <QObject>
#include <QDebug>
#include "attachment.h"
#include "contact.h"
#include "embeddedobject.h"
#include <QDateTime>

namespace Otter
{

class Message : public QObject
{
    Q_OBJECT
public:
    explicit Message(QObject *parent = nullptr) : QObject(parent) {}
    Message(const Message &other);
    Message &operator=(const Message &other);

    Contact sender() const;
    void setSender(const Contact sender);

    QString subject() const;
    void setSubject(const QString subject);

    QList<Contact> addressListTo() const;
    void setAddressListTo(const QList<Contact> addressListTo);

    QList<Contact> addressListInCopy() const;
    void setAddressListInCopy(const QList<Contact> addressListInCopy);

    QList<Contact> addressListInBlindCopy() const;
    void setAddressListInBlindCopy(const QList<Contact> addressListInBlindCopy);

    QList<Contact> replyTo() const;
    void setReplyTo(const QList<Contact> replyTo);

    QString plainTextContent() const;
    void setPlainTextContent(const QString plainTextContent);

    QString htmlContent() const;
    void setHtmlContent(const QString htmlContent);

    QList<Attachment> attachments() const;
    void setAttachments(const QList<Attachment> attachments);

    QList<EmbeddedObject> embeddedObjects() const;
    void setEmbeddedObjects(QList<EmbeddedObject> embeddedObjects);

    QDateTime dateTime() const;
    void setDateTime(const QDateTime dateTime);

signals:

public slots:

private:
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
};

QDebug operator<<(QDebug debug, const Message &message);

}

#endif // MESSAGE_H
