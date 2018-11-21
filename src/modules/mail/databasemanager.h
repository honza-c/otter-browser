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
    explicit DatabaseManager(QString databaseFileName, QObject *parent = nullptr);
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
    static void updateBlindCopyRecipients(const int messageId, const QList<Contact> blindCopyRecipients);
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

    static QString getArchiveFolderPathForAccount(QString emailAddress);
    void clearDatabase();
signals:

public slots:

private:
    bool checkDatabaseStructure();
    bool checkTableStructure(QString tableName);
    bool checkTableStructure(QSqlRecord tableRecord, QStringList tableFields);
    void initializeDatabaseStructure();

    QSqlDatabase m_database;
    QString m_databaseFileName;
    QStringList m_tableNames;
    QStringList m_mailFolderTableFields;
    QStringList m_messageDataTableFields;
};

}

#endif // DATABASEMANAGER_H
