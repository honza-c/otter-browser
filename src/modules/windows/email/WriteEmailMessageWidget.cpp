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

#include "WriteEmailMessageWidget.h"
#include "../../../core/ThemesManager.h"
#include "../../../ui/Action.h"
#include "../../../ui/MainWindow.h"
#include "../../../ui/Window.h"

#include <QtWidgets/QDesktopWidget>
#include <QtWidgets/QToolTip>

namespace Otter
{

WriteEmailMessageWidget::WriteEmailMessageWidget(QWidget *parent) :
    QWidget (parent),
    m_ui(new Ui::WriteEmailMessageWidget)
{
    m_ui->setupUi(this);

    m_ui->sendButton->setIcon(ThemesManager::createIcon(QLatin1String("mail-send"), true));
    m_ui->attachButton->setIcon(ThemesManager::createIcon(QLatin1String("mail-attachment"), true));
    m_ui->backToInboxButton->setIcon(ThemesManager::createIcon(QLatin1String("go-previous"), true));

    m_ui->messageContentSplitter->setStretchFactor(0, 1);
    m_ui->messageContentSplitter->setStretchFactor(1, 2);

    EmailAccountsListModel *model = new EmailAccountsListModel(EmailAccountsManager::getEmailAccounts());
    m_ui->senderComboBox->setModel(model);

    QStringList contactsDataForCompleter = DatabaseManager::getDataForContactsCompleter();
    m_completer = new MultiSelectCompleter(contactsDataForCompleter, this);
    m_completer->setCaseSensitivity(Qt::CaseInsensitive);
    m_completer->setFilterMode(Qt::MatchContains);

    m_ui->toLineEdit->setCompleter(m_completer);
    m_ui->inCopyLineEdit->setCompleter(m_completer);
    m_ui->inBlindCopyLineEdit->setCompleter(m_completer);

    m_messageEdited = false;
}

WriteEmailMessageWidget::~WriteEmailMessageWidget()
{
    delete m_ui;
}

void WriteEmailMessageWidget::setMode(const Mode mode)
{
    m_mode = mode;
}

void WriteEmailMessageWidget::setSenderComboBoxEditable(bool value)
{
    m_ui->senderComboBox->setEnabled(value);
}

void WriteEmailMessageWidget::setMessage(Message message)
{
    m_message = message;

    if (message == Message())
    {
        resetWidget();
    }
    else
    {
        setSenderIndex(message);
        setSubject(message);
        setPlainTextAndHtmlText(message);
        setRecipients(message);
        setAttachments(message);
        setEmbeddedObjects(message);
    }

    m_messageEdited = false;
}

void WriteEmailMessageWidget::resetWidget()
{
    m_attachments = QList<Attachment>();
    m_embeddedObjects = QList<EmbeddedObject>();
    m_ui->senderComboBox->setCurrentIndex(0);
    m_ui->subjectLineEdit->setText(QString());
    m_ui->messageContentTextBrowser->setHtml(QString());
    m_ui->messageContentTextEdit->setPlainText(QString());
    m_ui->messageContentTextBrowser->setVisible(false);
    m_ui->toLineEdit->setText(QString());
    m_ui->inCopyLineEdit->setText(QString());
    m_ui->inBlindCopyLineEdit->setText(QString());
    m_ui->attachmentListView->setModel(new AttachmentsListModel(m_attachments));
    m_ui->attachmentsWidget->setVisible(false);

    m_messageEdited = false;
}

void WriteEmailMessageWidget::setSenderIndex(const Message message)
{
    if (m_mode == Mode::Reply || m_mode == Mode::ReplyAll || m_mode == Mode::Forward)
    {
        QList<Contact> recipients = message.addressListTo();

        for (int i = 0; i < recipients.size(); i++)
        {
            for (int j = 0; j < EmailAccountsManager::getEmailAccounts().size(); j++)
            {
                if (recipients.at(i).emailAddress() == EmailAccountsManager::getEmailAccounts().at(j).emailAddress())
                {
                    m_ui->senderComboBox->setCurrentIndex(j);
                    m_ui->senderComboBox->setEnabled(false);
                }
            }
        }
    }
}

void WriteEmailMessageWidget::setSubject(const Message message)
{
    if (m_mode == Mode::Forward)
    {
        m_ui->subjectLineEdit->setText("Fwd: " + message.subject());
    }
    else if (m_mode == Mode::Reply || m_mode == Mode::ReplyAll)
    {
        m_ui->subjectLineEdit->setText("Re: " + message.subject());
    }
}

void WriteEmailMessageWidget::setPlainTextAndHtmlText(const Message message)
{
    if (message.htmlContent() == QString())
    {
        m_ui->messageContentTextBrowser->setVisible(false);
        setPlainTextContent(message.plainTextContent());

    }
    else
    {
        if (m_mode == Mode::Reply || m_mode == Mode::ReplyAll || m_mode == Mode::Forward)
        {
            m_ui->messageContentTextBrowser->setVisible(true);
            setHtmlContent(message.htmlContent());
        }
        else
        {
            m_ui->messageContentTextBrowser->setVisible(false);
        }
    }
}

void WriteEmailMessageWidget::setRecipients(const Message message)
{
    QString recipients;

    if (m_mode != Mode::Forward && m_mode != Mode::WriteMessage)
    {
        if (!message.replyTo().isEmpty())
        {
            recipients.append(Contact::toString(message.replyTo()));
        }
        else
        {
            recipients.append(Contact::toString(message.sender()));
        }

        if (m_mode == Mode::ReplyAll)
        {
            if (message.addressListTo().size() > 1)
            {

                recipients.append(", ");

                for (int i = 0; i < message.addressListTo().size(); i++)
                {
                    Contact contact = message.addressListTo().at(i);

                    if (m_ui->senderComboBox->currentText().contains(contact.emailAddress()))
                    {
                        continue;
                    }

                    recipients.append(Contact::toString(contact));

                    if (i < message.addressListTo().size() - 1)
                    {
                        recipients.append(", ");
                    }
                }
            }

            if (recipients.lastIndexOf(",") != recipients.size() - 2)
            {
                recipients.append(", ");
            }

            if (!message.addressListInCopy().isEmpty())
            {
                recipients.append(Contact::toString(message.addressListInCopy()));
            }
        }

        m_ui->toLineEdit->setText(recipients);
    }
}

void WriteEmailMessageWidget::setAttachments(const Message message)
{
    m_attachments = message.attachments();
    AttachmentsListModel *model = new AttachmentsListModel(m_attachments);
    m_ui->attachmentListView->setModel(model);

    if (m_attachments.size() <= 0)
    {
        m_ui->attachmentsWidget->setVisible(false);
    }
    else
    {
        m_ui->attachmentsWidget->setVisible(true);
    }
}

void WriteEmailMessageWidget::setHtmlContent(const QString content)
{
    if (m_mode == Mode::Forward)
    {
        QString forwardContent;

        forwardContent.append("--------Forwarded Message--------<br>");
        forwardContent.append("<b>Subject:</b> " + m_message.subject() + "<br>");
        forwardContent.append("<b>Date:</b> " + m_message.dateTime().toString(dateTimeFormat) + "<br>");
        forwardContent.append("<b>From:</b> " + m_message.sender().name() + " <" + m_message.sender().emailAddress() + ">" + "<br>");
        forwardContent.append("<b>To:</b> ");

        QString recipients = Contact::toString(m_message.addressListTo());
        recipients.replace("<", "&lt;");
        recipients.replace(">", "&gt;");

        forwardContent.append(recipients);
        forwardContent.append(content);

        m_ui->messageContentTextBrowser->setHtml(forwardContent);
    }
    else if (m_mode == Mode::Reply || m_mode == Mode::ReplyAll)
    {
        QString replyHtmlContent;

        replyHtmlContent.append("<p>On ");

        replyHtmlContent.append(m_message.dateTime().toString(dateTimeFormat));
        replyHtmlContent.append(", ");

        QString sender = Contact::toString(m_message.sender());

        sender.replace("<", "&lt;");
        sender.replace(">", "&gt;");

        replyHtmlContent.append(sender);
        replyHtmlContent.append(" wrote:<br></p>");

        replyHtmlContent.append("<div style=\"border-left: thick solid #201e50; margin-left: 15px;\">");
        replyHtmlContent.append(content);
        replyHtmlContent.append("</div>");

        m_ui->messageContentTextBrowser->setHtml(replyHtmlContent);
    }
}

void WriteEmailMessageWidget::setPlainTextContent(const QString content)
{
    if (m_mode == Mode::Forward)
    {
        QString forwardContent;

        forwardContent.append('\n');
        forwardContent.append('\n');
        forwardContent.append("---------- Forwarded message ---------");
        forwardContent.append('\n');

        forwardContent.append("Subject: ");
        forwardContent.append(m_message.subject());
        forwardContent.append('\n');

        forwardContent.append("Date: ");
        forwardContent.append(m_message.dateTime().toString(dateTimeFormat));
        forwardContent.append('\n');

        forwardContent.append("From: ");
        forwardContent.append(Contact::toString(m_message.sender()));
        forwardContent.append('\n');

        forwardContent.append("To: ");
        forwardContent.append(Contact::toString(m_message.addressListTo()));
        forwardContent.append('\n');

        if (!m_message.addressListInCopy().isEmpty())
        {
            forwardContent.append("In copy: ");
            forwardContent.append(Contact::toString(m_message.addressListInCopy()));
            forwardContent.append('\n');
        }

        forwardContent.append('\n');
        forwardContent.append(content);

        m_ui->messageContentTextEdit->setText(forwardContent);
    }
    else if (m_mode == Mode::Reply || m_mode == Mode::ReplyAll)
    {
        QString replyContent;

        replyContent.append("\n");
        replyContent.append("On ");
        replyContent.append(m_message.dateTime().toString(dateTimeFormat));
        replyContent.append(", ");
        replyContent.append(Contact::toString(m_message.sender()));
        replyContent.append(" wrote:\n\n");

        QStringList lines = content.split('\n');

        for (QString line : lines)
        {
            replyContent.append("> ");

            if (line.length() >= 78)
            {
                QStringList sublines;

                int length = line.length();

                for (int i = 0; i < length % 78; i++)
                {
                    bool isLastPart = (i + 1) == length % 78;

                    if (isLastPart)
                    {
                        sublines.append(line.mid(i * 78, -1));
                    }
                    else
                    {
                        sublines.append(line.mid(i * 78, 78));
                    }
                }

                for (QString &subline : sublines)
                {
                    int breakPosition = subline.lastIndexOf(" ");
                    subline.replace(breakPosition, 1, "\n> ");
                    replyContent.append(subline);
                }
            }
            else
            {
                replyContent.append(line);
            }
        }

        replyContent.append("\n\n");

        m_ui->messageContentTextEdit->setText(replyContent);
    }
}

void WriteEmailMessageWidget::setEmbeddedObjects(const Message message)
{
    m_embeddedObjects = message.embeddedObjects();
}

void WriteEmailMessageWidget::on_backToInboxButton_clicked()
{
    if (m_messageEdited)
    {
        QMessageBox messageBox;

        messageBox.setWindowTitle("Discard changes?");
        messageBox.setText("Written message was not sent. Are you sure to discard all changes in the message?");
        messageBox.setIcon(QMessageBox::Question);
        messageBox.addButton(new QPushButton("Cancel"), QMessageBox::ButtonRole::RejectRole);
        messageBox.addButton(new QPushButton("Discard changes"), QMessageBox::ButtonRole::AcceptRole);

        if (!messageBox.exec())
        {
            return;
        }
    }

    emit returnToInboxRequested();
}

void WriteEmailMessageWidget::on_attachButton_clicked()
{
    addAttachment();
}

void WriteEmailMessageWidget::on_sendButton_clicked()
{
    // sender
    Contact sender;
    sender.setEmailAddress(EmailAccountsManager::getEmailAccounts().at(m_ui->senderComboBox->currentIndex()).emailAddress());
    sender.setName(EmailAccountsManager::getEmailAccounts().at(m_ui->senderComboBox->currentIndex()).contactName());

    // to
    QList<Contact> recipients = parseContacts(m_ui->toLineEdit->text());

    if (recipients.isEmpty())
    {
        QMessageBox messageBox;

        messageBox.setWindowTitle("Could not parse message");
        messageBox.setText("Failed to parse recipients. Recipients in To: field should be a list of email addresses separated by comma");
        messageBox.setIcon(QMessageBox::Critical);
        messageBox.addButton(new QPushButton("Close"), QMessageBox::ButtonRole::AcceptRole);

        messageBox.exec();

        return;
    }

    // in copy
    QList<Contact> copyRecipients;

    if (m_ui->inCopyLineEdit->text() != QString())
    {
        copyRecipients = parseContacts(m_ui->inCopyLineEdit->text());

        if (copyRecipients.isEmpty())
        {
            QMessageBox messageBox;

            messageBox.setWindowTitle("Could not parse message");
            messageBox.setText("Failed to parse copy recipients. Recipients in In copy: field should be a list of email addresses separated by comma");
            messageBox.setIcon(QMessageBox::Critical);
            messageBox.addButton(new QPushButton("Close"), QMessageBox::ButtonRole::AcceptRole);

            messageBox.exec();

            return;
        }
    }

    // in blind copy
    QList<Contact> blindCopyRecipients;

    if (m_ui->inBlindCopyLineEdit->text() != QString())
    {
        blindCopyRecipients = parseContacts(m_ui->inBlindCopyLineEdit->text());

        if (blindCopyRecipients.isEmpty())
        {
            QMessageBox messageBox;

            messageBox.setWindowTitle("Could not parse message");
            messageBox.setText("Failed to parse blind copy recipients. Recipients in In blind copy: field should be a list of email addresses separated by comma");
            messageBox.setIcon(QMessageBox::Critical);
            messageBox.addButton(new QPushButton("Close"), QMessageBox::ButtonRole::AcceptRole);

            messageBox.exec();

            return;
        }
    }

    // subject

    QString subject = m_ui->subjectLineEdit->text();

    // content
    QString plainTextContent;
    QString htmlContent;

    if (!m_ui->messageContentTextBrowser->isVisible())
    {
        plainTextContent = m_ui->messageContentTextEdit->toPlainText();
    }
    else
    {
        htmlContent.append(m_ui->messageContentTextEdit->toPlainText());
        htmlContent.append(m_ui->messageContentTextBrowser->toHtml());

        plainTextContent.append(m_ui->messageContentTextEdit->toPlainText());
        plainTextContent.append(m_ui->messageContentTextBrowser->toPlainText());
    }

    Message message;
    message.setSender(sender);
    message.setAddressListTo(recipients);
    message.setAddressListInCopy(copyRecipients);
    message.setAddressListInBlindCopy(blindCopyRecipients);
    message.setSubject(subject);
    message.setPlainTextContent(plainTextContent);
    message.setHtmlContent(htmlContent);
    message.setDateTime(QDateTime::currentDateTime());
    message.setAttachments(m_attachments);
    message.setEmbeddedObjects(m_embeddedObjects);

    int senderId = m_ui->senderComboBox->currentIndex();

    if (EmailAccountsManager::getEmailAccounts().at(senderId).sendMessage(message))
    {
        emit returnToInboxRequested();
    }
}

QList<Contact> WriteEmailMessageWidget::parseContacts(const QString rawdata) const
{
    QList<Contact> contacts;
    QStringList contactsData;

    if (!rawdata.contains(","))
    {
        contactsData << rawdata;
    }
    else
    {
        contactsData = rawdata.split(",");
    }

    for (QString contactData : contactsData)
    {
        QRegExp emailAddressRegex = getEmailAddressRegexPattern();
        emailAddressRegex.indexIn(contactData);

        if (emailAddressRegex.capturedTexts().last() != QString())
        {
            Contact contact;
            contact.setEmailAddress(emailAddressRegex.capturedTexts().last());
            contacts.append(contact);
        }
        else
        {
            return QList<Contact>();
        }
    }

    return contacts;
}

QRegExp WriteEmailMessageWidget::getEmailAddressRegexPattern() const
{
    QRegExp emailAddressRegexPattern("\\b[A-Z0-9._%+-]+@[A-Z0-9.-]+\\.[A-Z]{2,4}\\b");
    emailAddressRegexPattern.setCaseSensitivity(Qt::CaseInsensitive);
    emailAddressRegexPattern.setPatternSyntax(QRegExp::RegExp);

    return emailAddressRegexPattern;
}

void WriteEmailMessageWidget::on_addAttachmentToolboxButton_clicked()
{
    addAttachment();
}

void WriteEmailMessageWidget::addAttachment()
{
    QFileDialog dialog;
    QString defaultPath = QStandardPaths::standardLocations(QStandardPaths::HomeLocation).first();
    dialog.setDirectory(defaultPath);
    dialog.setWindowTitle("Choose attachments");
    dialog.setAcceptMode(QFileDialog::AcceptOpen);

    if (dialog.exec() == QDialog::Accepted)
    {
        QStringList fileNames = dialog.selectedFiles();

        for (QString fileName : fileNames)
        {
            QString name = fileName.split("/").last();

            QFile file(fileName);

            if (file.open(QIODevice::ReadOnly))
            {
                QMimeDatabase mimeDatabase;
                QMimeType mimeType = mimeDatabase.mimeTypeForFile(fileName);

                QByteArray data = file.readAll();
                file.close();

                if (data != QByteArray())
                {
                    Attachment attachment;
                    attachment.setData(data);
                    attachment.setMimeType(mimeType.name());
                    attachment.setName(name);

                    m_attachments.push_back(attachment);
                }
                else
                {
                    QMessageBox messageBox;

                    messageBox.setWindowTitle("Could open attachment");
                    messageBox.setText("Failed to open file: " + fileName);
                    messageBox.setIcon(QMessageBox::Critical);
                    messageBox.addButton(new QPushButton("Close"), QMessageBox::ButtonRole::AcceptRole);

                    messageBox.exec();
                }
            }
            else
            {
                QMessageBox messageBox;

                messageBox.setWindowTitle("Could open attachment");
                messageBox.setText("Failed to open file: " + fileName);
                messageBox.setIcon(QMessageBox::Critical);
                messageBox.addButton(new QPushButton("Close"), QMessageBox::ButtonRole::AcceptRole);

                messageBox.exec();
            }
        }

        m_ui->attachmentListView->setModel(new AttachmentsListModel(m_attachments));

        if (m_attachments.size() > 0)
        {
            m_ui->attachmentsWidget->setVisible(true);
        }
    }
}

void WriteEmailMessageWidget::on_removeAttachmentToolboxButton_clicked()
{
    QItemSelectionModel *selectionModel = m_ui->attachmentListView->selectionModel();

    if (selectionModel->hasSelection())
    {
        int index = selectionModel->selectedRows().at(0).row();
        m_ui->attachmentListView->model()->removeRows(index, 1);
    }

    if (m_attachments.size() == 0)
    {
        m_ui->attachmentsWidget->setVisible(false);
    }
}

bool WriteEmailMessageWidget::hasUnsavedMessage()
{
    return m_messageEdited;
}

}

void Otter::WriteEmailMessageWidget::on_toLineEdit_textEdited(const QString &arg1)
{
    m_messageEdited = true;
}

void Otter::WriteEmailMessageWidget::on_inCopyLineEdit_textEdited(const QString &arg1)
{
    m_messageEdited = true;
}

void Otter::WriteEmailMessageWidget::on_inBlindCopyLineEdit_textEdited(const QString &arg1)
{
    m_messageEdited = true;
}

void Otter::WriteEmailMessageWidget::on_subjectLineEdit_textEdited(const QString &arg1)
{
    m_messageEdited = true;
}

void Otter::WriteEmailMessageWidget::on_messageContentTextEdit_textChanged()
{
    m_messageEdited = true;
}
