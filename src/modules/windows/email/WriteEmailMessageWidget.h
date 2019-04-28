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

#ifndef WRITEEMAILMESSAGEWIDGET_H
#define WRITEEMAILMESSAGEWIDGET_H

#include <QWidget>
#include <QtWidgets>

#include "../../../core/EmailAccountsManager.h"
#include "src/modules/mail/databasemanager.h"
#include "ui_WriteEmailMessageWidget.h"

#include "src/modules/mail/emailaccount.h"
#include "src/modules/mail/emailaccountslistmodel.h"
#include "src/modules/mail/attachment.h"
#include "src/modules/mail/attachmentslistmodel.h"
#include "src/modules/mail/message.h"
#include "src/modules/mail/contact.h"
#include "src/modules/mail/multiselectcompleter.h"

namespace Otter
{

namespace Ui {
    class WriteEmailMessageWidget;
}

class WriteEmailMessageWidget : public QWidget
{
    Q_OBJECT

public:
    enum Mode { WriteMessage, Reply, ReplyAll, Forward };

    explicit WriteEmailMessageWidget(QWidget *parent = nullptr);
    ~WriteEmailMessageWidget();

    void setMessage(Message message);
    void setSenderComboBoxEditable(bool value);
    void setMode(const Mode mode);

private slots:
    void on_backToInboxButton_clicked();
    void on_attachButton_clicked();
    void on_sendButton_clicked();
    void on_addAttachmentToolboxButton_clicked();
    void on_removeAttachmentToolboxButton_clicked();

    void on_toLineEdit_textEdited(const QString &arg1);

    void on_inCopyLineEdit_textEdited(const QString &arg1);

    void on_inBlindCopyLineEdit_textEdited(const QString &arg1);

    void on_subjectLineEdit_textEdited(const QString &arg1);

    void on_messageContentTextEdit_textChanged();

signals:
    void returnToInboxRequested();

private:
    void setSenderIndex(const Message message);
    void setSubject(const Message message);
    void setPlainTextAndHtmlText(const Message message);
    void setRecipients(const Message message);
    void setAttachments(const Message message);
    void setEmbeddedObjects(const Message message);

    void addAttachment();
    QList<Contact> parseContacts(const QString rawdata) const;
    QRegExp getEmailAddressRegexPattern() const;
    void setHtmlContent(const QString content);
    void setPlainTextContent(const QString content);
    void resetWidget();

    const QString dateTimeFormat = "dd.MM.yyyy HH:mm";

    bool m_messageEdited;
    QList<Attachment> m_attachments;
    QList<EmbeddedObject> m_embeddedObjects;
    Mode m_mode;
    Message m_message;
    MultiSelectCompleter *m_completer;
    Ui::WriteEmailMessageWidget *m_ui;
};

}

#endif // WRITEEMAILMESSAGEWIDGET_H
