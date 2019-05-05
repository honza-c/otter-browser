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

#include "vmimesmtpservice.h"
#include "../../core/NotificationsManager.h"

namespace Otter
{

bool VmimeSmtpService::sendMessage(Message message) const
{
    vmime::shared_ptr<vmime::message> constructedMessage = constructMessage(message);

    try
    {
        vmime::shared_ptr<vmime::net::transport> transport = getTransport();

        transport->connect();
        transport->send(constructedMessage);
        transport->disconnect();

        return true;
    }
    catch (vmime::exception e)
    {
        QString notificationText = m_emailAddress.c_str();
        notificationText.append(": Failed to send email message: ");
        notificationText.append(e.what());

        qWarning() << notificationText;

        Notification *notification(NotificationsManager::createNotification(NotificationsManager::UpdateAvailableEvent, notificationText));
    }

    return false;
}

vmime::shared_ptr<vmime::net::transport> VmimeSmtpService::getTransport() const
{
    vmime::shared_ptr<vmime::net::transport> transport;

    std::string urlString = "smtp://";
    urlString.append(m_serverUrl);

    vmime::utility::url url(urlString);

    transport = m_session->getTransport(url);

    transport->setProperty("auth.username", m_emailAddress);
    transport->setProperty("auth.password", m_password);
    transport->setProperty("options.need-authentication", true);

    if (m_isConnectionEncrypted)
    {
        transport->setProperty("connection.tls", true);
        transport->setCertificateVerifier(m_certificateVerifier);
    }

    return transport;
}

vmime::shared_ptr<vmime::message> VmimeSmtpService::constructMessage(const Message message) const
{
    vmime::messageBuilder messageBuilder;

    messageBuilder.setSubject(vmime::text(message.subject().toStdString()));
    messageBuilder.setExpeditor(vmime::mailbox(m_emailAddress));

    if (message.htmlContent() != QString())
    {
        messageBuilder.constructTextPart(vmime::mediaType(vmime::mediaTypes::TEXT, vmime::mediaTypes::TEXT_HTML));

        vmime::shared_ptr<vmime::htmlTextPart> htmlTextPart = vmime::dynamicCast<vmime::htmlTextPart>(messageBuilder.getTextPart());
        htmlTextPart->setCharset(vmime::charsets::UTF_8);
        htmlTextPart->setPlainText(vmime::make_shared<vmime::stringContentHandler>(message.plainTextContent().toStdString()));

        QString htmlContent = message.htmlContent();

        for (EmbeddedObject embeddedObject : message.embeddedObjects())
        {
            std::string objectData;

            for (int i = 0; i < embeddedObject.data().size(); i++)
            {
                objectData += embeddedObject.data().at(i);
            }

            vmime::encoding encoding;
            encoding.setUsage(vmime::encoding::USAGE_BINARY_DATA);

            vmime::shared_ptr<vmime::contentHandler> contentHandler = vmime::make_shared<vmime::stringContentHandler>(objectData, encoding);
            vmime::mediaType mediaType(embeddedObject.mimeType().toStdString());

            vmime::shared_ptr<const vmime::htmlTextPart::embeddedObject> eobj = htmlTextPart->addObject(contentHandler, mediaType);
            QString cid = eobj->getReferenceId().c_str();

            htmlContent.replace("src=\"cid:" + embeddedObject.name(), "src=\"cid:" + cid);
        }

        htmlTextPart->setText(vmime::make_shared<vmime::stringContentHandler>(htmlContent.toStdString()));
    }
    else
    {
        messageBuilder.getTextPart()->setCharset(vmime::charsets::UTF_8);
        messageBuilder.getTextPart()->setText(
                    vmime::make_shared<vmime::stringContentHandler>(message
                                                                    .plainTextContent()
                                                                    .toStdString()));
    }

    vmime::addressList addressListTo;

    for (Contact contact : message.addressListTo())
    {
        addressListTo.appendAddress(vmime::make_shared<vmime::mailbox>(contact.emailAddress().toStdString()));
    }

    messageBuilder.setRecipients(addressListTo);

    if (!message.addressListInCopy().isEmpty())
    {
        vmime::addressList copyRecipients;

        for (Contact contact : message.addressListInCopy())
        {
            copyRecipients.appendAddress(vmime::make_shared<vmime::mailbox>(contact.emailAddress().toStdString()));
        }

        messageBuilder.setCopyRecipients(copyRecipients);
    }

    if (!message.addressListInBlindCopy().isEmpty())
    {
        vmime::addressList blindCopyRecipients;

        for (Contact contact : message.addressListInBlindCopy())
        {
            blindCopyRecipients.appendAddress(vmime::make_shared<vmime::mailbox>(contact.emailAddress().toStdString()));
        }

        messageBuilder.setBlindCopyRecipients(blindCopyRecipients);
    }

    if (!message.attachments().isEmpty())
    {
        for (Attachment attachment : message.attachments())
        {
            std::string attachmentData;

            for (int i = 0; i < attachment.data().size(); i++)
            {
                attachmentData += attachment.data().at(i);
            }

            vmime::encoding encoding;
            encoding.setUsage(vmime::encoding::USAGE_BINARY_DATA);

            vmime::shared_ptr<vmime::contentHandler> contentHandler
                    = vmime::make_shared<vmime::stringContentHandler>(attachmentData, encoding);

            vmime::word fileName(attachment.name().toStdString().c_str());
            vmime::mediaType mediaType(attachment.mimeType().toStdString());

            vmime::shared_ptr<vmime::fileAttachment> vmimeAttachment = vmime::make_shared<vmime::fileAttachment>(contentHandler, fileName, mediaType);

            messageBuilder.appendAttachment(vmimeAttachment);
        }
    }

    return messageBuilder.construct();
}

}
