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

#include "EmailContentReaderWidget.h"
#include "ui_EmailContentReaderWidget.h"

namespace Otter
{

EmailContentReaderWidget::EmailContentReaderWidget(QWidget *parent) :
    QWidget(parent),
    m_ui(new Ui::EmailContentReaderWidget)
{
    m_ui->setupUi(this);

    setupTableModel();
    setupTableView();

    connect(m_ui->messageMetadataTableView->selectionModel(), SIGNAL(currentChanged(const QModelIndex &, const QModelIndex &)), this, SLOT(messageMetadataTableViewSelectionChanged(const QModelIndex &, const QModelIndex &)));
}

EmailContentReaderWidget::~EmailContentReaderWidget()
{
    delete m_ui;
}

void EmailContentReaderWidget::setupTableModel()
{
    m_messageMetadataTableModel = new MessageMetadataSqlTableModel();
    m_messageMetadataTableModel->setTable("MessageData");
    m_messageMetadataTableModel->select();
}

void EmailContentReaderWidget::setupTableView()
{
    m_ui->messageMetadataTableView->setModel(m_messageMetadataTableModel);

    m_ui->messageMetadataTableView->horizontalHeader()->moveSection(11, 0);
    m_ui->messageMetadataTableView->horizontalHeader()->moveSection(10, 1);
    m_ui->messageMetadataTableView->horizontalHeader()->moveSection(11, 2);

    m_ui->messageMetadataTableView->hideColumn(1);
    m_ui->messageMetadataTableView->hideColumn(2);
    m_ui->messageMetadataTableView->hideColumn(3);
    m_ui->messageMetadataTableView->hideColumn(4);
    m_ui->messageMetadataTableView->hideColumn(5);
    m_ui->messageMetadataTableView->hideColumn(6);
    m_ui->messageMetadataTableView->hideColumn(7);
    m_ui->messageMetadataTableView->hideColumn(12);
    m_ui->messageMetadataTableView->hideColumn(13);
    m_ui->messageMetadataTableView->hideColumn(14);
    m_ui->messageMetadataTableView->hideColumn(15);
    m_ui->messageMetadataTableView->hideColumn(16);
    m_ui->messageMetadataTableView->hideColumn(17);
    m_ui->messageMetadataTableView->hideColumn(18);
    m_ui->messageMetadataTableView->hideColumn(19);

    m_ui->messageMetadataTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_ui->messageMetadataTableView->verticalHeader()->hide();
    m_ui->messageMetadataTableView->horizontalHeader()->moveSection(3, 0);

    m_ui->messageMetadataTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_ui->messageMetadataTableView->setSelectionMode(QTableView::SingleSelection);
    m_ui->messageMetadataTableView->setShowGrid(false);


    m_ui->messageMetadataTableView->horizontalHeader()->setStretchLastSection(false);

    m_ui->messageMetadataTableView->setColumnWidth(0, 70); // id
    m_ui->messageMetadataTableView->horizontalHeader()->setSectionResizeMode(11, QHeaderView::Stretch); // subject
    m_ui->messageMetadataTableView->setColumnWidth(9, 250); // sender
    m_ui->messageMetadataTableView->setColumnWidth(10, 100); // size
    m_ui->messageMetadataTableView->setColumnWidth(8, 150); // date

    m_messageMetadataTableModel->setFilter("folderId = 5");

    while (m_messageMetadataTableModel->canFetchMore())
    {
        m_messageMetadataTableModel->fetchMore();
    }

    m_ui->messageMetadataTableView->scrollToBottom();

    m_ui->attachmentsScrollArea->setVisible(false);
}

void EmailContentReaderWidget::messageMetadataTableViewSelectionChanged(const QModelIndex &current, const QModelIndex &)
{
    int messageId = m_messageMetadataTableModel->data(current.sibling(current.row(), 0), Qt::DisplayRole).toInt();
    QString htmlContent = DatabaseManager::getHtmlContent(messageId);
    QString plainTextContent = DatabaseManager::getTextContent(messageId);

    if (htmlContent == QString() && plainTextContent == QString())
    {
        if (messageId > 0)
        {
            int folderId = m_messageMetadataTableModel->data(current.sibling(current.row(), 1), Qt::DisplayRole).toInt();

            QString folderPath = DatabaseManager::getFolderPath(folderId);
            QString emailAddress = DatabaseManager::getEmailAddress(folderId);

            int positionInFolder = DatabaseManager::getPositionInFolder(messageId);

            if (positionInFolder > 0)
            {
                for (UserAccount &userAccount : EmailAccountsManager::getEmailAccounts())
                {
                    if (userAccount.emailAddress() == emailAddress)
                    {
                        userAccount.fetchMissingMessageContent(folderPath, positionInFolder);
                    }
                }
            }
        }
    }
    else
    {
        showMessageContent(messageId);
    }
}

void EmailContentReaderWidget::showMessageContent(int messageId)
{
    if (messageId > 0)
    {
        QString subject = DatabaseManager::getSubject(messageId);

        QString htmlContent = DatabaseManager::getHtmlContent(messageId);
        QString plainTextContent = DatabaseManager::getTextContent(messageId);
        QList<EmbeddedObject> embeddedObjects = DatabaseManager::getEmbeddedObjects(messageId);

        if (!embeddedObjects.isEmpty())
        {
            for (EmbeddedObject embeddedObject : embeddedObjects)
            {
                QString srcOld = "src=\"cid:" + embeddedObject.name() + "\"";
                QString srcNew = "src=\"data:" + embeddedObject.mimeType() + ";base64," + embeddedObject.data().toBase64() + "\"";

                htmlContent.replace(srcOld, srcNew);
            }
        }

        QList<Contact> recipients = DatabaseManager::getRecipients(messageId);
        QList<Contact> copyRecipients = DatabaseManager::getCopyRecipients(messageId);
        QList<Contact> replyTo = DatabaseManager::getReplyTo(messageId);

        Contact from = DatabaseManager::getSender(messageId);

        m_ui->subjectLabel->setText("<b>Subject:</b> " + subject);

        m_ui->fromLabel->setText("<b>From:</b> "
                           + Contact::toString(from)
                           .replace("<", "&lt;")
                           .replace(">", "&gt;"));

        m_ui->toLabel->setText("<b>To:</b> "
                         + Contact::toString(recipients)
                         .replace("<", "&lt;")
                         .replace(">", "&gt;"));

        if (replyTo.isEmpty())
        {
            m_ui->replyToLabel->setVisible(false);
        }
        else
        {
            m_ui->replyToLabel->setVisible(true);
            m_ui->replyToLabel->setText("<b>Reply to:</b> "
                                  + Contact::toString(replyTo)
                                  .replace("<", "&lt;")
                                  .replace(">", "&gt;"));
        }

        if (copyRecipients.isEmpty())
        {
            m_ui->inCopyLabel->setVisible(false);
        }
        else
        {
            m_ui->inCopyLabel->setVisible(true);
            m_ui->inCopyLabel->setText("<b>In copy:</b> "
                                 + Contact::toString(copyRecipients)
                                 .replace("<", "&lt;")
                                 .replace(">", "&gt;"));
        }

        if (htmlContent != QString())
        {
            m_ui->messageContentView->setHtml(htmlContent);
        }
        else if (plainTextContent != QString())
        {
            m_ui->messageContentView->setHtml(plainTextContent);
        }
        else
        {
            m_ui->messageContentView->setHtml(QString());
        }

        if (!replyTo.isEmpty() || !copyRecipients.isEmpty())
        {
            m_ui->replyAllButton->setVisible(true);
        }
        else
        {
            m_ui->replyAllButton->setVisible(false);
        }
    }
}

}
