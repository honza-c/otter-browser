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

#ifndef VMIMEINBOXSERVICE_H
#define VMIMEINBOXSERVICE_H

#include <QIcon>
#include <QVariant>
#include <vmime/vmime.hpp>
#include "vmimebasemessagingservice.h"
#include "vmimeinboxfolder.h"
#include "messagemetadata.h"
#include "databasemanager.h"
#include "messagecontent.h"
#include "contact.h"
#include <QSqlError>

namespace Otter
{

class VmimeInboxService : public VmimeBaseMessagingService
{
public:
    explicit VmimeInboxService(QObject *parent = nullptr)
        : VmimeBaseMessagingService(parent) {}

    QList<InboxFolder> fetchInboxFolders();
    void initializeStore();
    QList<MessageMetadata> fetchMessagesMetadata(QMap<QString, int> folderPathsWithMessagesCountsInDb);
    QList<MessageMetadata> fetchMessagesMetadata();
    MessageContent fetchMessageContent(QString folderPath, int positionInFolder);
    void moveMessage(const QString sourceFolderPath, const int messageId, const QString destinationFolderPath);

    void deleteMessage(const int uid, QString folderPath);
    void renameFolder(const QString originalFolderPath, const QString renamedFolderPath);
    void deleteFolder(const QString folderPath);
protected:
    virtual vmime::utility::url getStoreUrl() const = 0;

    bool m_storeInitialized;
    vmime::shared_ptr<vmime::net::store> m_store;
    bool m_foldersCreated();
    QList<VmimeInboxFolder> m_folders;
};

}

#endif // VMIMEINBOXSERVICE_H
