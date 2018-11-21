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

#include "vmimeinboxfolderparser.h"

namespace Otter
{

VmimeInboxFolderParser::VmimeInboxFolderParser(vmime::shared_ptr<vmime::net::folder> folder, QObject *parent)
    : QObject(parent)
{
    m_folder = folder;
    m_folderAttributes = m_folder->getAttributes();
}

InboxFolder VmimeInboxFolderParser::parse() const
{
    InboxFolder inboxFolder;

    inboxFolder.setPath(getFolderPath());
    inboxFolder.setIsAllMessages(m_folderAttributes.getSpecialUse() == vmime::net::folderAttributes::SPECIALUSE_ALL);
    inboxFolder.setIsArchive(m_folderAttributes.getSpecialUse() == vmime::net::folderAttributes::SPECIALUSE_ARCHIVE);
    inboxFolder.setIsDrafts(m_folderAttributes.getSpecialUse() == vmime::net::folderAttributes::SPECIALUSE_DRAFTS);
    inboxFolder.setIsFlagged(m_folderAttributes.getSpecialUse() == vmime::net::folderAttributes::SPECIALUSE_FLAGGED);
    inboxFolder.setIsImportant(m_folderAttributes.getSpecialUse() == vmime::net::folderAttributes::SPECIALUSE_IMPORTANT);
    inboxFolder.setIsJunk(m_folderAttributes.getSpecialUse() == vmime::net::folderAttributes::SPECIALUSE_JUNK);
    inboxFolder.setIsSent(m_folderAttributes.getSpecialUse() == vmime::net::folderAttributes::SPECIALUSE_SENT);
    inboxFolder.setIsTrash(m_folderAttributes.getSpecialUse() == vmime::net::folderAttributes::SPECIALUSE_TRASH);
    inboxFolder.setHasChildren(m_folder->getFolders(false).size() > 0);

    return inboxFolder;
}

QString VmimeInboxFolderParser::getFolderPath() const
{
    QString path = getFolderPath(m_folder);

    if (path != "/")
    {
        return path.left(path.size() - 1);
    }

    return path;
}

QString VmimeInboxFolderParser::getFolderPath(const vmime::shared_ptr<vmime::net::folder> folder) const
{
    vmime::string path = folder->getName().getBuffer();

    if (path.empty())
    {
        return QString("/");
    }
    else
    {
        vmime::shared_ptr<vmime::net::folder> parent = folder->getParent();
        return QString(getFolderPath(parent) + path.c_str() + "/");
    }
}

}
