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
    bool initializeStore();
    QList<MessageMetadata> fetchMessagesMetadata();
    MessageContent fetchMessageContent(QString folderPath, int positionInFolder);
    long moveMessage(const int uid, const QString oldPath, const QString newPath);
    bool deleteMessage(const int uid, QString folderPath);
    bool renameFolder(const QString originalFolderPath, const QString renamedFolderPath);
    bool deleteFolder(const QString folderPath);
    bool createFolder(const QString folderPath);
    long copyMessage(const int uid, const QString oldPath, const QString newPath);
    bool setMessageAsSeen(const int uid, const QString folderPath);
    bool setMessageAsUnseen(const int uid, const QString folderPath);
protected:
    virtual QString getStoreUrl() const = 0;

    vmime::shared_ptr<vmime::net::store> m_store;
};

}

#endif // VMIMEINBOXSERVICE_H
