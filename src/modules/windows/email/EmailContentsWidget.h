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

#ifndef EMAILPANELWIDGET_H
#define EMAILPANELWIDGET_H

#include "../../../ui/ContentsWidget.h"
#include "../../../core/EmailAccountsManager.h"
#include "../../../modules/mail/inboxfoldertreemodel.h"
#include "../../../modules/mail/inboxfolder.h"
#include "../../../modules/mail/databasemanager.h"
#include "../../../modules/mail/messagemetadatasqltablemodel.h"
#include "EmailContentReaderWidget.h"
#include "WriteEmailMessageWidget.h"
#include "RenameEmailFolderDialog.h"
#include "NewEmailFolderDialogWindow.h"

namespace Otter
{

namespace Ui {
    class EmailContentsWidget;
}

class Window;

class EmailContentsWidget final : public ContentsWidget
{
    Q_OBJECT

public:
    explicit EmailContentsWidget(const QVariantMap &parameters, Window *window, QWidget *parent = nullptr);
    ~EmailContentsWidget();

    QString getTitle() const override;
    QLatin1String getType() const override;
    QUrl getUrl() const override;
    QIcon getIcon() const override;

protected:

protected slots:

private slots:
    void folderTreeViewContextMenuRequested(QPoint);
    void createNewFolderActionTriggered(bool);
    void createNewSubfolderActionTriggered(bool);
    void deleteFolderActionTriggered(bool);
    void renameFolderActionTriggered(bool);
    void returnToInboxRequested();
    void replyOrForwardMessageRequested(WriteEmailMessageWidget::Mode, int);
    void inboxFoldersStructureChanged();
    void messagesMetadataStructureChanged();
    void newEmailTabRequested(const QModelIndex &, const QModelIndex &);

    void on_getMessagesButton_clicked();
    void on_writeMessageButton_clicked();

private:
    Message getMessage(const int messageId) const;

    QString getEmailAddressFromFolderTreeItemIndex(QModelIndex currentIndex);
    QString getFolderPathFromFolderTreeItemIndex(QModelIndex currentIndex);
    QString getFullFolderPathFromFolderTreeItemIndex(QModelIndex currentIndex);

    Window *m_window;
    Ui::EmailContentsWidget *m_ui;
};

}

#endif // EMAILPANELWIDGET_H
