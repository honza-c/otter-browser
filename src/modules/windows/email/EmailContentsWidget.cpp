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

#include "EmailContentsWidget.h"
#include "../../../core/ThemesManager.h"
#include "../../../ui/Action.h"
#include "../../../ui/MainWindow.h"
#include "../../../ui/Window.h"

#include "ui_EmailContentsWidget.h"

#include <QtWidgets/QDesktopWidget>
#include <QtWidgets/QToolTip>

namespace Otter
{

EmailContentsWidget::EmailContentsWidget(const QVariantMap &parameters, Window *window, QWidget *parent) :ContentsWidget (parameters, nullptr, parent),
    m_window(window),
    m_ui(new Ui::EmailContentsWidget)
{
    m_ui->setupUi(this);

    const MainWindow *mainWindow(MainWindow::findMainWindow(parentWidget()));

    if (mainWindow)
    {
        m_window = mainWindow->getActiveWindow();
    }

    m_ui->inboxFoldersTreeView->setModel(new InboxFolderTreeModel(DatabaseManager::getInboxFolders()));
    m_ui->inboxFoldersTreeView->setViewMode(ItemViewWidget::TreeView);
    m_ui->inboxFoldersTreeView->expandAll();

    m_ui->getMessagesButton->setIcon(ThemesManager::createIcon(QLatin1String("mail-send-receive"), false));
    m_ui->writeMessageButton->setIcon(ThemesManager::createIcon(QLatin1String("mail-send"), false));

    connect(DatabaseManager::getInstance(), SIGNAL(inboxFoldersStructureChanged()), this, SLOT(inboxFoldersStructureChanged()));

    if (isSidebarPanel())
    {
        m_ui->emailTabWidget->setVisible(false);
    }
    else
    {
        m_ui->writeEmailWidget->setVisible(false);

        m_ui->splitter->setStretchFactor(0, 1);
        m_ui->splitter->setStretchFactor(1, 5);

        m_ui->inboxFoldersTreeView->setContextMenuPolicy(Qt::CustomContextMenu);
        connect(m_ui->inboxFoldersTreeView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(folderTreeViewContextMenuRequested(QPoint)));
        connect(m_ui->inboxFoldersTreeView->selectionModel(), SIGNAL(currentChanged(const QModelIndex &, const QModelIndex &)), m_ui->emailContentReaderWidget, SLOT(selectedInboxFolderTreeIndexChanged(const QModelIndex &, const QModelIndex &)));
        connect(m_ui->writeEmailWidget, SIGNAL(returnToInboxRequested()), this, SLOT(returnToInboxRequested()));
        connect(m_ui->emailContentReaderWidget, SIGNAL(replyOrForwardMessageRequested(WriteEmailMessageWidget::Mode, int)), this, SLOT(replyOrForwardMessageRequested(WriteEmailMessageWidget::Mode, int)));
        connect(DatabaseManager::getInstance(), SIGNAL(messagesMetadataStructureChanged()), this, SLOT(messagesMetadataStructureChanged()));
    }

    m_unreadMessagesCount = DatabaseManager::getTotalCountOfUnreadMessages();
}

QString EmailContentsWidget::getTitle() const
{
    QString title = "(";
    title.append(QString::number(m_unreadMessagesCount));
    title.append(") Email");

    return title;
}

QLatin1String EmailContentsWidget::getType() const
{
    return QLatin1String("email");
}

QUrl EmailContentsWidget::getUrl() const
{
    return QUrl(QLatin1String("about:email"));
}

QIcon EmailContentsWidget::getIcon() const
{
    return ThemesManager::createIcon(QLatin1String("mail"), true);
}

EmailContentsWidget::~EmailContentsWidget()
{
    delete m_ui;
}

void EmailContentsWidget::on_getMessagesButton_clicked()
{
    for (EmailAccount& account : EmailAccountsManager::getEmailAccounts())
    {
        account.initializeInbox();
    }
}

void EmailContentsWidget::inboxFoldersStructureChanged()
{
    InboxFolderTreeModel *model = new InboxFolderTreeModel(DatabaseManager::getInboxFolders());
    m_ui->inboxFoldersTreeView->setModel(model);
    m_ui->inboxFoldersTreeView->expandAll();
    connect(m_ui->inboxFoldersTreeView->selectionModel(), SIGNAL(currentChanged(const QModelIndex &, const QModelIndex &)), m_ui->emailContentReaderWidget, SLOT(selectedInboxFolderTreeIndexChanged(const QModelIndex &, const QModelIndex &)));
    m_unreadMessagesCount = DatabaseManager::getTotalCountOfUnreadMessages();
}

void EmailContentsWidget::messagesMetadataStructureChanged()
{
    m_ui->emailContentReaderWidget->messagesMetadataStructureChanged();
    static_cast<InboxFolderTreeModel*>(m_ui->inboxFoldersTreeView->model())->updateCountOfUnreadMessages();
}

void EmailContentsWidget::on_writeMessageButton_clicked()
{
    if (!isSidebarPanel())
    {
        m_ui->emailContentReaderWidget->hide();
        m_ui->emailSidebarWidget->hide();
        m_ui->writeEmailWidget->setMode(WriteEmailMessageWidget::Mode::WriteMessage);
        m_ui->writeEmailWidget->setMessage(Message());
        m_ui->writeEmailWidget->show();
    }
}

void EmailContentsWidget::folderTreeViewContextMenuRequested(QPoint position)
{
    QItemSelectionModel *selectionModel = m_ui->inboxFoldersTreeView->selectionModel();
    QModelIndex indexUnderCursor = m_ui->inboxFoldersTreeView->indexAt(position);

    if (indexUnderCursor.isValid())
    {
        QMenu *menu = new QMenu();

        InboxFolderTreeItem* selectedItem = static_cast<InboxFolderTreeItem*>(selectionModel->selectedIndexes().first().internalPointer());
        QString caption = selectedItem->data(0).toString();

        QRegExp emailAddressRegexPattern("\\b[A-Z0-9._%+-]+@[A-Z0-9.-]+\\.[A-Z]{2,4}\\b");
        emailAddressRegexPattern.setCaseSensitivity(Qt::CaseInsensitive);
        emailAddressRegexPattern.setPatternSyntax(QRegExp::RegExp);

        emailAddressRegexPattern.indexIn(caption);

        if (emailAddressRegexPattern.capturedTexts().last() != QString())
        {
            return;
        }
        else if (caption == "INBOX")
        {
            QAction *newFolderAction = new QAction("New Folder");

            connect(newFolderAction, SIGNAL(triggered(bool)), this, SLOT(createNewFolderActionTriggered(bool)));

            menu->addAction(newFolderAction);
        }
        else
        {
            QAction *newSubfolderAction = new QAction("New Subfolder");
            QAction *deleteFolderAction = new QAction("Delete Folder");
            QAction *renameFolderAction = new QAction("Rename Folder");

            connect(newSubfolderAction, SIGNAL(triggered(bool)), this, SLOT(createNewSubfolderActionTriggered(bool)));
            connect(deleteFolderAction, SIGNAL(triggered(bool)), this, SLOT(deleteFolderActionTriggered(bool)));
            connect(renameFolderAction, SIGNAL(triggered(bool)), this, SLOT(renameFolderActionTriggered(bool)));

            menu->addAction(newSubfolderAction);
            menu->addAction(deleteFolderAction);
            menu->addAction(renameFolderAction);
        }

        menu->popup(m_ui->inboxFoldersTreeView->viewport()->mapToGlobal(position));
    }
}

void EmailContentsWidget::createNewFolderActionTriggered(bool)
{
    QItemSelectionModel *selectionModel = m_ui->inboxFoldersTreeView->selectionModel();

    if (selectionModel->hasSelection())
    {
        QModelIndex index = selectionModel->selectedIndexes().first();

        QString emailAddress = getEmailAddressFromFolderTreeItemIndex(index);
        QString folderPath = getFolderPathFromFolderTreeItemIndex(index);
        // int folderId = DatabaseManager::getFolderId(emailAddress, folderPath);

        NewEmailFolderDialogWindow *dialog = new NewEmailFolderDialogWindow();
        dialog->setWindowTitle("New Folder");

        if (dialog->exec())
        {
            QString folderName = dialog->getFolderName();

            if (folderName != QString())
            {
                qWarning() << "Folder " << folderName << " will be created in INBOX folder of account " << emailAddress;
                // TODO: validace na lomitka a existujici adresare ve stejnem adresari
            }
        }
    }
}

void EmailContentsWidget::createNewSubfolderActionTriggered(bool)
{
    QItemSelectionModel *selectionModel = m_ui->inboxFoldersTreeView->selectionModel();

    if (selectionModel->hasSelection())
    {
        QModelIndex index = selectionModel->selectedIndexes().first();

        QString emailAddress = getEmailAddressFromFolderTreeItemIndex(index);
        QString folderPath = getFolderPathFromFolderTreeItemIndex(index);
        // int folderId = DatabaseManager::getFolderId(emailAddress, folderPath);

        NewEmailFolderDialogWindow *dialog = new NewEmailFolderDialogWindow();
        dialog->setWindowTitle("New Subfolder");

        if (dialog->exec())
        {
            QString folderName = dialog->getFolderName();

            if (folderName != QString())
            {
                qWarning() << "Folder " << folderName << " will be created on path " << folderPath << " in account " << emailAddress;
                // TODO: validace na lomitka a existujici adresare ve stejnem adresari
            }
        }
    }
}

void EmailContentsWidget::deleteFolderActionTriggered(bool)
{
    QItemSelectionModel *selectionModel = m_ui->inboxFoldersTreeView->selectionModel();

    if (selectionModel->hasSelection())
    {
        QModelIndex index = selectionModel->selectedIndexes().first();

        QString emailAddress = getEmailAddressFromFolderTreeItemIndex(index);
        QString folderPath = getFolderPathFromFolderTreeItemIndex(index);
        // int folderId = DatabaseManager::getFolderId(emailAddress, folderPath);

        QMessageBox messageBox;

        messageBox.setWindowTitle("Delete Folder");
        messageBox.setText("Are you sure to delete the folder " + folderPath.split("/").last() + "?");
        messageBox.setIcon(QMessageBox::Question);
        messageBox.addButton(new QPushButton("Cancel"), QMessageBox::ButtonRole::RejectRole);
        messageBox.addButton(new QPushButton("Delete Folder"), QMessageBox::ButtonRole::AcceptRole);

        if (messageBox.exec())
        {
           qWarning() << "Folder " << folderPath << " of account " << emailAddress << " will be deleted";
        }
    }
}

void EmailContentsWidget::renameFolderActionTriggered(bool)
{
    QItemSelectionModel *selectionModel = m_ui->inboxFoldersTreeView->selectionModel();

    if (selectionModel->hasSelection())
    {
        QModelIndex index = selectionModel->selectedIndexes().first();

        QString emailAddress = getEmailAddressFromFolderTreeItemIndex(index);
        QString originalFolderPath = getFolderPathFromFolderTreeItemIndex(index);

        RenameEmailFolderDialog *dialog = new RenameEmailFolderDialog();
        dialog->setFolderName(originalFolderPath.split("/").last());


        if (dialog->exec())
        {
            QString renamedFolderName = dialog->getFolderName();

            if (renamedFolderName != QString())
            {
                originalFolderPath = originalFolderPath.remove(0, 1);

                QStringList renamedFolderPathTokens = originalFolderPath.split("/");
                renamedFolderPathTokens.last() = renamedFolderName;

                QString renamedFolderPath;

                for (int i = 0; i < renamedFolderPathTokens.count(); i++)
                {
                    renamedFolderPath.append(renamedFolderPathTokens.at(i));

                    if (i < renamedFolderPathTokens.size() - 1)
                    {
                        renamedFolderPath.append("/");
                    }
                }

                for (EmailAccount &account : EmailAccountsManager::getEmailAccounts())
                {
                    if (account.emailAddress() == emailAddress)
                    {
                        account.renameFolder(originalFolderPath, renamedFolderPath);
                    }
                }
            }
        }
    }
}

QString EmailContentsWidget::getEmailAddressFromFolderTreeItemIndex(QModelIndex currentIndex)
{
    QString path = getFullFolderPathFromFolderTreeItemIndex(currentIndex);
    return path.split("/")[0];
}

QString EmailContentsWidget::getFolderPathFromFolderTreeItemIndex(QModelIndex currentIndex)
{
    QString path = getFullFolderPathFromFolderTreeItemIndex(currentIndex);
    QString emailAddress = getEmailAddressFromFolderTreeItemIndex(currentIndex);

    return path.right(path.length() - emailAddress.length());
}

QString EmailContentsWidget::getFullFolderPathFromFolderTreeItemIndex(QModelIndex currentIndex)
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

void EmailContentsWidget::returnToInboxRequested()
{
    m_ui->writeEmailWidget->hide();
    m_ui->emailContentReaderWidget->show();
    m_ui->emailSidebarWidget->show();
}

void EmailContentsWidget::replyOrForwardMessageRequested(WriteEmailMessageWidget::Mode mode, int messageId)
{
    m_ui->emailContentReaderWidget->hide();
    m_ui->emailSidebarWidget->hide();
    m_ui->writeEmailWidget->setMode(mode);
    m_ui->writeEmailWidget->setMessage(getMessage(messageId));
    m_ui->writeEmailWidget->setSenderComboBoxEditable(true);
    m_ui->writeEmailWidget->show();
}

Message EmailContentsWidget::getMessage(const int messageId) const
{
    QString htmlContent = DatabaseManager::getHtmlContent(messageId);
    QString plainTextContent = DatabaseManager::getTextContent(messageId);
    QString subject = DatabaseManager::getSubject(messageId);
    Contact from = DatabaseManager::getSender(messageId);
    QList<Contact> to = DatabaseManager::getRecipients(messageId);
    QDateTime date = DatabaseManager::getDate(messageId);
    QList<Attachment> attachments = DatabaseManager::getAttachments(messageId);
    QList<EmbeddedObject> embeddedObjects = DatabaseManager::getEmbeddedObjects(messageId);

    Message message;
    message.setHtmlContent(htmlContent);
    message.setPlainTextContent(plainTextContent);
    message.setSubject(subject);
    message.setSender(from);
    message.setAddressListTo(to);
    message.setDateTime(date);
    message.setAttachments(attachments);
    message.setEmbeddedObjects(embeddedObjects);

    return message;
}

}
