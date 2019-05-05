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

#ifndef EMAILACCOUNT_H
#define EMAILACCOUNT_H

#include <QObject>
#include <QDebug>
#include <QFuture>
#include <QtConcurrent/QtConcurrent>
#include <QFutureWatcher>
#include "vmimeinboxservice.h"
#include "vmimeinboxfolder.h"
#include "inboxfoldertreeitem.h"
#include "vmimeimapservice.h"
#include "message.h"
#include "vmimesmtpservice.h"
#include "messagecontent.h"

namespace Otter
{

class EmailAccount : public QObject
{
    Q_OBJECT
public:
    explicit EmailAccount(QObject *parent = nullptr);
    EmailAccount(const EmailAccount &other);
    EmailAccount &operator=(const EmailAccount &other);

    struct connectionSettingsHolder
    {
        QString contactName;
        QString emailAddress;

        QString imapServerUserName;
        QString imapServerPassword;
        QString imapServerAddress;
        int imapServerPort;
        bool isImapServerConnectionSecured;

        QString smtpServerUserName;
        QString smtpServerPassword;
        QString smtpServerAddress;
        int smtpServerPort;
        bool isSmtpServerConnectionSecured;
    };

    QString contactName() const;
    void setContactName(const QString contactName);

    QString emailAddress() const;
    void setEmailAddress(const QString emailAddress);

    QString imapServerUserName() const;
    void setImapServerUserName(const QString imapServerUserName);

    QString imapServerPassword() const;
    void setImapServerPassword(const QString imapServerPassword);

    QString imapServerAddress() const;
    void setImapServerAddress(const QString imapServerAddress);

    int imapServerPort() const;
    void setImapServerPort(const int imapServerPort);

    bool isImapServerConnectionEncrypted() const;
    void setIsImapServerConnectionEncrypted(const bool isImapServerConnectionEncrypted);

    QString smtpServerUserName() const;
    void setSmtpServerUserName(const QString smtpServerUserName);

    QString smtpServerPassword() const;
    void setSmtpServerPassword(const QString smtpServerPassword);

    QString smtpServerAddress() const;
    void setSmtpServerAddress(const QString smtpServerAddress);

    int smtpServerPort() const;
    void setSmtpServerPort(const int smtpServerPort);

    bool isSmtpServerConnectionEncrypted() const;
    void setIsSmtpServerConnectionEncrypted(const bool isSmtpServerConnectionEncrypted);

    void fetchStoreContent();
    bool sendMessage(Message message) const;
    void fetchMessageContent(const QString folderPath, const int positionInFolder);

    void deleteMessage(const int uid, const int folderId);
    void copyMessage(const int uid, const QString oldPath, const QString newPath);
    void moveMessage(const int uid, const QString oldPath, const QString newPath);

    void renameFolder(const QString originalFolderPath, const QString renamedFolderPath);
    void deleteFolder(const QString folderPath);
    void createFolder(const QString folderPath);

    void setMessageAsSeen(const int uid);
    void setMessageAsUnseen(const int uid);

private:
    void fetchMessagesMetadata();
    void updateFolderStructureInDatabase(QList<InboxFolder> folders);
    void updateMessageContentInDatabase(const QString emailAddress, const QString folderPath, const int positionInFolder, MessageContent messageContent);

    QFuture<QList<InboxFolder>> fetchInboxFoldersThread();
    QFuture<MessageContent> fetchMessageContentThread(QString folderPath, int positionInFolder);
    QFuture<QList<MessageMetadata>> fetchMessagesMetadataThread();
    QFuture<bool> deleteMessageThread(const int uid, const QString folderPath);
    QFuture<bool> renameFolderThread(const QString originalFolderPath, const QString renamedFolderPath);
    QFuture<bool> deleteFolderThread(const QString folderPath);
    QFuture<bool> createFolderThread(const QString folderPath);
    QFuture<long> copyMessageThread(const int uid, const QString oldPath, const QString newPath);
    QFuture<long> moveMessageThread(const int uid, const QString oldPath, const QString newPath);
    QFuture<bool> setMessageAsSeenThread(const int uid, const QString folderPath);
    QFuture<bool> setMessageAsUnseenThread(const int uid, const QString folderPath);

    connectionSettingsHolder getConnectionSettings() const;

    QString m_contactName;
    QString m_emailAddress;

    QString m_imapServerUserName;
    QString m_imapServerPassword;
    QString m_imapServerAddress;
    int m_imapServerPort;
    bool m_isImapServerConnectionEncrypted;

    QString m_smtpServerUserName;
    QString m_smtpServerPassword;
    QString m_smtpServerAddress;
    int m_smtpServerPort;
    bool m_isSmtpServerConnectionEncrypted;

signals:
    void messageContentFetched(int);
    void newMessagesReceived(QString, int);

public slots:
};

QDebug operator<<(QDebug debug, const EmailAccount &account);

}

#endif // EMAILACCOUNT_H
