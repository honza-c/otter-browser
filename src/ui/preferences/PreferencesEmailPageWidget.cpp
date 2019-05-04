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

#include "PreferencesEmailPageWidget.h"
#include "ui_PreferencesEmailPageWidget.h"
#include <QMessageBox>
#include "../../modules/windows/email/EmailAccountPreferencesDialogWindow.h"

namespace Otter
{

PreferencesEmailPageWidget::PreferencesEmailPageWidget(QWidget *parent) :
    QWidget(parent),
    m_ui(new Ui::PreferencesEmailPageWidget)
{
    m_ui->setupUi(this);
    m_emailAccounts = EmailAccountsManager::getInstance()->getEmailAccounts();

    m_emailAccountsListModel = new EmailAccountsListModel(m_emailAccounts);
    m_ui->emailAccountsListView->setModel(m_emailAccountsListModel);

    m_ui->editAccountButton->setEnabled(false);
    m_ui->removeAccountButton->setEnabled(false);

    connect(m_ui->emailAccountsListView->selectionModel(), SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)), this, SLOT(emailAccountsListViewSelectionChanged(const QItemSelection &, const QItemSelection &)));
}

PreferencesEmailPageWidget::~PreferencesEmailPageWidget()
{
    delete m_ui;
}

void PreferencesEmailPageWidget::changeEvent(QEvent *event)
{
    QWidget::changeEvent(event);

    if (event->type() == QEvent::LanguageChange)
    {
        m_ui->retranslateUi(this);
    }
}

void PreferencesEmailPageWidget::save()
{

}

void PreferencesEmailPageWidget::emailAccountsListViewSelectionChanged(const QItemSelection &selected, const QItemSelection &)
{
    if (!selected.indexes().isEmpty() && m_emailAccountsListModel->rowCount() > 0 && selected.indexes().first().isValid())
    {
        m_ui->editAccountButton->setEnabled(true);
        m_ui->removeAccountButton->setEnabled(true);
    }
    else
    {
        m_ui->editAccountButton->setEnabled(false);
        m_ui->removeAccountButton->setEnabled(false);
    }
}

void PreferencesEmailPageWidget::on_addAccountButton_clicked()
{
    EmailAccountPreferencesDialogWindow dialog;
    dialog.setWindowTitle("Add e-mail account");

    if (dialog.exec())
    {
        m_emailAccountsListModel->addAccount(dialog.getEmailAccount());
        EmailAccountsManager::updateEmailAccountsConfiguration(m_emailAccounts);
    }
}

void PreferencesEmailPageWidget::on_editAccountButton_clicked()
{
    if (m_ui->emailAccountsListView->selectionModel()->hasSelection())
    {
        QModelIndex index = m_ui->emailAccountsListView->selectionModel()->selectedIndexes().first();

        EmailAccountPreferencesDialogWindow dialog;
        dialog.setWindowTitle("Edit e-mail account");
        dialog.setEmailAccount(m_emailAccounts.at(index.row()));

        if (dialog.exec())
        {
            m_emailAccountsListModel->replaceAccount(dialog.getEmailAccount(), index);
            EmailAccountsManager::updateEmailAccountsConfiguration(m_emailAccounts);
        }
    }
}

void PreferencesEmailPageWidget::on_removeAccountButton_clicked()
{
    if (m_ui->emailAccountsListView->selectionModel()->hasSelection())
    {
        int accountIndex = m_ui->emailAccountsListView->selectionModel()->selectedRows().first().row();
        QString accountAddress = m_emailAccounts.at(accountIndex).emailAddress();
        QString contactName = m_emailAccounts.at(accountIndex).contactName();
        QMessageBox messageBox;

        messageBox.setWindowTitle("Remove e-mail account");
        messageBox.setText("Are you sure to remove the account <" +  accountAddress + ">?");
        messageBox.setIcon(QMessageBox::Question);
        messageBox.addButton(new QPushButton("Cancel"), QMessageBox::ButtonRole::RejectRole);
        messageBox.addButton(new QPushButton("Remove Account"), QMessageBox::ButtonRole::AcceptRole);

        if (messageBox.exec())
        {
            m_emailAccountsListModel->removeRows(accountIndex, 1);
            EmailAccountsManager::updateEmailAccountsConfiguration(m_emailAccounts);
        }
    }
}

}
