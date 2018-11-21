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

#ifndef MESSAGECONTENT_H
#define MESSAGECONTENT_H

#include <QObject>
#include "attachment.h"
#include "contact.h"
#include "embeddedobject.h"

namespace Otter
{

class MessageContent : public QObject
{
    Q_OBJECT
public:
    explicit MessageContent(QObject *parent = nullptr)
        :QObject(parent) {}

    MessageContent(const MessageContent &other);

    MessageContent &operator=(const MessageContent &other);
    bool operator==(const MessageContent &other);
    bool operator!=(const MessageContent &other);
    bool operator==(MessageContent &other);
    bool operator!=(MessageContent &other);

    QString htmlContent() const;
    void setHtmlContent(const QString htmlContent);

    QString plainTextContent() const;
    void setPlainTextContent(const QString plainTextContent);

    QList<Attachment> attachments() const;
    void setAttachments(const QList<Attachment> attachments);

    QList<EmbeddedObject> embeddedObjects() const;
    void setEmbeddedObjects(const QList<EmbeddedObject> embeddedObjects);

    QList<Contact> recipients() const;
    void setRecipients(const QList<Contact> recipients);

    QList<Contact> copyRecipients() const;
    void setCopyRecipients(const QList<Contact> copyRecipients);

    QList<Contact> blindCopyRecipients() const;
    void setBlindCopyRecipients(const QList<Contact> blindCopyRecipients);

signals:

public slots:

private:
    QString m_htmlContent;
    QString m_plainTextContent;

    QList<Attachment> m_attachments;
    QList<EmbeddedObject> m_embeddedObjects;

    QList<Contact> m_recipients;
    QList<Contact> m_copyRecipients;
    QList<Contact> m_blindCopyRecipients;
};

QDebug operator<<(QDebug debug, const MessageContent &messageContent);

}

#endif // MESSAGECONTENT_H
