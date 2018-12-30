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

    // m_ui->getMessagesButton->setIcon(ThemesManager::createIcon(QLatin1String("mail-send-receive"), false));
    m_ui->getMessagesButton->setIcon(QIcon::fromTheme("mail-send-receive"));
    m_ui->writeMessageButton->setIcon(ThemesManager::createIcon(QLatin1String("mail-send"), false));

    if (isSidebarPanel())
    {
        m_ui->emailContentReaderWidget->setVisible(false);
    }

    m_ui->splitter->setStretchFactor(0, 1);
    m_ui->splitter->setStretchFactor(1, 5);

    m_ui->inboxFolderTreeAndTagsSplitter->setStretchFactor(0, 4);
    m_ui->inboxFolderTreeAndTagsSplitter->setStretchFactor(1, 1);

    connect(m_ui->inboxFoldersTreeView->selectionModel(), SIGNAL(currentChanged(const QModelIndex &, const QModelIndex &)), m_ui->emailContentReaderWidget, SLOT(selectedInboxFolderTreeIndexChanged(const QModelIndex &, const QModelIndex &)));
}

QString EmailContentsWidget::getTitle() const
{
    return tr("Email");
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
    return ThemesManager::createIcon(QLatin1String("mail-send"), true);
}

EmailContentsWidget::~EmailContentsWidget()
{
    delete m_ui;
}

}
