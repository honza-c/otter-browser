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

namespace Otter
{

PreferencesEmailPageWidget::PreferencesEmailPageWidget(QWidget *parent) :
    QWidget(parent),
    m_ui(new Ui::PreferencesEmailPageWidget)
{
    m_ui->setupUi(this);
    m_emailAccounts = EmailAccountsManager::getInstance()->getEmailAccounts();
    m_emailAccountsListModel = new UserAccountsListModel(m_emailAccounts);

    m_ui->accountListAndActionButtonsWidget->setFixedWidth(m_ui->emailAccountsListView->sizeHint().width());

    m_ui->emailAccountDetailsWidget->setVisible(false);
    m_ui->emailAccountsListView->setModel(m_emailAccountsListModel);

    m_ui->incomingServerTypeApplicationComboBox->addItem("IMAP");
    m_ui->incomingServerTypeApplicationComboBox->addItem("POP3");

    m_ui->saveChangesButton->setVisible(false);
    m_ui->discardChangesButton->setVisible(false);

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

void PreferencesEmailPageWidget::emailAccountsListViewSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected)
{
    if (selected.indexes().size() > 0)
    {
        QModelIndex index = selected.indexes().first();

        if (index.isValid() && index != QModelIndex())
        {
            int row = index.row();
            UserAccount account = m_emailAccounts.at(row);

            m_ui->accountNameLineEditWidget->setText(account.accountName());
            m_ui->yourNameLineEditWidget->setText(account.contactName());
            m_ui->emailAddressLineEditWidget->setText(account.emailAddress());
            m_ui->userNameLineEditWidget->setText(account.userName());
            m_ui->passwordLineEditWidget->setText(account.password());
            m_ui->incomingServerUrlLineEditWidget->setText(account.incomingServerAddress());
            m_ui->incomingServerPortSpinBox->setValue(account.incomingServerPort());
            m_ui->smtpServerUrlLineEditWidget->setText(account.smtpServerUrl());
            m_ui->smtpServerPortSpinBox->setValue(account.smtpServerPort());

            if (account.incomingServerType() == UserAccount::IMAP)
            {
                m_ui->incomingServerTypeApplicationComboBox->setCurrentIndex(0);
            }
            else
            {
                m_ui->incomingServerTypeApplicationComboBox->setCurrentIndex(1);
            }

            m_ui->emailAccountDetailsWidget->setVisible(true);
        }
    }
    else
    {
        m_ui->emailAccountDetailsWidget->setVisible(false);
    }
}

void PreferencesEmailPageWidget::save()
{

}

}

void Otter::PreferencesEmailPageWidget::on_addAccountButton_clicked()
{
    m_ui->emailAccountsListView->setEnabled(false);

    m_ui->addAccountButton->setVisible(false);
    m_ui->removeAccountButton->setVisible(false);

    m_ui->saveChangesButton->setVisible(true);
    m_ui->discardChangesButton->setVisible(true);

    m_ui->horizontalSpacer->changeSize(m_ui->horizontalSpacer->maximumSize().width(), m_ui->horizontalSpacer->sizeHint().height());

    m_ui->accountNameLineEditWidget->setText(QString());
    m_ui->yourNameLineEditWidget->setText(QString());
    m_ui->emailAddressLineEditWidget->setText(QString());
    m_ui->userNameLineEditWidget->setText(QString());
    m_ui->passwordLineEditWidget->setText(QString());
    m_ui->incomingServerUrlLineEditWidget->setText(QString());
    m_ui->incomingServerPortSpinBox->clear();
    m_ui->smtpServerUrlLineEditWidget->setText(QString());
    m_ui->smtpServerPortSpinBox->clear();

    m_ui->emailAccountDetailsWidget->setVisible(true);

    m_ui->accountNameLineEditWidget->setFocus();
}

void Otter::PreferencesEmailPageWidget::on_saveChangesButton_clicked()
{
    m_ui->emailAccountsListView->setEnabled(true);

    m_ui->addAccountButton->setVisible(true);
    m_ui->removeAccountButton->setVisible(true);

    m_ui->saveChangesButton->setVisible(false);
    m_ui->discardChangesButton->setVisible(false);
}

void Otter::PreferencesEmailPageWidget::on_discardChangesButton_clicked()
{
    m_ui->emailAccountsListView->setEnabled(true);

    m_ui->addAccountButton->setVisible(true);
    m_ui->removeAccountButton->setVisible(true);

    m_ui->saveChangesButton->setVisible(false);
    m_ui->discardChangesButton->setVisible(false);
}

void Otter::PreferencesEmailPageWidget::on_removeAccountButton_clicked()
{
    int accountIndex = m_ui->emailAccountsListView->selectionModel()->selectedRows().first().row();
    QString accountAddress = m_emailAccounts.at(accountIndex).emailAddress();
    QMessageBox messageBox;

    messageBox.setWindowTitle("Delete e-mail account");
    messageBox.setText("Are you sure to delete the account " +  accountAddress + "?");
    messageBox.setIcon(QMessageBox::Question);
    messageBox.addButton(new QPushButton("Cancel"), QMessageBox::ButtonRole::RejectRole);
    messageBox.addButton(new QPushButton("Delete Account"), QMessageBox::ButtonRole::AcceptRole);

    if (messageBox.exec())
    {
        m_emailAccountsListModel->removeRows(accountIndex, 1);
    }
}
