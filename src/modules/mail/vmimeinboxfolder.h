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

#ifndef VMIMEINBOXFOLDER_H
#define VMIMEINBOXFOLDER_H

#include <QObject>
#include <QDebug>
#include <vmime/vmime.hpp>
#include "vmimeinboxfolderparser.h"
#include "vmimemessagecontentparser.h"
#include "vmimemessagemetadataparser.h"
#include "messagemetadata.h"
#include "attachment.h"
#include "databasemanager.h"
#include "inboxfolder.h"
#include "contact.h"

namespace Otter
{

class VmimeInboxFolder : public QObject
{
    Q_OBJECT
public:
    explicit VmimeInboxFolder(vmime::shared_ptr<vmime::net::folder> remoteFolder, QString emailAddress, QObject *parent = nullptr);
    VmimeInboxFolder(const VmimeInboxFolder &other);
    VmimeInboxFolder &operator=(const VmimeInboxFolder &other);

    InboxFolder data() const;
    void setData(const InboxFolder data);

    QList<MessageMetadata> getMessagesMetadataFromPosition(int position) const;
signals:

public slots:

private:
    void openFolder();
    vmime::shared_ptr<vmime::net::folder> m_remoteFolder;
    InboxFolder m_inboxFolder;

    bool m_successfullyOpened;
};

}

#endif // VMIMEINBOXFOLDER_H
