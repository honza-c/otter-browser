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

#ifndef VMIMEMESSAGEMETADATAPARSER_H
#define VMIMEMESSAGEMETADATAPARSER_H

#include <QObject>
#include <vmime/vmime.hpp>
#include "messagemetadata.h"
#include "contact.h"

namespace Otter
{

class VmimeMessageMetadataParser : public QObject
{
    Q_OBJECT
public:
    explicit VmimeMessageMetadataParser(QObject *parent = nullptr)
        : QObject(parent) {}

    MessageMetadata parse(const vmime::shared_ptr<vmime::net::message> message);

signals:

public slots:

private:
    QString vmimeDecodedStringToQString(vmime::string input);
    QString parseSubject(vmime::shared_ptr<const vmime::header> header);
    QString parseContactName(QString extractedHeader);
    QString parseContactEmailAddress(QString extractedHeader);
    QDateTime parseDateTime(vmime::shared_ptr<const vmime::header> header);
    QRegExp getEmailAddressRegexPattern() const;
    int getTimeZoneOffsetInUnixTime(const QString timezone);

    Contact parseExpeditor(vmime::shared_ptr<const vmime::header> header);
    QList<Contact> parseReplyTo(vmime::shared_ptr<const vmime::header> header);
};

}

#endif // VMIMEMESSAGEMETADATAPARSER_H
