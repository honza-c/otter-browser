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

#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QObject>
#include <QSqlDatabase>
#include <QDebug>
#include <QFile>
#include <QSqlRecord>
#include <QSqlField>
#include <QSqlQuery>
#include <QSqlError>
#include <QFuture>
#include <QtConcurrent/QtConcurrent>
#include <QFutureWatcher>
#include "attachment.h"
#include "messagemetadata.h"
#include "inboxfolder.h"
#include "contact.h"
#include "embeddedobject.h"

namespace Otter
{

class DatabaseManager : public QObject
{
    Q_OBJECT
public:
    static void createInstance();
    static DatabaseManager *getInstance();

    static int getFolderId(const QString emailAddress, const QString folderPath);
    static QString getHtmlContent(const int messageId);
    static QString getTextContent(const int messageId);
    static int getPositionInFolder(const int messageId);
    static int getMessageId(const int folderId, const int positionInFolder);
    static void updateHtmlContent(const int messageId, const QString htmlContent);
    static void updatePlainTextContent(const int messageId, const QString plainTextContent);
    static void updateAttachmentsContent(const int messageId, const QList<Attachment> attachments);
    static void updateEmbeddedObjectsContent(const int messageId, const QList<EmbeddedObject> embeddedObjects);
    static void updateRecipients(const int messageId, const QList<Contact> recipients);
    static void updateCopyRecipients(const int messageId, const QList<Contact> copyRecipients);
    static QList<Attachment> getAttachments(const int messageId);
    static QList<EmbeddedObject> getEmbeddedObjects(const int messageId);
    static QStringList getFoldersPathForAccount(const QString emailAddress);
    static void deleteFolderFromDatabase(const QString emailAddress, const QString path);
    static void addFolderToDatabase(const InboxFolder folder);
    static int getMessagesCountForFolder(const QString emailAddress, const QString path);
    static void addMessagesMetadataToDatabase(const QList<MessageMetadata> metadata);
    static void cleanUnusedDataFromDatabase(const QStringList emailAddresses);
    static QString getSubject(const int messageId);
    static Contact getSender(const int messageId);
    static QDateTime getDate(const int messageId);
    static QList<Contact> getRecipients(const int messageId);
    static QList<Contact> getCopyRecipients(const int messageId);
    static QList<Contact> getReplyTo(const int messageId);
    static QList<InboxFolder> getInboxFolders();
    static QList<InboxFolder> getInboxFolders(QString emailAddress);
    static int getTotalCountOfUnreadMessages();
    static int getCountOfUnreadMessagesForFolder(QString emailAddress, QString path);
    static QStringList getDataForContactsCompleter();

    static bool isFolderJunk(int folderId);
    static bool isFolderTrash(int folderId);
    static bool isFolderArchive(int folderId);

    static QString getFolderPath(int folderId);
    static QString getEmailAddress(int folderId);

    static void updateMessageMetadata(const QList<MessageMetadata> freshMetadataFromServer, const QString emailAddress);
    static QList<MessageMetadata> getMessageMetadataForAccount(const QString emailAddress);
    static void setMessageAsSeen(const unsigned long uid, const QString emailAddress);
    static void deleteMessageFromDatabase(const unsigned long uid, const QString emailAddress);

    static QString getArchiveFolderPathForAccount(QString emailAddress);
    static void clearDatabase();

    static QFuture<QList<unsigned long>> getUidsOfSeenMessagesOnServer(const QList<MessageMetadata> messagesFromServer, const QList<MessageMetadata> messagesFromDatabase);
    static QFuture<QList<unsigned long>> getUidsOfMessagesDeletedFromServer(const QList<MessageMetadata> messagesFromServer, const QList<MessageMetadata> messagesFromDatabase);
    static QFuture<QList<MessageMetadata>> getMissingMessagesFromServer(const QList<MessageMetadata> messagesFromServer, const QList<MessageMetadata> messagesFromDatabase);

    static void renameFolder(const QString emailAddress, const QString originalFolderPath, const QString renamedFolderPath);

    static void copyMessage(const QString emailAddress, const QString oldPath, const QString newPath, const unsigned long oldUid, const unsigned long newUid);
    static void moveMessage(const QString emailAddress, const QString oldPath, const QString newPath, const unsigned long oldUid, const long unsigned newUid);

    static bool hasTheAccountTrashFolder(const QString emailAddress);
    static bool hasTheAccountJunkFolder(const QString emailAddress);
    static bool hasTheAccountArchiveFolder(const QString emailAddress);
    static QString getTrashFolderPath(const QString emailAddress);
    static QString getJunkFolderPath(const QString emailAddress);
    static QString getArchiveFolderPath(const QString emailAddress);

signals:
    void inboxFoldersStructureChanged();
    void messagesMetadataStructureChanged();

public slots:

private:
    explicit DatabaseManager(QObject *parent = nullptr);

    static bool checkDatabaseStructure();
    static bool checkTableStructure(QString tableName);
    static bool checkTableStructure(QSqlRecord tableRecord, QStringList tableFields);
    static void initializeDatabaseStructure();

    static DatabaseManager *m_instance;

    static QSqlDatabase m_database;
    static QString m_databaseFileName;
    static QStringList m_tableNames;
    static QStringList m_mailFolderTableFields;
    static QStringList m_messageDataTableFields;
};

}

#endif // DATABASEMANAGER_H
