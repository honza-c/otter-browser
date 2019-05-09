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

#include "simplemailsmtpservice.h"
#include "SmtpMime"
#include "../../core/NotificationsManager.h"

namespace Otter
{

bool SimpleMailSmtpService::sendMessage(const Message message) const
{
    SimpleMail::MimeMessage mimeMessage;

    SimpleMail::EmailAddress sender(message.sender().emailAddress(), message.sender().name());
    mimeMessage.setSender(sender);

    for (Contact recipient : message.addressListTo())
    {
        SimpleMail::EmailAddress address(recipient.emailAddress(), recipient.name());
        mimeMessage.addTo(address);
    }

    for (Contact copyRecipient : message.addressListInCopy())
    {
        SimpleMail::EmailAddress address(copyRecipient.emailAddress(), copyRecipient.name());
        mimeMessage.addCc(address);
    }

    for (Contact blindCopyRecipient : message.addressListInBlindCopy())
    {
        SimpleMail::EmailAddress address(blindCopyRecipient.emailAddress(), blindCopyRecipient.name());
        mimeMessage.addBcc(address);
    }

    mimeMessage.setSubject(message.subject());

    SimpleMail::MimeText text;
    if (message.plainTextContent() != QString())
    {
        text.setText(message.plainTextContent());
        mimeMessage.addPart(&text);
    }

    SimpleMail::MimeHtml html;
    QList<SimpleMail::MimeInlineFile*> embeddedObjects;

    if (message.htmlContent() != QString())
    {
        html.setHtml(message.htmlContent());

        mimeMessage.addPart(&html);

        for (EmbeddedObject object : message.embeddedObjects())
        {
            SimpleMail::MimeInlineFile *embeddedObject = new SimpleMail::MimeInlineFile(new QFile(object.data()));
            embeddedObject->setContentId(object.contentId().toStdString().c_str());
            embeddedObject->setContentType(object.contentId().toStdString().c_str());

            embeddedObjects.append(embeddedObject);
        }
    }

    for (SimpleMail::MimeInlineFile *embeddedObject : embeddedObjects)
    {
        mimeMessage.addPart(embeddedObject);
    }

    QList<SimpleMail::MimeAttachment*> attachments;

    for (Attachment attachment : message.attachments())
    {
        SimpleMail::MimeAttachment *mimeAttachment = new SimpleMail::MimeAttachment(attachment.data(), attachment.name());
        mimeAttachment->setContentType(attachment.mimeType().toStdString().c_str());

        attachments.append(mimeAttachment);
    }

    for (SimpleMail::MimeAttachment *attachment : attachments)
    {
        mimeMessage.addPart(attachment);
    }

    if (m_isConnectionEncrypted)
    {
        SimpleMail::Sender smtp(QLatin1String(m_serverUrl.c_str()), m_port, SimpleMail::Sender::SslConnection);
        smtp.setUser(QLatin1String(m_emailAddress.c_str()));
        smtp.setPassword(QLatin1String(m_password.c_str()));
        smtp.setResponseTimeout(20000);

        if (!smtp.sendMail(mimeMessage))
        {
            QString notificationText = m_emailAddress.c_str();
            notificationText.append(": Failed to send email message: ");
            notificationText.append(smtp.lastError());

            qWarning() << notificationText;

            Notification *notification(NotificationsManager::createNotification(NotificationsManager::UpdateAvailableEvent, notificationText));

            smtp.quit();
            return false;
        }
    }
    else
    {
        SimpleMail::Sender smtp(QString(m_serverUrl.c_str()), m_port, SimpleMail::Sender::ConnectionType::TcpConnection);
        smtp.setUser(m_emailAddress.c_str());
        smtp.setPassword(m_password.c_str());
        smtp.setResponseTimeout(20000);

        if (!smtp.sendMail(mimeMessage))
        {
            QString notificationText = m_emailAddress.c_str();
            notificationText.append(": Failed to send email message: ");
            notificationText.append(smtp.lastError());

            qWarning() << notificationText;

            Notification *notification(NotificationsManager::createNotification(NotificationsManager::UpdateAvailableEvent, notificationText));

            smtp.quit();
            return false;
        }
    }

    return true;
}

}
