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

#include "databasemanager.h"

namespace Otter
{

DatabaseManager::DatabaseManager(QString databaseFileName, QObject *parent)
    : QObject(parent)
{
    m_databaseFileName = databaseFileName;

    m_tableNames << "Folders" << "MessageData";

    m_mailFolderTableFields << "id" << "emailAddress" << "path" << "isAllMessages" << "isArchive"
                            << "isDrafts" << "isJunk" << "isSent" << "isTrash";

    m_messageDataTableFields << "id" << "folderId" << "uid" << "isSeen" << "isDraft"
                              << "date" << "sender" << "size" << "subject"
                              << "recipients" << "copyRecipients" << "plainTextContent" << "htmlContent"
                              << "attachments" << "embeddedObjects" << "replyToRecipients";

    if (!checkDatabaseStructure())
    {
        initializeDatabaseStructure();
    }
}


void DatabaseManager::initializeDatabaseStructure()
{
    if (m_database.isOpen())
    {
        m_database.close();
    }

    if (QFile::exists(m_databaseFileName))
    {
        QFile::remove(m_databaseFileName);
    }

    m_database = QSqlDatabase::addDatabase("QSQLITE");
    m_database.setDatabaseName(m_databaseFileName);
    m_database.open();

    QSqlQuery msgFoldersQuery;
    msgFoldersQuery.exec("create table Folders "
                         "(id integer primary key, "
                         "emailAddress text,"
                         "path text,"
                         "isAllMessages integer,"
                         "isArchive integer,"
                         "isDrafts integer,"
                         "isJunk integer,"
                         "isSent integer,"
                         "isTrash integer)");

    QSqlQuery msgDataQuery;
    msgDataQuery.exec("create table MessageData "
                      "(id integer primary key, "
                      "folderId integer, "
                      "uid integer, "
                      "isSeen integer, "
                      "isDraft integer, "
                      "date text, "
                      "sender text, "
                      "size integer, "
                      "subject text, "
                      "recipients text, "
                      "copyRecipients text, "
                      "plainTextContent text, "
                      "htmlContent text, "
                      "attachments blob, "
                      "embeddedObjects blob, "
                      "replyToRecipients text)");

}

bool DatabaseManager::checkDatabaseStructure()
{
    if (!QFile::exists(m_databaseFileName))
    {
        return false;
    }

    m_database = QSqlDatabase::addDatabase("QSQLITE");
    m_database.setDatabaseName(m_databaseFileName);
    m_database.open();

    QStringList tablesFromDatabase = m_database.tables();

    for (QString table : m_tableNames)
    {
        if (!tablesFromDatabase.contains(table))
        {
            qWarning() << "Table " << table << " is not in the database";
            m_database.close();
            return false;
        }
    }

    for (QString table : tablesFromDatabase)
    {
        if (!checkTableStructure(table))
        {
            qWarning() << " Table " << table << " has not valid structure";
            m_database.close();
            return false;
        }
    }

    return true;
}

bool DatabaseManager::checkTableStructure(QString tableName)
{
    QSqlRecord tableRecord = m_database.record(tableName);

    if (tableName == "Folders")
    {
        return  checkTableStructure(tableRecord, m_mailFolderTableFields);
    }
    else if (tableName == "MessageData")
    {
        return checkTableStructure(tableRecord, m_messageDataTableFields);
    }

    qWarning() << tableName << " is not a name of table according to DB structure";

    return false;
}

bool DatabaseManager::checkTableStructure(QSqlRecord tableRecord, QStringList tableFields)
{
    int columnCount = tableRecord.count();

    if (columnCount != tableFields.size())
    {
        qWarning() << "column count doesnt match";
        return false;
    }

    for (QString fieldName : tableFields)
    {
        if (!tableFields.contains(fieldName))
        {
            qWarning() << "column names doesnt match";
            return false;
        }
    }

    return true;
}

int DatabaseManager::getFolderId(const QString emailAddress, const QString folderPath)
{
    int folderId = -1;

    QSqlQuery query;

    query.prepare("SELECT id FROM Folders "
                  "WHERE emailAddress = :emailAddress AND path = :path");

    query.bindValue(":emailAddress", emailAddress);
    query.bindValue(":path", folderPath);
    query.exec();

    while (query.next())
    {
        folderId = query.value(0).toInt();
    }


    return folderId;
}

QString DatabaseManager::getHtmlContent(const int messageId)
{
    QString content;

    QSqlQuery query;

    query.prepare("SELECT htmlContent FROM MessageData "
                  "WHERE id = :messageId");

    query.bindValue(":messageId", messageId);
    query.exec();

    while (query.next())
    {
        content = query.value(0).toString();
    }

    return content;
}

QString DatabaseManager::getTextContent(const int messageId)
{
    QString content;

    QSqlQuery query;

    query.prepare("SELECT plainTextContent FROM MessageData "
                  "WHERE id = :messageId");

    query.bindValue(":messageId", messageId);
    query.exec();

    while (query.next())
    {
        content = query.value(0).toString();
    }

    return content;
}

QList<Contact> DatabaseManager::getRecipients(const int messageId)
{
    QList<Contact> recipients;

    QSqlQuery recipientsQuery;

    recipientsQuery.prepare("SELECT recipients FROM MessageData "
                            "WHERE id = :messageId");

    recipientsQuery.bindValue(":messageId", messageId);
    recipientsQuery.exec();

    while (recipientsQuery.next())
    {
        recipients = Contact::contactsFromJson(recipientsQuery.value(0).toString());
    }

    return recipients;
}

QList<Contact> DatabaseManager::getCopyRecipients(const int messageId)
{
    QList<Contact> copyRecipients;

    QSqlQuery recipientsQuery;

    recipientsQuery.prepare("SELECT copyRecipients FROM MessageData "
                            "WHERE id = :messageId");

    recipientsQuery.bindValue(":messageId", messageId);
    recipientsQuery.exec();

    while (recipientsQuery.next())
    {
        copyRecipients = Contact::contactsFromJson(recipientsQuery.value(0).toString());
    }

    return copyRecipients;
}

QList<Contact> DatabaseManager::getReplyTo(const int messageId)
{
    QList<Contact> replyToRecipients;

    QSqlQuery query;

    query.prepare("SELECT replyToRecipients FROM MessageData "
                            "WHERE id = :messageId");

    query.bindValue(":messageId", messageId);
    query.exec();

    while (query.next())
    {
        replyToRecipients = Contact::contactsFromJson(query.value(0).toString());
    }

    return replyToRecipients;
}

int DatabaseManager::getPositionInFolder(const int messageId)
{
    int uid = 0;

    QSqlQuery query;

    query.prepare("SELECT uid FROM MessageData "
                  "WHERE id = :messageId");

    query.bindValue(":messageId", messageId);
    query.exec();

    while (query.next())
    {
        uid = query.value(0).toInt();
    }

    return uid;
}

int DatabaseManager::getMessageId(const int folderId, const int uid)
{
    int messageId = 0;

    QSqlQuery query;

    query.prepare("SELECT id FROM MessageData "
                  "WHERE folderId = :folderId AND uid = :uid");

    query.bindValue(":folderId", folderId);
    query.bindValue(":uid", uid);
    query.exec();

    while (query.next())
    {
        messageId = query.value(0).toInt();
    }

    return messageId;
}

void DatabaseManager::updateHtmlContent(const int messageId, const QString htmlContent)
{
    QSqlQuery query;

    query.prepare("UPDATE MessageData "
                  "SET htmlContent = :htmlContent "
                  "WHERE id = :messageId");

    query.bindValue(":htmlContent", htmlContent);
    query.bindValue(":messageId", messageId);

    query.exec();
}

void DatabaseManager::updatePlainTextContent(const int messageId, const QString plainTextContent)
{
    QSqlQuery query;

    query.prepare("UPDATE MessageData "
                  "SET plainTextContent = :plainTextContent "
                  "WHERE id = :messageId");

    query.bindValue(":plainTextContent", plainTextContent);
    query.bindValue(":messageId", messageId);

    query.exec();
}

void DatabaseManager::updateRecipients(const int messageId, const QList<Contact> recipients)
{   
    QString jsonString = Contact::toJson(recipients);
    QSqlQuery query;

    query.prepare("UPDATE MessageData "
                  "SET recipients = :json "
                  "WHERE id = :messageId");

    query.bindValue(":json", jsonString);
    query.bindValue(":messageId", messageId);

    query.exec();
}

void DatabaseManager::updateCopyRecipients(const int messageId, const QList<Contact> copyRecipients)
{   
    QString jsonString = Contact::toJson(copyRecipients);
    QSqlQuery query;

    query.prepare("UPDATE MessageData "
                  "SET copyRecipients = :json "
                  "WHERE id = :messageId");

    query.bindValue(":json", jsonString);
    query.bindValue(":messageId", messageId);

    query.exec();
}

QString DatabaseManager::getSubject(const int messageId)
{
    QString subject;

    QSqlQuery query;

    query.prepare("SELECT subject FROM MessageData "
                  "WHERE id = :messageId");

    query.bindValue(":messageId", messageId);
    query.exec();

    while (query.next())
    {
        subject = query.value(0).toString();
    }

    return subject;
}

Contact DatabaseManager::getSender(const int messageId)
{
    Contact sender;
    QSqlQuery query;

    query.prepare("SELECT sender FROM MessageData "
                  "WHERE id = :messageId");

    query.bindValue(":messageId", messageId);
    query.exec();

    while (query.next())
    {
        sender = Contact::contactFromJson(query.value(0).toString());
    }

    return sender;
}

QDateTime DatabaseManager::getDate(const int messageId)
{
    long dateTime = 0;

    QSqlQuery query;

    query.prepare("SELECT date FROM MessageData "
                  "WHERE id = :messageId");

    query.bindValue(":messageId", messageId);
    query.exec();

    while (query.next())
    {
        dateTime = query.value(0).toLongLong();
    }

    return QDateTime::fromSecsSinceEpoch(dateTime);
}

void DatabaseManager::updateAttachmentsContent(const int messageId, const QList<Attachment> attachments)
{
    QByteArray byteArray;
    QDataStream dataStream(&byteArray, QIODevice::WriteOnly);

    for (Attachment attachment : attachments)
    {
        dataStream << attachment;
    }

    QSqlQuery query;

    query.prepare("UPDATE MessageData "
                  "SET attachments = :byteArray "
                  "WHERE id = :messageId");

    query.bindValue(":byteArray", byteArray);
    query.bindValue(":messageId", messageId);

    query.exec();
}

void DatabaseManager::updateEmbeddedObjectsContent(const int messageId, const QList<EmbeddedObject> embeddedObjects)
{
    QByteArray byteArray;
    QDataStream dataStream(&byteArray, QIODevice::WriteOnly);

    for (EmbeddedObject embeddedObject : embeddedObjects)
    {
        dataStream << embeddedObject;
    }

    QSqlQuery query;

    query.prepare("UPDATE MessageData "
                  "SET embeddedObjects = :byteArray "
                  "WHERE id = :messageId");

    query.bindValue(":byteArray", byteArray);
    query.bindValue(":messageId", messageId);

    query.exec();
}

QList<Attachment> DatabaseManager::getAttachments(const int messageId)
{
    QList<Attachment> attachments;

    QSqlQuery query;

    query.prepare("SELECT attachments FROM MessageData "
                  "WHERE id = :messageId");

    query.bindValue(":messageId", messageId);

    query.exec();

    while (query.next())
    {
        QByteArray byteArray = query.value(0).toByteArray();
        QDataStream dataStream(byteArray);

        while (!dataStream.atEnd())
        {
            Attachment attachment;
            dataStream >> attachment;
            attachments.append(attachment);
        }
    }

    return attachments;
}

QList<EmbeddedObject> DatabaseManager::getEmbeddedObjects(const int messageId)
{
    QList<EmbeddedObject> embeddedObjects;

    QSqlQuery query;

    query.prepare("SELECT embeddedObjects FROM MessageData "
                  "WHERE id = :messageId");

    query.bindValue(":messageId", messageId);

    query.exec();

    while (query.next())
    {
        QByteArray byteArray = query.value(0).toByteArray();
        QDataStream dataStream(byteArray);

        while (!dataStream.atEnd())
        {
            EmbeddedObject embeddedObject;
            dataStream >> embeddedObject;
            embeddedObjects.append(embeddedObject);
        }
    }

    return embeddedObjects;
}

void DatabaseManager::clearDatabase()
{
    initializeDatabaseStructure();
}

QStringList DatabaseManager::getFoldersPathForAccount(const QString emailAddress)
{
    QStringList result;
    QSqlQuery query;

    query.prepare("SELECT path FROM Folders"
                  " WHERE "
                  "emailAddress = :emailAddress");

    query.bindValue(":emailAddress", emailAddress);
    query.exec();

    while (query.next())
    {
        result << query.value(0).toString();
    }

    return result;
}

void DatabaseManager::deleteFolderFromDatabase(const QString emailAddress, const QString path)
{
    int folderId = DatabaseManager::getFolderId(emailAddress, path);

    QSqlQuery deleteMessagesQuery;

    deleteMessagesQuery.prepare("DELETE FROM MessageData"
                                " WHERE "
                                "folderId = :folderId");

    deleteMessagesQuery.bindValue(":folderId", folderId);

    deleteMessagesQuery.exec();

    QSqlQuery deleteFolderQuery;

    deleteFolderQuery.prepare("DELETE FROM Folders"
                              " WHERE "
                              "emailAddress = :emailAddress AND path = :path");

    deleteFolderQuery.bindValue(":emailAddress", emailAddress);
    deleteFolderQuery.bindValue(":path", path);

    deleteFolderQuery.exec();
}

void DatabaseManager::addFolderToDatabase(const InboxFolder folder)
{
    QSqlQuery query;

    query.prepare("INSERT INTO Folders "
                  "(emailAddress, path, isAllMessages, isArchive, isDrafts, isJunk, isSent, isTrash)"
                  " VALUES "
                  "(:emailAddress, :path, :isAllMessages, :isArchive, :isDrafts, :isJunk, :isSent, :isTrash)");

    query.bindValue(":emailAddress", folder.emailAddress());
    query.bindValue(":path", folder.path());
    query.bindValue(":isAllMessages", folder.isAllMessages());
    query.bindValue(":isArchive", folder.isArchive());
    query.bindValue(":isDrafts", folder.isDrafts());
    query.bindValue(":isJunk", folder.isJunk());
    query.bindValue(":isSent", folder.isSent());
    query.bindValue(":isTrash", folder.isTrash());

    query.exec();
}

int DatabaseManager::getMessagesCountForFolder(const QString emailAddress, const QString path)
{
    int folderId = getFolderId(emailAddress, path);

    QSqlQuery query;

    query.prepare("SELECT"
                  " COUNT( * ) "
                  "FROM MessageData"
                  " WHERE "
                  "folderId = :folderId");

    query.bindValue(":folderId", folderId);

    query.exec();

    if (query.next())
    {
        return query.value(0).toInt();
    }
    else
    {
        return -1;
    }
}

int DatabaseManager::getTotalCountOfUnreadMessages()
{
    QSqlQuery query;

    query.prepare("SELECT"
                  " COUNT( * ) "
                  "FROM MessageData"
                  " WHERE "
                  "isSeen = :isSeen");

    query.bindValue(":isSeen", 0);
    query.exec();

    if (query.next())
    {
        return query.value(0).toInt();
    }
    else
    {
        return -1;
    }
}

int DatabaseManager::getCountOfUnreadMessagesForFolder(QString emailAddress, QString path)
{
    int folderId = getFolderId(emailAddress, path);

    QSqlQuery query;

    query.prepare("SELECT"
                  " COUNT( * ) "
                  "FROM MessageData"
                  " WHERE "
                  "isSeen = :isSeen"
                  " AND "
                  "folderId = :folderId");

    query.bindValue(":isSeen", 0);
    query.bindValue(":folderId", folderId);
    query.exec();

    if (query.next())
    {
        return query.value(0).toInt();
    }
    else
    {
        return -1;
    }
}

void DatabaseManager::addMessagesMetadataToDatabase(const QList<MessageMetadata> metadata)
{
        QSqlQuery query;

        query.prepare("INSERT INTO MessageData "
                      "(folderId, uid, isSeen, isDraft, date, sender, size, subject, replyToRecipients)"
                      " VALUES "
                      "(:folderId, :uid, :isSeen, :isDraft, :date, :sender, :size, :subject, :replyToRecipients)");

        QVariantList folderIdInDb, uid, isSeen, isDraft, date, sender, size, subject, replyToRecipients;

        for (MessageMetadata data : metadata)
        {
            int folderId = getFolderId(data.emailAddress(), data.folderPath());

            if (folderId > 0)
            {
                folderIdInDb << folderId;
                uid << static_cast<int>(data.uid());
                isSeen << data.isSeen();
                isDraft << data.isDraft();
                date << data.dateTime().toSecsSinceEpoch();
                sender << Contact::toJson(data.from());
                size << static_cast<int>(data.size());
                subject << data.subject();
                replyToRecipients << Contact::toJson(data.replyTo());
            }
        }

        query.bindValue(":folderId", folderIdInDb);
        query.bindValue(":uid", uid);
        query.bindValue(":isSeen", isSeen);
        query.bindValue(":isDraft", isDraft);
        query.bindValue(":date", date);
        query.bindValue(":sender", sender);
        query.bindValue(":size", size);
        query.bindValue(":subject", subject);
        query.bindValue(":replyToRecipients", replyToRecipients);

        query.execBatch();
}

void DatabaseManager::cleanUnusedDataFromDatabase(const QStringList emailAddresses)
{
    // 1) getting FolderIDs from database, which do not belong to any user account in application
    QList<int> unusedFolderIds;

    QSqlQuery query;

    query.prepare("SELECT * FROM Folders");
    query.exec();

    while (query.next())
    {
        bool found = false;

        for (int i = 0; i < emailAddresses.size(); i++)
        {
            if (query.value(1).toString() == emailAddresses.at(i))
            {
                found = true;
                break;
            }
        }

        if (!found)
        {
            unusedFolderIds << query.value(0).toInt();
        }
    }

    // 2) removing messages data which belong to folder ids that are not associated with any user account in application

    for (int folderId : unusedFolderIds)
    {
        QSqlQuery messagesDataQuery;
        messagesDataQuery.prepare("DELETE FROM MessageData"
                                  " WHERE "
                                  "folderId = :folderId");
        messagesDataQuery.bindValue(":folderId", folderId);
        messagesDataQuery.exec();

        QSqlQuery foldersQuery;
        foldersQuery.prepare("DELETE FROM Folders"
                             " WHERE "
                             "id = :folderId");
        foldersQuery.bindValue(":folderId", folderId);
        foldersQuery.exec();
    }
}

QList<InboxFolder> DatabaseManager::getInboxFolders()
{
    QList<InboxFolder> result;

    QSqlQuery query;

    query.exec("SELECT"
                  " * "
                  "FROM Folders"
                  " ORDER BY"
                  " emailAddress ASC,"
                  " path ASC");

    while (query.next())
    {
        InboxFolder folder;

        folder.setEmailAddress(query.value(1).toString());
        folder.setPath(query.value(2).toString());
        folder.setIsAllMessages(query.value(3).toBool());
        folder.setIsArchive(query.value(4).toBool());
        folder.setIsDrafts(query.value(5).toBool());
        folder.setIsImportant(query.value(6).toBool());
        folder.setIsJunk(query.value(7).toBool());
        folder.setIsSent(query.value(8).toBool());
        folder.setIsTrash(query.value(9).toBool());

        result << folder;
    }

    return result;
}

QList<InboxFolder> DatabaseManager::getInboxFolders(QString emailAddress)
{
    QList<InboxFolder> result;

    QSqlQuery query;

    query.prepare("SELECT"
                  " * "
                  "FROM Folders"
                  " WHERE "
                  " emailAddress = :emailAddress "
                  " ORDER BY"
                  " path ASC");

    query.bindValue(":emailAddress", emailAddress);
    query.exec();

    while (query.next())
    {
        InboxFolder folder;

        folder.setEmailAddress(query.value(1).toString());
        folder.setPath(query.value(2).toString());
        folder.setIsAllMessages(query.value(3).toBool());
        folder.setIsArchive(query.value(4).toBool());
        folder.setIsDrafts(query.value(5).toBool());
        folder.setIsImportant(query.value(6).toBool());
        folder.setIsJunk(query.value(7).toBool());
        folder.setIsSent(query.value(8).toBool());
        folder.setIsTrash(query.value(9).toBool());

        result << folder;
    }

    return result;
}

QStringList DatabaseManager::getDataForContactsCompleter()
{
    QSet<QString> contacts;

    QSqlQuery senderQuery;
    senderQuery.prepare("SELECT sender FROM MessageData");
    senderQuery.exec();

    while (senderQuery.next())
    {
        Contact sender = Contact::contactFromJson(senderQuery.value(0).toString());
        contacts << Contact::toString(sender);
    }

    QSqlQuery recipientsQuery;
    recipientsQuery.prepare("SELECT recipients FROM MessageData");
    recipientsQuery.exec();

    while (recipientsQuery.next())
    {
        Contact contact = Contact::contactFromJson(recipientsQuery.value(0).toString());
        contacts << Contact::toString(contact);
    }

    QSqlQuery copyRecipientsQuery;
    copyRecipientsQuery.prepare("SELECT copyRecipients FROM MessageData");
    copyRecipientsQuery.exec();

    while (copyRecipientsQuery.next())
    {
        Contact contact = Contact::contactFromJson(copyRecipientsQuery.value(0).toString());
        contacts << Contact::toString(contact);
    }

    QSqlQuery replyToRecipientsQuery;
    replyToRecipientsQuery.prepare("SELECT replyToRecipients FROM MessageData");
    replyToRecipientsQuery.exec();

    while (replyToRecipientsQuery.next())
    {
        Contact contact = Contact::contactFromJson(replyToRecipientsQuery.value(0).toString());
        contacts << Contact::toString(contact);
    }

    return contacts.toList();
}

QString DatabaseManager::getFolderPath(int folderId)
{
    QString folderPath;

    QSqlQuery query;

    query.prepare("SELECT path FROM Folders "
                  "WHERE id = :id");

    query.bindValue(":id", folderId);
    query.exec();

    while (query.next())
    {
        folderPath = query.value(0).toString();
    }

    return folderPath;
}

QString DatabaseManager::getEmailAddress(int folderId)
{
    QString emailAddress;

    QSqlQuery query;

    query.prepare("SELECT emailAddress FROM Folders "
                  "WHERE id = :id");

    query.bindValue(":id", folderId);
    query.exec();

    while (query.next())
    {
        emailAddress = query.value(0).toString();
    }

    return emailAddress;
}

bool DatabaseManager::isFolderJunk(int folderId)
{
    bool isJunk = false;

    QSqlQuery query;

    query.prepare("SELECT isJunk FROM Folders "
                  "WHERE id = :id");

    query.bindValue(":id", folderId);
    query.exec();

    while (query.next())
    {
        isJunk = query.value(0).toBool();
    }

    return isJunk;
}

bool DatabaseManager::isFolderTrash(int folderId)
{
    bool isTrash = false;

    QSqlQuery query;

    query.prepare("SELECT isTrash FROM Folders "
                  "WHERE id = :id");

    query.bindValue(":id", folderId);
    query.exec();

    while (query.next())
    {
        isTrash = query.value(0).toBool();
    }

    return isTrash;
}

bool DatabaseManager::isFolderArchive(int folderId)
{
    bool isArchive = false;

    QSqlQuery query;

    query.prepare("SELECT isArchive FROM Folders "
                  "WHERE id = :id");

    query.bindValue(":id", folderId);
    query.exec();

    while (query.next())
    {
        isArchive = query.value(0).toBool();
    }

    return isArchive;
}

QString DatabaseManager::getArchiveFolderPathForAccount(QString emailAddress)
{
    QString path;

    QSqlQuery query;

    query.prepare("SELECT path FROM Folders "
                  "WHERE emailAddress = :emailAddress AND isArchive = :isArchive");

    query.bindValue(":emailAddress", emailAddress);
    query.bindValue(":isArchive", 1);

    query.exec();

    while (query.next())
    {
        path = query.value(0).toString();
    }

    return path;
}

QFuture<QList<unsigned long>> DatabaseManager::getUidsOfSeenMessagesOnServer(const QList<MessageMetadata> messagesFromServer, const QList<MessageMetadata> messagesFromDatabase)
{
    auto getUidsOfSeenMessagesOnServerWorker = []
            (const QList<MessageMetadata> messagesFromServer,
             const QList<MessageMetadata> messagesFromDatabase)
    {
        QList<unsigned long> uidsOfUnseenMsgsFromDatabase;
        QList<unsigned long> uidsOfSeenMsgsSinceLastUpdate;

        for (MessageMetadata message : messagesFromDatabase)
        {
            if (!message.isSeen())
            {
                uidsOfUnseenMsgsFromDatabase << message.uid();
            }
        }

        for (unsigned long uid : uidsOfUnseenMsgsFromDatabase)
        {
            for (MessageMetadata message : messagesFromServer)
            {
                if (message.uid() == uid)
                {
                    if (message.isSeen())
                    {
                        uidsOfSeenMsgsSinceLastUpdate << uid;
                    }
                    break;
                }
            }
        }

        return uidsOfSeenMsgsSinceLastUpdate;
    };

    return QtConcurrent::run(getUidsOfSeenMessagesOnServerWorker, messagesFromServer, messagesFromDatabase);
}

QFuture<QList<unsigned long>> DatabaseManager::getUidsOfMessagesDeletedFromServer(const QList<MessageMetadata> messagesFromServer, const QList<MessageMetadata> messagesFromDatabase)
{
    auto getUidsOfMessagesDeletedFromServerWorker = []
            (const QList<MessageMetadata> messagesFromServer,
             const QList<MessageMetadata> messagesFromDatabase)
    {
        QList<unsigned long> uidsOfDeletedMessages;

        for (MessageMetadata messageFromDatabase : messagesFromDatabase)
        {
            bool found = false;

            for (MessageMetadata messageFromServer : messagesFromServer)
            {
                if (messageFromServer.uid() == messageFromDatabase.uid())
                {
                    found = true;
                    break;
                }
            }

            if (!found)
            {
                uidsOfDeletedMessages << messageFromDatabase.uid();
            }
        }

        return uidsOfDeletedMessages;
    };

    return QtConcurrent::run(getUidsOfMessagesDeletedFromServerWorker, messagesFromServer, messagesFromDatabase);
}

QFuture<QList<MessageMetadata>> DatabaseManager::getMissingMessagesFromServer(const QList<MessageMetadata> messagesFromServer, const QList<MessageMetadata> messagesFromDatabase)
{
    auto getMissingMessagesFromServerWorker = []
            (const QList<MessageMetadata> messagesFromServer,
             const QList<MessageMetadata> messagesFromDatabase)
    {
        QList<MessageMetadata> newMessages;

        for (MessageMetadata messageFromServer : messagesFromServer)
        {
            bool found = false;

            for (MessageMetadata messageFromDatabase : messagesFromDatabase)
            {
                if (messageFromServer.uid() == messageFromDatabase.uid())
                {
                    found = true;
                    break;
                }
            }

            if (!found)
            {
                newMessages << messageFromServer;
            }
        }

        return newMessages;
    };

    return QtConcurrent::run(getMissingMessagesFromServerWorker, messagesFromServer, messagesFromDatabase);
}

void DatabaseManager::updateMessageMetadata(const QList<MessageMetadata> freshMetadataFromServer, const QString emailAddress)
{
    QList<MessageMetadata> messagesFromDatabase = getMessageMetadataForAccount(emailAddress);

    // update status about unread messages

    QFuture<QList<unsigned long>> getUidsOfSeenMessagesOnServerFuture = getUidsOfSeenMessagesOnServer(freshMetadataFromServer, messagesFromDatabase);
    QFutureWatcher<QList<unsigned long>> *getUidsOfSeenMessagesOnServerFutureWatcher = new QFutureWatcher<QList<unsigned long>>();

    connect(getUidsOfSeenMessagesOnServerFutureWatcher, &QFutureWatcher<QList<unsigned long>>::finished, [=](){
        for (unsigned long uid : getUidsOfSeenMessagesOnServerFuture.result())
        {
            DatabaseManager::setMessageAsSeen(uid, emailAddress);
        }
        // TODO: refresh GUI
    });

    getUidsOfSeenMessagesOnServerFutureWatcher->setFuture(getUidsOfSeenMessagesOnServerFuture);

    // delete messages from database which are deleted from server

    QFuture<QList<unsigned long>> getUidsOfMessagesDeletedFromServerFuture = getUidsOfMessagesDeletedFromServer(freshMetadataFromServer, messagesFromDatabase);
    QFutureWatcher<QList<unsigned long>> *getUidsOfMessagesDeletedFromServerFutureWatcher = new QFutureWatcher<QList<unsigned long>>();

    connect(getUidsOfMessagesDeletedFromServerFutureWatcher, &QFutureWatcher<QList<unsigned long>>::finished, [=](){
        for (unsigned long uid : getUidsOfMessagesDeletedFromServerFuture.result())
        {
            DatabaseManager::deleteMessageFromDatabase(uid, emailAddress);
        }
        // TODO: refresh GUI
    });

    getUidsOfMessagesDeletedFromServerFutureWatcher->setFuture(getUidsOfMessagesDeletedFromServerFuture);

    // add new messages to database

    QFuture<QList<MessageMetadata>> getMissingMessagesFromServerFuture = getMissingMessagesFromServer(freshMetadataFromServer, messagesFromDatabase);
    QFutureWatcher<QList<MessageMetadata>> *getMissingMessagesFromServerFutureWatcher = new QFutureWatcher<QList<MessageMetadata>>();

    connect(getMissingMessagesFromServerFutureWatcher, &QFutureWatcher<QList<MessageMetadata>>::finished, [=](){
        int newMessagesCount = 0;

        for (MessageMetadata message : getMissingMessagesFromServerFuture.result())
        {
            if (!message.isSeen())
            {
                newMessagesCount++;
            }
        }

        DatabaseManager::addMessagesMetadataToDatabase(getMissingMessagesFromServerFuture.result());

        // TODO: refresh GUI
    });

    getMissingMessagesFromServerFutureWatcher->setFuture(getMissingMessagesFromServerFuture);
}

void DatabaseManager::setMessageAsSeen(const unsigned long uid, const QString emailAddress)
{
    QList<int> folderIds;

    QSqlQuery folderIdsQuery;

    folderIdsQuery.prepare("SELECT"
                  " id "
                  "FROM Folders"
                  " WHERE "
                  " emailAddress = :emailAddress ");

    folderIdsQuery.bindValue(":emailAddress", emailAddress);
    folderIdsQuery.exec();

    while (folderIdsQuery.next())
    {
        folderIds << folderIdsQuery.value(0).toInt();
    }

    for (int folderId : folderIds)
    {
        QSqlQuery updateQuery;


        updateQuery.prepare("UPDATE MessageData "
                            "SET isSeen = :isSeen "
                            "WHERE folderId = :folderId AND uid = :uid");

        updateQuery.bindValue(":folderId", folderId);
        updateQuery.bindValue(":isSeen", true);
        updateQuery.bindValue(":uid", static_cast<int>(uid));

        updateQuery.exec();
    }
}

void DatabaseManager::deleteMessageFromDatabase(const unsigned long uid, const QString emailAddress)
{
    QList<int> folderIds;

    QSqlQuery folderIdsQuery;

    folderIdsQuery.prepare("SELECT"
                  " id "
                  "FROM Folders"
                  " WHERE "
                  " emailAddress = :emailAddress ");

    folderIdsQuery.bindValue(":emailAddress", emailAddress);
    folderIdsQuery.exec();

    while (folderIdsQuery.next())
    {
        folderIds << folderIdsQuery.value(0).toInt();
    }

    for (int folderId : folderIds)
    {
        QSqlQuery deleteQuery;

        deleteQuery.prepare("DELETE FROM MessageData"
                            " WHERE "
                            "folderId = :folderId AND uid = :uid");

        deleteQuery.bindValue(":folderId", folderId);
        deleteQuery.bindValue(":uid", static_cast<int>(uid));

        deleteQuery.exec();
    }


}

QList<MessageMetadata> DatabaseManager::getMessageMetadataForAccount(const QString emailAddress)
{
    QList<int> folderIds;

    QSqlQuery folderIdsQuery;

    folderIdsQuery.prepare("SELECT"
                  " id "
                  "FROM Folders"
                  " WHERE "
                  " emailAddress = :emailAddress ");

    folderIdsQuery.bindValue(":emailAddress", emailAddress);
    folderIdsQuery.exec();

    while (folderIdsQuery.next())
    {
        folderIds << folderIdsQuery.value(0).toInt();
    }


    QList<MessageMetadata> result;

    for (int folderId : folderIds)
    {
        QSqlQuery metadataQuery;

        metadataQuery.prepare("SELECT"
                      " uid, isSeen "
                      "FROM MessageData"
                      " WHERE "
                      " folderId = :folderId ");

        metadataQuery.bindValue(":folderId", folderId);
        metadataQuery.exec();

        while (metadataQuery.next())
        {
            MessageMetadata metadata;
            metadata.setUid(metadataQuery.value(0).toULongLong());
            metadata.setIsSeen(metadataQuery.value(1).toBool());

            result << metadata;
        }
    }

    return result;
}

}
