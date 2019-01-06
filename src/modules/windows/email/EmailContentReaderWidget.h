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

#ifndef EMAILCONTENTREADERWIDGET_H
#define EMAILCONTENTREADERWIDGET_H

#include <QWidget>
#include <QWebEngineView>
#include <QWebEngineSettings>
#include <QtWidgets>
#include "src/modules/mail/messagemetadatasqltablemodel.h"
#include "src/modules/mail/databasemanager.h"
#include "../../../core/EmailAccountsManager.h"
#include "src/modules/mail/qpushbuttonwithid.h"
#include "src/ui/TextBrowserWidget.h"
#include "WriteEmailMessageWidget.h"

namespace Otter
{

namespace Ui {
    class EmailContentReaderWidget;
}

class EmailContentReaderWidget : public QWidget
{
    Q_OBJECT

public:
    explicit EmailContentReaderWidget(QWidget *parent = 0);
    ~EmailContentReaderWidget();

signals:
    void replyOrForwardMessageRequested(WriteEmailMessageWidget::Mode, int);

public slots:
    void selectedInboxFolderTreeIndexChanged(const QModelIndex &, const QModelIndex &);

private slots:
    void messageMetadataTableViewSelectionChanged(const QModelIndex &, const QModelIndex &);
    void messageContentFetched(int);
    void onAttachmentButtonClicked(bool);
    void messageMetadataTableViewContextMenuRequested(QPoint);

    void moveMessageActionTriggered(bool);
    void copyMessageActionTriggered(bool);
    void deleteMessageActionTriggered(bool);

    void on_enableRemoteContentButton_clicked(bool checked);
    void on_filterMessagesEdit_textChanged(const QString &input);
    void on_moveToTrashButton_clicked();
    void on_junkButton_clicked();
    void on_archiveButton_clicked();
    void on_replyAllButton_clicked();
    void on_replyButton_clicked();
    void on_forwardButton_clicked();

private:
    void setupTableModel();
    void setupTableView();
    void showMessageContent(int messageId);
    void setupBlockRemoteContentPanel(bool isHtmlMessage);
    void setupAttachmentsPanel(QList<Attachment> attachments);

    void updateMessageMetadataTableFilter(QModelIndex currentIndex, QString filterText);
    void deleteOldAttachmentsLayout(QLayout *layout);
    QHBoxLayout* createAttachmentsLayout(QString caption, QList<Attachment> attachments);
    QString getEmailAddressFromFolderTreeItemIndex(QModelIndex currentIndex);
    QString getFullFolderPathFromFolderTreeItemIndex(QModelIndex currentIndex);
    QString getFolderPathFromFolderTreeItemIndex(QModelIndex currentIndex);

    Ui::EmailContentReaderWidget *m_ui;
    MessageMetadataSqlTableModel *m_messageMetadataTableModel;
    QModelIndex m_currentInboxFolderTreeIndex;
};

}
#endif // EMAILCONTENTREADERWIDGET_H
