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

#ifndef VMIMEMESSAGECONTENTPARSER_H
#define VMIMEMESSAGECONTENTPARSER_H

#include <QObject>
#include <QTextCodec>
#include <vmime/vmime.hpp>
#include "attachment.h"
#include "contact.h"
#include "embeddedobject.h"

namespace Otter
{

class VmimeMessageContentParser : public QObject
{
    Q_OBJECT
public:
    explicit VmimeMessageContentParser(vmime::shared_ptr<vmime::message> message, QObject *parent = nullptr);

    QString parseHtmlContent() const;
    QString parsePlaintextContent() const;
    QList<Attachment> parseAttachments() const;
    QList<EmbeddedObject> parseEmbeddedObjects() const;
    QList<Contact> parseRecipients() const;
    QList<Contact> parseInCopyRecipients() const;
    QList<Contact> parseBlindCopyRecipients() const;

signals:

public slots:

private:
    QString parseTextParts(const char *type) const;
    QByteArray parseContentHandler(vmime::shared_ptr<const vmime::contentHandler> contentHandler) const;
    QList<Contact> parseContacts(const vmime::shared_ptr<vmime::address> address) const;

    std::vector<vmime::shared_ptr<const vmime::htmlTextPart::embeddedObject>> extractEmbeddedObjects() const;
    vmime::shared_ptr<vmime::message> m_message;
    vmime::shared_ptr<vmime::messageParser> m_messageParser;
};

}

#endif // VMIMEMESSAGECONTENTPARSER_H
