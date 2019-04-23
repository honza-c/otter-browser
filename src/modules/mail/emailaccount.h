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
        QString userName;
        QString emailAddress;
        QString password;
        QString smtpServerUrl;
        int smtpServerPort;
        QString imapServerAddress;
        int imapServerPort;
    };

    QString contactName() const;
    void setContactName(const QString contactName);

    QString emailAddress() const;
    void setEmailAddress(const QString emailAddress);

    QString userName() const;
    void setUserName(const QString userName);

    QString password() const;
    void setPassword(const QString password);

    QString smtpServerUrl() const;
    void setSmtpServerUrl(const QString smtpServerUrl);

    int smtpServerPort() const;
    void setSmtpServerPort(const int smtpServerPort);

    QString imapServerAddress() const;
    void setImapServerAddress(const QString imapServerAddress);

    int imapServerPort() const;
    void setImapServerPort(const int imapServerPort);

    void fetchStoreContent();
    QFuture<QList<InboxFolder>> fetchInboxFolders();

    void initializeInbox();
    void sendMessage(Message message) const;
    QFuture<MessageContent> fetchMessageContent(QString folderPath, int positionInFolder);
    void fetchMissingMessageContent(const QString folderPath, const int positionInFolder);
    void fetchMessageMetadata();
    void updateMessageMetadataInDatabase(QList<MessageMetadata> metadata);

    QFuture<bool> moveMessageThread(const QString sourceFolderPath, const int messageId, const QString destinationFolderPath);
    void moveMessage(const QString sourceFolderPath, const int messageId, const QString destinationFolderPath);

    void deleteMessage(const int uid, const int folderId);

    void renameFolder(const QString originalFolderPath, const QString renamedFolderPath);
    void deleteFolder(const QString folderPath);
    void createFolder(const QString folderPath);
    void copyMessage(const int uid, const QString oldPath, const QString newPath);

private:
    QFuture<QList<MessageMetadata>> fetchMessagesMetadata();
    void updateFolderStructureInDatabase(QList<InboxFolder> folders);
    void updateMessageContentInDatabase(const QString emailAddress, const QString folderPath, const int positionInFolder, MessageContent messageContent);

    QFuture<void> deleteMessageThread(const int uid, const QString folderPath);
    QFuture<void> renameFolderThread(const QString originalFolderPath, const QString renamedFolderPath);
    QFuture<void> deleteFolderThread(const QString folderPath);
    QFuture<void> createFolderThread(const QString folderPath);
    QFuture<long> copyMessageThread(const int uid, const QString oldPath, const QString newPath);

    connectionSettingsHolder getConnectionSettings() const;

    QString m_contactName;
    QString m_userName;
    QString m_emailAddress;
    QString m_password;

    QString m_smtpServerUrl;
    int m_smtpServerPort;

    QString m_imapServerAddress;
    int m_imapServerPort;

    VmimeImapService *m_imapService;

signals:
    void messageContentFetched(int);
    void newMessagesReceived(QString, int);

public slots:
};

QDebug operator<<(QDebug debug, const EmailAccount &account);

}

#endif // EMAILACCOUNT_H
