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

#include "vmimemessagemetadataparser.h"

namespace Otter
{

MessageMetadata VmimeMessageMetadataParser::parse(const vmime::shared_ptr<vmime::net::message> message)
{
    MessageMetadata messageMetadata;

    vmime::shared_ptr<const vmime::header> header = message->getHeader();

    try
    {
        const int flags = message->getFlags();

        messageMetadata.setIsSeen(flags & vmime::net::message::FLAG_SEEN);
        messageMetadata.setIsDeleted(flags & vmime::net::message::FLAG_DELETED);
        messageMetadata.setIsRecent(flags & vmime::net::message::FLAG_RECENT);
        messageMetadata.setIsReplied(flags & vmime::net::message::FLAG_REPLIED);
        messageMetadata.setIsDraft(flags & vmime::net::message::FLAG_DRAFT);
    }
    catch (std::exception e)
    {
        messageMetadata.setIsSeen(false);
        messageMetadata.setIsDeleted(false);
        messageMetadata.setIsRecent(false);
        messageMetadata.setIsReplied(false);
        messageMetadata.setIsDraft(false);
    }

    messageMetadata.setMessageId(message->getNumber());
    messageMetadata.setSize(message->getSize());

    messageMetadata.setDateTime(parseDateTime(header));
    messageMetadata.setSubject(parseSubject(header));

    Contact from = parseExpeditor(header);
    messageMetadata.setFrom(from);

    QList<Contact> replyTo = parseReplyTo(header);

    if (replyTo.size() != 1 || replyTo.first().emailAddress() != from.emailAddress())
    {
        messageMetadata.setReplyTo(replyTo);
    }
    else
    {
        messageMetadata.setReplyTo(QList<Contact>());
    }

    return messageMetadata;
}

QString VmimeMessageMetadataParser::vmimeDecodedStringToQString(vmime::string input)
{
    vmime::text output;
    vmime::text::decodeAndUnfold(input, &output);
    return QString(output.getConvertedText(vmime::charsets::UTF_8).c_str());
}

QString VmimeMessageMetadataParser::parseSubject(vmime::shared_ptr<const vmime::header> header)
{
    QString extractedSubject = vmimeDecodedStringToQString(header->Subject()->generate());

    // Extracted data is in format: "Subject: desired data..."
    // Removing "Subject: " from the extracted string

    return extractedSubject.right(extractedSubject.size() - 9);
}

QString VmimeMessageMetadataParser::parseContactName(QString extractedHeader)
{
    QRegExp emailAddressRegex = getEmailAddressRegexPattern();
    emailAddressRegex.indexIn(extractedHeader);
    QStringList capturedEmailAddresses = emailAddressRegex.capturedTexts();

    // if From header is in format "From: john.doe@example.com" return this address as name
    if (capturedEmailAddresses.last() == extractedHeader)
    {
        return extractedHeader;
    }

    // if From header is in format "From: John Doe <john.doe@example.com>"
    // or "From: "John Doe" <john.doe@example.com>"
    // remove part with e-mail address and keep only the name
    QString fromName = extractedHeader.left(extractedHeader.lastIndexOf(' '));

    // if the are at the name's begin and end quoted marks, remove them and return name
    if (fromName.at(0) == '"' && fromName.at(fromName.length() - 1) == '"')
    {
        return fromName.mid(1, fromName.length() - 2);
    }

    // else name isn't covered with quote marks, so return it as it is
    return fromName;
}

QList<Contact> VmimeMessageMetadataParser::parseReplyTo(vmime::shared_ptr<const vmime::header> header)
{
    if (header->hasField("Reply-To"))
    {
        QString extractedHeader = vmimeDecodedStringToQString(header->ReplyTo()->generate());

        // extracted data is in format: "Reply-To: desired data..."
        // removing "Reply-To: " part from the extracted string

        extractedHeader = extractedHeader.right(extractedHeader.size() - 10);

        QStringList headerContacts = extractedHeader.split(",");

        QList<Contact> contacts;

        for (QString headerContact : headerContacts)
        {
            Contact contact;
            contact.setName(parseContactName(headerContact));
            contact.setEmailAddress(parseContactEmailAddress(headerContact));

            contacts << contact;
        }

        return contacts;
    }

    return QList<Contact>();
}

Contact VmimeMessageMetadataParser::parseExpeditor(vmime::shared_ptr<const vmime::header> header)
{
    QString extractedFromHeader = vmimeDecodedStringToQString(header->From()->generate());

    // extracted data is in format: "From: desired data..."
    // removing "From: " part from the extracted string
    extractedFromHeader = extractedFromHeader.right(extractedFromHeader.size() - 6);

    Contact contact;

    contact.setName(parseContactName(extractedFromHeader));
    contact.setEmailAddress(parseContactEmailAddress(extractedFromHeader));

    return contact;
}

QString VmimeMessageMetadataParser::parseContactEmailAddress(QString extractedHeader)
{
    QRegExp emailAddressRegex = getEmailAddressRegexPattern();
    emailAddressRegex.indexIn(extractedHeader);
    QStringList capturedEmailAddresses = emailAddressRegex.capturedTexts();

    return capturedEmailAddresses.last();
}

QDateTime VmimeMessageMetadataParser::parseDateTime(vmime::shared_ptr<const vmime::header> header)
{
    try
    {
        QString extractedDate = vmimeDecodedStringToQString(header->Date()->generate());

        // extracted data is in format: "Date: desired data..."
        // removing "Date: " part from the extracted string
        extractedDate = extractedDate.right(extractedDate.size() - 6);

        // Date is in format ""Fri, 5 Aug 2011 05:22:45 -0700"
        // Extracting information about date and time without timezone
        QString dateTimeInformation = extractedDate.left(extractedDate.length() - 6);
        // Extracting just timezone information
        QString timeZoneInformation = extractedDate.right(5);

        QDateTime dateTime = QDateTime::fromString(dateTimeInformation, "ddd, d MMM yyyy HH:mm:ss");
        int timezoneOffset = getTimeZoneOffsetInUnixTime(timeZoneInformation);
        dateTime.setOffsetFromUtc(timezoneOffset);


        return dateTime;
    }
    catch (std::exception e)
    {
        return QDateTime();
    }
}

QRegExp VmimeMessageMetadataParser::getEmailAddressRegexPattern() const
{
    QRegExp emailAddressRegexPattern("\\b[A-Z0-9._%+-]+@[A-Z0-9.-]+\\.[A-Z]{2,4}\\b");
    emailAddressRegexPattern.setCaseSensitivity(Qt::CaseInsensitive);
    emailAddressRegexPattern.setPatternSyntax(QRegExp::RegExp);

    return emailAddressRegexPattern;
}

int VmimeMessageMetadataParser::getTimeZoneOffsetInUnixTime(const QString timezone)
{
    QChar sign = timezone[0];
    QString hoursStr = timezone.mid(1, 2);
    QString minutesStr = timezone.right(2);

    int offset = minutesStr.toInt() * 60 + hoursStr.toInt() * 3600;

    if (sign == '-')
    {
        offset = -offset;
    }

    return offset;
}

}
