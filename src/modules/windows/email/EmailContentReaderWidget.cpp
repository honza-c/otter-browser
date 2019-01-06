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

namespace Otter
{

EmailContentReaderWidget::EmailContentReaderWidget(QWidget *parent) :
    QWidget(parent),
    m_ui(new Ui::EmailContentReaderWidget)
{
    m_ui->setupUi(this);

    setupTableModel();
    setupTableView();

    QWebEngineSettings *webViewSettings = m_ui->messageContentView->settings()->globalSettings();
    webViewSettings->setAttribute(QWebEngineSettings::WebAttribute::JavascriptEnabled, false);

    m_ui->blockRemoteContentWidget->setVisible(false);
    m_ui->blockRemoteContentLine->setVisible(false);
    m_ui->messageActionsAndInfoWidget->setVisible(false);
    m_ui->attachmentsScrollArea->setVisible(false);

    m_ui->blockRemoteContentLayout->setContentsMargins(0, 0, 0, 0);

    m_ui->splitter->setStretchFactor(0, 1);
    m_ui->splitter->setStretchFactor(1, 5);

    connect(m_ui->messageMetadataTableView->selectionModel(), SIGNAL(currentChanged(const QModelIndex &, const QModelIndex &)), this, SLOT(messageMetadataTableViewSelectionChanged(const QModelIndex &, const QModelIndex &)));

    for (UserAccount &account : EmailAccountsManager::getEmailAccounts())
    {
        connect(&account, SIGNAL(messageContentFetched(int)), this, SLOT(messageContentFetched(int)));
    }

    m_ui->moveToTrashButton->setIcon(ThemesManager::createIcon(QLatin1String("user-trash"), true));
    m_ui->junkButton->setIcon(ThemesManager::createIcon(QLatin1String("mail-mark-junk"), true));
    m_ui->replyAllButton->setIcon(ThemesManager::createIcon(QLatin1String("mail-reply-all"), true));
    m_ui->replyButton->setIcon(ThemesManager::createIcon(QLatin1String("mail-reply-sender"), true));
    m_ui->forwardButton->setIcon(ThemesManager::createIcon(QLatin1String("mail-forward"), true));

    m_ui->textBrowserWidget->hide();
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

    m_ui->messageMetadataTableView->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(m_ui->messageMetadataTableView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(messageMetadataTableViewContextMenuRequested(QPoint)));

    m_ui->messageMetadataTableView->horizontalHeader()->setStretchLastSection(false);

    m_ui->messageMetadataTableView->setColumnWidth(0, 70); // id
    m_ui->messageMetadataTableView->horizontalHeader()->setSectionResizeMode(11, QHeaderView::Stretch); // subject
    m_ui->messageMetadataTableView->setColumnWidth(9, 250); // sender
    m_ui->messageMetadataTableView->setColumnWidth(10, 100); // size
    m_ui->messageMetadataTableView->setColumnWidth(8, 150); // date

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


    if (selectionModel->hasSelection())
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

        QAction *deleteAction = new QAction("Delete Message");

        QObject::connect(deleteAction, SIGNAL(triggered(bool)), this, SLOT(deleteMessageActionTriggered(bool)));

        menu->addMenu(moveMessageMenu);
        menu->addMenu(copyMessageMenu);
        menu->addAction(deleteAction);

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
}

void EmailContentReaderWidget::setupBlockRemoteContentPanel(bool isHtmlMessage)
{
    if (isHtmlMessage)
    {
        m_ui->blockRemoteContentLine->setVisible(true);
        m_ui->blockRemoteContentWidget->setVisible(true);

        QWebEngineSettings *webViewSettings = m_ui->messageContentView->settings()->globalSettings();
        webViewSettings->setAttribute(QWebEngineSettings::WebAttribute::AutoLoadImages, false);
        m_ui->textBrowserWidget->setImagesPolicy(TextBrowserWidget::ImagesPolicy::NoImages);
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
            m_ui->messageContentView->setHtml(htmlContent);
            // m_ui->textBrowserWidget->setText(htmlContent);
            m_ui->textBrowserWidget->setHtml(htmlContent);
        }
        else if (plainTextContent != QString())
        {
            m_ui->messageContentView->setHtml(plainTextContent);
            // m_ui->textBrowserWidget->setText(plainTextContent);
            m_ui->textBrowserWidget->setPlainText(plainTextContent);
        }
        else
        {
            m_ui->messageContentView->setHtml(QString());
            m_ui->textBrowserWidget->setText(QString());
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
            file.open(QIODevice::WriteOnly);
            file.write(attachment.data());
            file.close();
        }
    }
}

void EmailContentReaderWidget::on_enableRemoteContentButton_clicked(bool)
{
    m_ui->blockRemoteContentLine->setVisible(false);
    m_ui->blockRemoteContentWidget->setVisible(false);

    QWebEngineSettings *webViewSettings = m_ui->messageContentView->settings()->globalSettings();
    webViewSettings->setAttribute(QWebEngineSettings::WebAttribute::AutoLoadImages, true);
    m_ui->textBrowserWidget->setImagesPolicy(TextBrowserWidget::ImagesPolicy::AllImages);

    m_ui->textBrowserWidget->reload();


    m_ui->messageContentView->reload();
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
    // TODO:
}

void EmailContentReaderWidget::on_junkButton_clicked()
{
    // TODO:
}

void EmailContentReaderWidget::on_archiveButton_clicked()
{
    // TODO:
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
    // TODO:
}

void EmailContentReaderWidget::copyMessageActionTriggered(bool)
{
    // TODO:
}

void EmailContentReaderWidget::deleteMessageActionTriggered(bool)
{
    // TODO:
}

}

