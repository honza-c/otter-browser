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
#include "../../../core/ThemesManager.h"
#include "../../../core/Application.h"

namespace Otter
{

EmailContentReaderWidget::EmailContentReaderWidget(QWidget *parent) :
    QWidget(parent),
    m_ui(new Ui::EmailContentReaderWidget)
{
    m_ui->setupUi(this);

    setupTableModel();
    setupTableView();

    m_ui->blockRemoteContentWidget->setVisible(false);
    m_ui->blockRemoteContentLine->setVisible(false);
    m_ui->messageActionsAndInfoWidget->setVisible(false);
    m_ui->attachmentsScrollArea->setVisible(false);

    m_ui->blockRemoteContentLayout->setContentsMargins(0, 0, 0, 0);

    m_ui->splitter->setStretchFactor(0, 1);
    m_ui->splitter->setStretchFactor(1, 2);

    connect(m_ui->messageMetadataTableView->selectionModel(), SIGNAL(currentChanged(const QModelIndex &, const QModelIndex &)), this, SLOT(messageMetadataTableViewSelectionChanged(const QModelIndex &, const QModelIndex &)));
    connect(m_ui->messageMetadataTableView->horizontalHeader(), SIGNAL(sectionClicked(int)), m_messageMetadataTableModel, SLOT(onTableHeaderClicked(int)));

    for (EmailAccount &account : EmailAccountsManager::getEmailAccounts())
    {
        connect(&account, SIGNAL(messageContentFetched(int)), this, SLOT(messageContentFetched(int)));
    }

    m_ui->moveToTrashButton->setIcon(ThemesManager::createIcon(QLatin1String("user-trash"), true));
    m_ui->junkButton->setIcon(ThemesManager::createIcon(QLatin1String("mail-mark-junk"), true));
    m_ui->replyAllButton->setIcon(ThemesManager::createIcon(QLatin1String("mail-reply-all"), true));
    m_ui->replyButton->setIcon(ThemesManager::createIcon(QLatin1String("mail-reply-sender"), true));
    m_ui->forwardButton->setIcon(ThemesManager::createIcon(QLatin1String("mail-forward"), true));
    m_ui->archiveButton->setIcon(ThemesManager::createIcon(QLatin1String("folder-koperta"), true));
    m_ui->enableRemoteContentButton->setIcon(ThemesManager::createIcon(QLatin1String("view-information"), true));

    m_ui->messageContentWidget->setOpenLinks(false);
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

    m_ui->messageMetadataTableView->horizontalHeader()->moveSection(8, 1);
    m_ui->messageMetadataTableView->horizontalHeader()->moveSection(6, 15);

    m_ui->messageMetadataTableView->hideColumn(0);
    m_ui->messageMetadataTableView->hideColumn(1);
    m_ui->messageMetadataTableView->hideColumn(2);
    m_ui->messageMetadataTableView->hideColumn(3);
    m_ui->messageMetadataTableView->hideColumn(4);
    m_ui->messageMetadataTableView->hideColumn(9);
    m_ui->messageMetadataTableView->hideColumn(10);
    m_ui->messageMetadataTableView->hideColumn(11);
    m_ui->messageMetadataTableView->hideColumn(12);
    m_ui->messageMetadataTableView->hideColumn(13);
    m_ui->messageMetadataTableView->hideColumn(14);
    m_ui->messageMetadataTableView->hideColumn(15);

    m_ui->messageMetadataTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_ui->messageMetadataTableView->verticalHeader()->hide();
    m_ui->messageMetadataTableView->horizontalHeader()->moveSection(3, 0);

    m_ui->messageMetadataTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_ui->messageMetadataTableView->setSelectionMode(QTableView::SingleSelection);
    m_ui->messageMetadataTableView->setShowGrid(false);

    m_ui->messageMetadataTableView->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(m_ui->messageMetadataTableView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(messageMetadataTableViewContextMenuRequested(QPoint)));

    m_ui->messageMetadataTableView->horizontalHeader()->setStretchLastSection(false);

    m_ui->messageMetadataTableView->horizontalHeader()->setSectionResizeMode(8, QHeaderView::Stretch); // subject
    m_ui->messageMetadataTableView->setColumnWidth(6, 250); // sender
    m_ui->messageMetadataTableView->setColumnWidth(7, 100); // size
    m_ui->messageMetadataTableView->setColumnWidth(5, 150); // date

    while (m_messageMetadataTableModel->canFetchMore())
    {
        m_messageMetadataTableModel->fetchMore();
    }

    m_ui->messageMetadataTableView->scrollToBottom();
}

void EmailContentReaderWidget::messageMetadataTableViewContextMenuRequested(QPoint position)
{
    QItemSelectionModel *selectionModel = m_ui->messageMetadataTableView->selectionModel();
    MessageMetadataSqlTableModel *model = static_cast<MessageMetadataSqlTableModel*>(m_ui->messageMetadataTableView->model());
    QModelIndex indexUnderCursor = m_ui->messageMetadataTableView->indexAt(position);

    if (indexUnderCursor.isValid())
    {
        QMenu *menu = new QMenu();

        QModelIndex index = selectionModel->selectedRows().at(0);
        int folderId = model->data(QModelIndex(index.sibling(index.row(), 1)), Qt::DisplayRole).toInt();
        QString emailAddress = DatabaseManager::getEmailAddress(folderId);

        QList<InboxFolder> folders = DatabaseManager::getInboxFolders(emailAddress);


        QMenu *moveMessageMenu = new QMenu("Move to");

        for (InboxFolder folder : folders)
        {
            if (folder.path() != "/")
            {
                QAction *action = new QAction(folder.path());
                action->setIcon(folder.getIcon());
                moveMessageMenu->addAction(action);

                connect(action, SIGNAL(triggered(bool)), this, SLOT(moveMessageActionTriggered(bool)));
            }
        }

        QMenu *copyMessageMenu = new QMenu("Copy to");

        for (InboxFolder folder : folders)
        {
            if (folder.path() != "/")
            {
                QAction *action = new QAction(folder.path());
                action->setIcon(folder.getIcon());
                copyMessageMenu->addAction(action);

                QObject::connect(action, SIGNAL(triggered(bool)), this, SLOT(copyMessageActionTriggered(bool)));
            }
        }



        QString markAsReadText;

        if (model->data(QModelIndex(index.sibling(index.row(), 3)), Qt::DisplayRole).toBool())
        {
            markAsReadText = "Mark as unseen";
        }
        else
        {
            markAsReadText = "Mark as seen";
        }

        QAction *markAsReadAction = new QAction(markAsReadText);

        QObject::connect(markAsReadAction, SIGNAL(triggered(bool)), this, SLOT(markAsReadActionTriggered(bool)));

        QAction *deleteAction = new QAction("Delete Message");

        QObject::connect(deleteAction, SIGNAL(triggered(bool)), this, SLOT(deleteMessageActionTriggered(bool)));

        menu->addMenu(moveMessageMenu);
        menu->addMenu(copyMessageMenu);
        menu->addAction(deleteAction);
        menu->addAction(markAsReadAction);

        menu->popup(m_ui->messageMetadataTableView->viewport()->mapToGlobal(position));
    }
}

void EmailContentReaderWidget::messageMetadataTableViewSelectionChanged(const QModelIndex &current, const QModelIndex &)
{
    if (current.isValid())
    {
        int messageId = m_messageMetadataTableModel->data(current.sibling(current.row(), 0), Qt::DisplayRole).toInt();
        QString htmlContent = DatabaseManager::getHtmlContent(messageId);
        QString plainTextContent = DatabaseManager::getTextContent(messageId);

        int folderId = m_messageMetadataTableModel->data(current.sibling(current.row(), 1), Qt::DisplayRole).toInt();
        int positionInFolder = DatabaseManager::getPositionInFolder(messageId);

        QString folderPath = DatabaseManager::getFolderPath(folderId);
        QString emailAddress = DatabaseManager::getEmailAddress(folderId);

        if (DatabaseManager::hasTheAccountTrashFolder(emailAddress))
        {
            if (DatabaseManager::isFolderTrash(folderId))
            {
                m_ui->moveToTrashButton->setText("Delete message");
            }
            else
            {
                m_ui->moveToTrashButton->setText("Move to trash");
            }
        }
        else
        {
            m_ui->moveToTrashButton->setText("Delete message");
        }


        if (DatabaseManager::hasTheAccountJunkFolder(emailAddress))
        {
            m_ui->junkButton->setVisible(true);

            if (DatabaseManager::isFolderJunk(folderId))
            {
                m_ui->junkButton->setText("Not junk");
            }
            else
            {
                m_ui->junkButton->setText("Junk");
            }
        }
        else
        {
            m_ui->junkButton->setVisible(false);
        }

        if (DatabaseManager::hasTheAccountArchiveFolder(emailAddress))
        {
            if (DatabaseManager::isFolderArchive(folderId))
            {
                m_ui->archiveButton->setText("Unarchive");
            }
            else
            {
                m_ui->archiveButton->setText("Archive");
            }
        }
        else
        {
            m_ui->archiveButton->setVisible(false);
        }

        if (htmlContent == QString() && plainTextContent == QString())
        {
            if (messageId > 0)
            {
                if (positionInFolder > 0)
                {
                    m_ui->messageContentWidget->setText(QString("Downloading message from the server..."));

                    for (EmailAccount &userAccount : EmailAccountsManager::getEmailAccounts())
                    {
                        if (userAccount.emailAddress() == emailAddress)
                        {
                            userAccount.fetchMessageContent(folderPath, positionInFolder);
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
}

void EmailContentReaderWidget::setupBlockRemoteContentPanel(bool isHtmlMessage)
{
    if (isHtmlMessage)
    {
        m_ui->blockRemoteContentLine->setVisible(true);
        m_ui->blockRemoteContentWidget->setVisible(true);

        m_ui->messageContentWidget->setImagesPolicy(TextBrowserWidget::ImagesPolicy::NoImages);
    }
    else
    {
        m_ui->blockRemoteContentLine->setVisible(false);
        m_ui->blockRemoteContentWidget->setVisible(false);
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
        QList<Attachment> attachments = DatabaseManager::getAttachments(messageId);

        setupBlockRemoteContentPanel(!htmlContent.isEmpty());

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
            m_ui->messageContentWidget->setHtml(htmlContent);
        }
        else if (plainTextContent != QString())
        {
            m_ui->messageContentWidget->setPlainText(plainTextContent);
        }
        else
        {
            m_ui->messageContentWidget->setText(QString());
        }

        if (!replyTo.isEmpty() || !copyRecipients.isEmpty())
        {
            m_ui->replyAllButton->setVisible(true);
        }
        else
        {
            m_ui->replyAllButton->setVisible(false);
        }

        m_ui->messageActionsAndInfoWidget->setVisible(true);
        setupAttachmentsPanel(attachments);

        QModelIndex index = m_ui->messageMetadataTableView->selectionModel()->currentIndex();

        int folderId = m_messageMetadataTableModel->data(index.sibling(index.row(), 1), Qt::DisplayRole).toInt();
        int uid = m_messageMetadataTableModel->data(index.sibling(index.row(), 2), Qt::DisplayRole).toInt();
        QString emailAddress = DatabaseManager::getEmailAddress(folderId);
        bool isSeen = m_messageMetadataTableModel->data(index.sibling(index.row(), 3), Qt::DisplayRole).toBool();

        if (!isSeen)
        {
            for (EmailAccount &account : EmailAccountsManager::getEmailAccounts())
            {
                if (account.emailAddress() == emailAddress)
                {
                    account.setMessageAsSeen(uid);
                }
            }
        }
    }
}

void EmailContentReaderWidget::setupAttachmentsPanel(QList<Attachment> attachments)
{
    if (attachments.size() > 0)
    {
        QLayout *oldLayout = m_ui->attachmentsWidget->layout();
        deleteOldAttachmentsLayout(oldLayout);
        m_ui->attachmentsWidget->setLayout(createAttachmentsLayout("<b>Attachments:</b>", attachments));

        if (m_ui->attachmentsWidget->layout()->count() > 1)
        {
            m_ui->attachmentsScrollArea->show();
        }
        else
        {
            m_ui->attachmentsScrollArea->hide();
        }
    }
    else
    {
        m_ui->attachmentsScrollArea->hide();
    }
}

void EmailContentReaderWidget::deleteOldAttachmentsLayout(QLayout *layout)
{
    if (layout != nullptr)
    {
        QLayoutItem *item;
        QWidget *widget;

        while ((item = layout->takeAt(0)) != nullptr)
        {
            if ((widget = item->widget()) != nullptr)
            {
                widget->hide();
                delete widget;
            }
        }
    }

    delete layout;
}

QHBoxLayout* EmailContentReaderWidget::createAttachmentsLayout(QString caption, QList<Attachment> attachments)
{
    QHBoxLayout *layout = new QHBoxLayout();
    QLabel *attachmentsLabel = new QLabel(caption);
    layout->addWidget(attachmentsLabel);

    for (int i = 0; i < attachments.size(); i++)
    {
        QString name = attachments.at(i).name();

        if (name != QString())
        {
            QPushButtonWithId *button = new QPushButtonWithId(attachments.at(i).name());
            button->setId(i);
            button->setIcon(ThemesManager::createIcon(QLatin1String("mail-attachment"), true));

            QObject::connect(button, SIGNAL(clicked(bool)), this, SLOT(onAttachmentButtonClicked(bool)));

            layout->addWidget(button);
        }
    }

    layout->setAlignment(Qt::AlignLeft);
    layout->setContentsMargins(0, 0, 0, 0);

    return layout;
}

void EmailContentReaderWidget::onAttachmentButtonClicked(bool)
{
    QPushButtonWithId *button = static_cast<QPushButtonWithId*>(QObject::sender());
    QModelIndex index = m_ui->messageMetadataTableView->currentIndex();
    int messageId = m_ui->messageMetadataTableView->model()->data(QModelIndex(index.sibling(index.row(), 0)), Qt::DisplayRole).toInt();
    Attachment attachment = DatabaseManager::getAttachments(messageId).at(button->id());

    QFileDialog dialog;

    QString defaultPath = QStandardPaths::standardLocations(QStandardPaths::DownloadLocation).first();
    dialog.setDirectory(defaultPath);
    dialog.setWindowTitle("Save attachment");
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    dialog.selectFile(attachment.name());
    dialog.setMimeTypeFilters(QStringList(attachment.mimeType()));

    if (dialog.exec() == QDialog::Accepted)
    {
        QString fileName = dialog.selectedFiles().first();

        if (fileName != QString())
        {
            QFile file(fileName);

            if (file.open(QIODevice::WriteOnly))
            {
                long size = file.write(attachment.data());

                if (size == -1)
                {
                    QMessageBox messageBox;

                    messageBox.setWindowTitle("Could save attachment");
                    messageBox.setText("Failed to save file: " + fileName);
                    messageBox.setIcon(QMessageBox::Critical);
                    messageBox.addButton(new QPushButton("Close"), QMessageBox::ButtonRole::AcceptRole);

                    messageBox.exec();
                }
                file.close();
            }
            else
            {
                QMessageBox messageBox;

                messageBox.setWindowTitle("Could save attachment");
                messageBox.setText("Failed to save file: " + fileName);
                messageBox.setIcon(QMessageBox::Critical);
                messageBox.addButton(new QPushButton("Close"), QMessageBox::ButtonRole::AcceptRole);

                messageBox.exec();
            }
        }
    }
}

void EmailContentReaderWidget::on_enableRemoteContentButton_clicked(bool)
{
    m_ui->blockRemoteContentLine->setVisible(false);
    m_ui->blockRemoteContentWidget->setVisible(false);

    m_ui->messageContentWidget->setImagesPolicy(TextBrowserWidget::ImagesPolicy::AllImages);
    m_ui->messageContentWidget->reload();
}

void EmailContentReaderWidget::on_filterMessagesEdit_textChanged(const QString &input)
{
    updateMessageMetadataTableFilter(m_currentInboxFolderTreeIndex, input);
}

void EmailContentReaderWidget::updateMessageMetadataTableFilter(QModelIndex currentIndex, QString filterText)
{
    QString emailAddress = getEmailAddressFromFolderTreeItemIndex(currentIndex);
    QString folderPath = getFolderPathFromFolderTreeItemIndex(currentIndex);
    int folderId = DatabaseManager::getFolderId(emailAddress, folderPath);

    MessageMetadataSqlTableModel *tableModel = static_cast<MessageMetadataSqlTableModel*>(m_ui->messageMetadataTableView->model());

    if (folderId > 0)
    {
        if (filterText == QString())
        {
            tableModel->setFilter(QString("folderId = %1").arg(folderId));
            tableModel->select();
        }
        else
        {
            tableModel->setFilter(QString("folderId = %1 AND (sender LIKE '%%2%' OR subject LIKE '%%2%' OR plainTextContent LIKE '%%2%' OR htmlContent LIKE '%%2%')")
                                       .arg(folderId)
                                       .arg(filterText));
            tableModel->select();
        }
    }

    while (tableModel->canFetchMore())
    {
        tableModel->fetchMore();
    }

    m_ui->messageMetadataTableView->scrollToBottom();
}

QString EmailContentReaderWidget::getEmailAddressFromFolderTreeItemIndex(QModelIndex currentIndex)
{
    QString path = getFullFolderPathFromFolderTreeItemIndex(currentIndex);
    return path.split("/")[0];
}

QString EmailContentReaderWidget::getFullFolderPathFromFolderTreeItemIndex(QModelIndex currentIndex)
{
    InboxFolderTreeItem* item = static_cast<InboxFolderTreeItem*>(currentIndex.internalPointer());

    if (item != nullptr)
    {
        QString path = item->data(0).toString();
        QModelIndex index = currentIndex;

        while(index.parent() != QModelIndex())
        {
            index = index.parent();
            item = static_cast<InboxFolderTreeItem*>(index.internalPointer());
            path = item->data(0).toString() + "/" + path;
        }

        return path;
    }
    else
    {
        return QString();
    }
}

QString EmailContentReaderWidget::getFolderPathFromFolderTreeItemIndex(QModelIndex currentIndex)
{
    QString path = getFullFolderPathFromFolderTreeItemIndex(currentIndex);
    QString emailAddress = getEmailAddressFromFolderTreeItemIndex(currentIndex);

    return path.right(path.length() - emailAddress.length());
}

void EmailContentReaderWidget::selectedInboxFolderTreeIndexChanged(const QModelIndex &currentIndex, const QModelIndex &)
{
    m_currentInboxFolderTreeIndex = currentIndex;
    updateMessageMetadataTableFilter(currentIndex, m_ui->filterMessagesEdit->text());
}

void EmailContentReaderWidget::messageContentFetched(int messageId)
{
    QItemSelectionModel *selectionModel = m_ui->messageMetadataTableView->selectionModel();

    if (selectionModel->hasSelection())
    {
        QModelIndex index = selectionModel->selectedRows().at(0);
        int currentMessageId = m_ui->messageMetadataTableView->model()->data(QModelIndex(index.sibling(index.row(), 0)), Qt::DisplayRole).toInt();

        if (messageId == currentMessageId)
        {
            showMessageContent(messageId);
        }
    }
}

void EmailContentReaderWidget::on_moveToTrashButton_clicked()
{
    QPushButton *button = static_cast<QPushButton*>(sender());

    QItemSelectionModel *selectionModel = m_ui->messageMetadataTableView->selectionModel();

    if (selectionModel->hasSelection())
    {
        QModelIndex index = selectionModel->selectedRows().at(0);

        if (button->text() == "Move to trash")
        {
            int uid = m_messageMetadataTableModel->data(QModelIndex(index.sibling(index.row(), 2)), Qt::DisplayRole).toInt();
            int folderId = m_messageMetadataTableModel->data(QModelIndex(index.sibling(index.row(), 1)), Qt::DisplayRole).toInt();

            QString emailAddress = DatabaseManager::getEmailAddress(folderId);
            QString currentPath = DatabaseManager::getFolderPath(folderId);

            QString trashFolderPath = DatabaseManager::getTrashFolderPath(emailAddress);

            for (EmailAccount &account : EmailAccountsManager::getEmailAccounts())
            {
                if (account.emailAddress() == emailAddress)
                {
                    account.moveMessage(uid, currentPath, trashFolderPath);
                }
            }
        }
        else if (button->text() == "Delete message")
        {
            QMessageBox messageBox;

            messageBox.setWindowTitle("Delete the message");
            messageBox.setText("Are you sure to delete the selected message?");
            messageBox.setIcon(QMessageBox::Question);
            messageBox.addButton(new QPushButton("Cancel"), QMessageBox::ButtonRole::RejectRole);
            messageBox.addButton(new QPushButton("Delete Message"), QMessageBox::ButtonRole::AcceptRole);

            if (messageBox.exec())
            {
                int folderId = m_messageMetadataTableModel->data(QModelIndex(index.sibling(index.row(), 1)), Qt::DisplayRole).toInt();
                int uid = m_messageMetadataTableModel->data(QModelIndex(index.sibling(index.row(), 2)), Qt::DisplayRole).toInt();
                QString emailAddress = DatabaseManager::getEmailAddress(folderId);

                for (EmailAccount &account : EmailAccountsManager::getEmailAccounts())
                {
                    if (account.emailAddress() == emailAddress)
                    {
                        account.deleteMessage(uid, folderId);
                    }
                }
            }
        }
    }
}

void EmailContentReaderWidget::on_junkButton_clicked()
{
    QPushButton *button = static_cast<QPushButton*>(sender());

    QItemSelectionModel *selectionModel = m_ui->messageMetadataTableView->selectionModel();

    if (selectionModel->hasSelection())
    {
        QModelIndex index = selectionModel->selectedRows().at(0);

        if (button->text() == "Junk")
        {
            int uid = m_messageMetadataTableModel->data(QModelIndex(index.sibling(index.row(), 2)), Qt::DisplayRole).toInt();
            int folderId = m_messageMetadataTableModel->data(QModelIndex(index.sibling(index.row(), 1)), Qt::DisplayRole).toInt();

            QString emailAddress = DatabaseManager::getEmailAddress(folderId);
            QString currentPath = DatabaseManager::getFolderPath(folderId);

            QString junkFolderPath = DatabaseManager::getJunkFolderPath(emailAddress);

            for (EmailAccount &account : EmailAccountsManager::getEmailAccounts())
            {
                if (account.emailAddress() == emailAddress)
                {
                    account.moveMessage(uid, currentPath, junkFolderPath);
                }
            }
        }
        else if (button->text() == "Not junk")
        {
            int uid = m_messageMetadataTableModel->data(QModelIndex(index.sibling(index.row(), 2)), Qt::DisplayRole).toInt();
            int folderId = m_messageMetadataTableModel->data(QModelIndex(index.sibling(index.row(), 1)), Qt::DisplayRole).toInt();

            QString emailAddress = DatabaseManager::getEmailAddress(folderId);
            QString currentPath = DatabaseManager::getFolderPath(folderId);

            QString defaultFolderPath = "/INBOX";

            for (EmailAccount &account : EmailAccountsManager::getEmailAccounts())
            {
                if (account.emailAddress() == emailAddress)
                {
                    account.moveMessage(uid, currentPath, defaultFolderPath);
                }
            }
        }
    }
}

void EmailContentReaderWidget::on_archiveButton_clicked()
{
    QPushButton *button = static_cast<QPushButton*>(sender());

    QItemSelectionModel *selectionModel = m_ui->messageMetadataTableView->selectionModel();

    if (selectionModel->hasSelection())
    {
        QModelIndex index = selectionModel->selectedRows().at(0);

        if (button->text() == "Archive")
        {
            int uid = m_messageMetadataTableModel->data(QModelIndex(index.sibling(index.row(), 2)), Qt::DisplayRole).toInt();
            int folderId = m_messageMetadataTableModel->data(QModelIndex(index.sibling(index.row(), 1)), Qt::DisplayRole).toInt();

            QString emailAddress = DatabaseManager::getEmailAddress(folderId);
            QString currentPath = DatabaseManager::getFolderPath(folderId);

            QString archiveFolderPath = DatabaseManager::getArchiveFolderPath(emailAddress);

            for (EmailAccount &account : EmailAccountsManager::getEmailAccounts())
            {
                if (account.emailAddress() == emailAddress)
                {
                    account.moveMessage(uid, currentPath, archiveFolderPath);
                }
            }
        }
        else if (button->text() == "Unarchive")
        {
            int uid = m_messageMetadataTableModel->data(QModelIndex(index.sibling(index.row(), 2)), Qt::DisplayRole).toInt();
            int folderId = m_messageMetadataTableModel->data(QModelIndex(index.sibling(index.row(), 1)), Qt::DisplayRole).toInt();

            QString emailAddress = DatabaseManager::getEmailAddress(folderId);
            QString currentPath = DatabaseManager::getFolderPath(folderId);

            QString defaultFolderPath = "/INBOX";

            for (EmailAccount &account : EmailAccountsManager::getEmailAccounts())
            {
                if (account.emailAddress() == emailAddress)
                {
                    account.moveMessage(uid, currentPath, defaultFolderPath);
                }
            }
        }
    }
}

void EmailContentReaderWidget::on_replyAllButton_clicked()
{
    QItemSelectionModel *selectionModel = m_ui->messageMetadataTableView->selectionModel();

    if (selectionModel->hasSelection())
    {
        QModelIndex index = selectionModel->selectedRows().at(0);
        int messageId = m_messageMetadataTableModel->data(QModelIndex(index.sibling(index.row(), 0)), Qt::DisplayRole).toInt();
        emit replyOrForwardMessageRequested(WriteEmailMessageWidget::Mode::ReplyAll, messageId);
    }
}

void EmailContentReaderWidget::on_replyButton_clicked()
{
    QItemSelectionModel *selectionModel = m_ui->messageMetadataTableView->selectionModel();

    if (selectionModel->hasSelection())
    {
        QModelIndex index = selectionModel->selectedRows().at(0);
        int messageId = m_messageMetadataTableModel->data(QModelIndex(index.sibling(index.row(), 0)), Qt::DisplayRole).toInt();
        emit replyOrForwardMessageRequested(WriteEmailMessageWidget::Mode::Reply, messageId);
    }
}

void EmailContentReaderWidget::on_forwardButton_clicked()
{
    QItemSelectionModel *selectionModel = m_ui->messageMetadataTableView->selectionModel();

    if (selectionModel->hasSelection())
    {
        QModelIndex index = selectionModel->selectedRows().at(0);
        int messageId = m_messageMetadataTableModel->data(QModelIndex(index.sibling(index.row(), 0)), Qt::DisplayRole).toInt();
        emit replyOrForwardMessageRequested(WriteEmailMessageWidget::Mode::Forward, messageId);
    }
}

void EmailContentReaderWidget::moveMessageActionTriggered(bool)
{
    QAction *action = static_cast<QAction*>(QObject::sender());
    QString newPath = action->text();

    QItemSelectionModel *selectionModel = m_ui->messageMetadataTableView->selectionModel();
    QModelIndex index = selectionModel->selectedRows().at(0);

    int uid = m_messageMetadataTableModel->data(QModelIndex(index.sibling(index.row(), 2)), Qt::DisplayRole).toInt();
    int folderId = m_messageMetadataTableModel->data(QModelIndex(index.sibling(index.row(), 1)), Qt::DisplayRole).toInt();

    QString emailAddress = DatabaseManager::getEmailAddress(folderId);
    QString currentPath = DatabaseManager::getFolderPath(folderId);

    for (EmailAccount &account : EmailAccountsManager::getEmailAccounts())
    {
        if (account.emailAddress() == emailAddress)
        {
            account.moveMessage(uid, currentPath, newPath);
        }
    }
}

void EmailContentReaderWidget::copyMessageActionTriggered(bool)
{
    QAction *action = static_cast<QAction*>(QObject::sender());
    QString newPath = action->text();

    QItemSelectionModel *selectionModel = m_ui->messageMetadataTableView->selectionModel();
    QModelIndex index = selectionModel->selectedRows().at(0);

    int uid = m_messageMetadataTableModel->data(QModelIndex(index.sibling(index.row(), 2)), Qt::DisplayRole).toInt();
    int folderId = m_messageMetadataTableModel->data(QModelIndex(index.sibling(index.row(), 1)), Qt::DisplayRole).toInt();

    QString emailAddress = DatabaseManager::getEmailAddress(folderId);
    QString currentPath = DatabaseManager::getFolderPath(folderId);

    for (EmailAccount &account : EmailAccountsManager::getEmailAccounts())
    {
        if (account.emailAddress() == emailAddress)
        {
            account.copyMessage(uid, currentPath, newPath);
        }
    }
}

void EmailContentReaderWidget::markAsReadActionTriggered(bool)
{
    QAction *action = static_cast<QAction*>(QObject::sender());

    QItemSelectionModel *selectionModel = m_ui->messageMetadataTableView->selectionModel();
    QModelIndex index = selectionModel->selectedRows().at(0);

    int uid = m_messageMetadataTableModel->data(QModelIndex(index.sibling(index.row(), 2)), Qt::DisplayRole).toInt();
    int folderId = m_messageMetadataTableModel->data(QModelIndex(index.sibling(index.row(), 1)), Qt::DisplayRole).toInt();

    QString emailAddress = DatabaseManager::getEmailAddress(folderId);

    for (EmailAccount &account : EmailAccountsManager::getEmailAccounts())
    {
        if (account.emailAddress() == emailAddress)
        {
            if (action->text() == "Mark as seen")
            {
                account.setMessageAsSeen(uid);
            }
            else
            {
                account.setMessageAsUnseen(uid);
            }
        }
    }
}

void EmailContentReaderWidget::deleteMessageActionTriggered(bool)
{
    QItemSelectionModel *selectionModel = m_ui->messageMetadataTableView->selectionModel();

    if (selectionModel->hasSelection())
    {
        QMessageBox messageBox;

        messageBox.setWindowTitle("Delete the message");
        messageBox.setText("Are you sure to delete the selected message?");
        messageBox.setIcon(QMessageBox::Question);
        messageBox.addButton(new QPushButton("Cancel"), QMessageBox::ButtonRole::RejectRole);
        messageBox.addButton(new QPushButton("Delete Message"), QMessageBox::ButtonRole::AcceptRole);

        if (messageBox.exec())
        {
            QModelIndex index = selectionModel->selectedRows().at(0);

            int folderId = m_messageMetadataTableModel->data(QModelIndex(index.sibling(index.row(), 1)), Qt::DisplayRole).toInt();
            int uid = m_messageMetadataTableModel->data(QModelIndex(index.sibling(index.row(), 2)), Qt::DisplayRole).toInt();
            QString emailAddress = DatabaseManager::getEmailAddress(folderId);

            for (EmailAccount &account : EmailAccountsManager::getEmailAccounts())
            {
                if (account.emailAddress() == emailAddress)
                {
                    account.deleteMessage(uid, folderId);
                }
            }
        }
    }
}

void EmailContentReaderWidget::messagesMetadataStructureChanged()
{
    m_messageMetadataTableModel->select();

    while (m_messageMetadataTableModel->canFetchMore())
    {
        m_messageMetadataTableModel->fetchMore();
    }

    m_ui->messageMetadataTableView->scrollToBottom();
}

}

void Otter::EmailContentReaderWidget::on_messageContentWidget_anchorClicked(const QUrl &arg1)
{
    Application::triggerAction(ActionsManager::OpenUrlAction, {{QLatin1String("url"), arg1.url()}}, this);
}
