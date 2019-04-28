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

#ifndef MESSAGEMETADATA_H
#define MESSAGEMETADATA_H

#include <QObject>
#include <QDateTime>
#include "contact.h"

namespace Otter
{

class MessageMetadata : public QObject
{
    Q_OBJECT
public:
    explicit MessageMetadata(QObject *parent = nullptr)
        : QObject(parent) {}
    MessageMetadata(const MessageMetadata &other);
    MessageMetadata &operator=(const MessageMetadata &other);

    QString emailAddress() const;
    void setEmailAddress(const QString emailAddress);

    QString folderPath() const;
    void setFolderPath(const QString folderPath);

    unsigned long uid() const;
    void setUid(const unsigned long uid);

    unsigned long size() const;
    void setSize(const unsigned long size);

    QDateTime dateTime() const;
    void setDateTime(const QDateTime dateTime);

    Contact from() const;
    void setFrom(const Contact from);

    QList<Contact> replyTo() const;
    void setReplyTo(const QList<Contact> replyTo);

    QString subject() const;
    void setSubject(const QString subject);

    bool isDeleted() const;
    void setIsDeleted(const bool isDeleted);

    bool isDraft() const;
    void setIsDraft(const bool isDraft);

    bool isRecent() const;
    void setIsRecent(const bool isRecent);

    bool isReplied() const;
    void setIsReplied(const bool isReplied);

    bool isSeen() const;
    void setIsSeen(const bool isSeen);

signals:

public slots:

private:
    QString m_emailAddress;
    QString m_folderPath;
    unsigned long m_uid;
    unsigned long m_size;
    QDateTime m_dateTime;
    Contact m_from;
    QList<Contact> m_replyTo;
    QString m_subject;
    bool m_isDeleted;
    bool m_isDraft;
    bool m_isRecent;
    bool m_isReplied;
    bool m_isSeen;
};

QDebug operator<<(QDebug debug, const MessageMetadata &metadata);
bool operator==(const MessageMetadata first, const MessageMetadata second);
}

#endif // MESSAGEMETADATA_H
