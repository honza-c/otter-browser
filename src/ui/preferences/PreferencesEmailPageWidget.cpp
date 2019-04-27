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

    m_emailAccountsListModel = new EmailAccountsListModel(m_emailAccounts);
    m_editMode = false;
    m_creatingNewAccountMode = false;

    m_ui->removeAccountButton->setEnabled(false);

    m_ui->emailAccountDetailsWidget->setVisible(false);

    m_ui->emailAccountsListView->setModel(m_emailAccountsListModel);
    m_ui->saveAndDiscardChangesButtons->setVisible(false);


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
            EmailAccount account = m_emailAccounts.at(row);

            m_ui->yourNameLineEditWidget->setText(account.contactName());
            m_ui->emailAddressLineEditWidget->setText(account.emailAddress());
            m_ui->userNameLineEditWidget->setText(account.userName());
            m_ui->passwordLineEditWidget->setText(account.password());
            m_ui->imapServerUrlLineEditWidget->setText(account.imapServerAddress());
            m_ui->imapServerPortSpinBox->setValue(account.imapServerPort());
            m_ui->smtpServerUrlLineEditWidget->setText(account.smtpServerUrl());
            m_ui->smtpServerPortSpinBox->setValue(account.smtpServerPort());

            m_ui->emailAccountDetailsWidget->setVisible(true);

            deactivateEditMode();
        }
    }
    else
    {
        m_ui->emailAccountDetailsWidget->setVisible(false);
    }
}

void Otter::PreferencesEmailPageWidget::save()
{

}

}

void Otter::PreferencesEmailPageWidget::on_saveChangesButton_clicked()
{
    EmailAccount account;

    account.setContactName(m_ui->yourNameLineEditWidget->text());
    account.setEmailAddress(m_ui->emailAddressLineEditWidget->text());
    account.setUserName(m_ui->userNameLineEditWidget->text());
    account.setPassword(m_ui->passwordLineEditWidget->text());
    account.setImapServerAddress(m_ui->imapServerUrlLineEditWidget->text());
    account.setImapServerPort(m_ui->imapServerPortSpinBox->value());
    account.setSmtpServerUrl(m_ui->smtpServerUrlLineEditWidget->text());
    account.setSmtpServerPort(m_ui->smtpServerPortSpinBox->value());

    if (m_creatingNewAccountMode)
    {
        m_emailAccountsListModel->removeAccount(m_ui->emailAccountsListView->currentIndex().row());
        m_emailAccountsListModel->addAccount(account);
        m_creatingNewAccountMode = false;
        m_ui->emailAccountsListView->setCurrentIndex(m_emailAccountsListModel->index(m_emailAccountsListModel->rowCount() - 1, 0, QModelIndex()));
        EmailAccountsManager::updateEmailAccountsConfiguration(m_emailAccounts);
    }
    else
    {
        QModelIndex index = m_ui->emailAccountsListView->selectionModel()->selectedIndexes().first();
        m_emailAccountsListModel->replaceAccount(account, index);
        EmailAccountsManager::updateEmailAccountsConfiguration(m_emailAccounts);
    }

    deactivateEditMode();
}

void Otter::PreferencesEmailPageWidget::on_discardChangesButton_clicked()
{
    if (!m_creatingNewAccountMode)
    {
        int index = m_ui->emailAccountsListView->selectionModel()->selectedIndexes().first().row();

        EmailAccount account = m_emailAccounts.at(index);

        m_ui->yourNameLineEditWidget->setText(account.contactName());
        m_ui->emailAddressLineEditWidget->setText(account.emailAddress());
        m_ui->userNameLineEditWidget->setText(account.userName());
        m_ui->passwordLineEditWidget->setText(account.password());
        m_ui->imapServerUrlLineEditWidget->setText(account.imapServerAddress());
        m_ui->imapServerPortSpinBox->setValue(account.imapServerPort());
        m_ui->smtpServerUrlLineEditWidget->setText(account.smtpServerUrl());
        m_ui->smtpServerPortSpinBox->setValue(account.smtpServerPort());
    }
    else
    {
        m_creatingNewAccountMode = false;
        m_emailAccountsListModel->removeAccount(m_emailAccountsListModel->rowCount() - 1);
        EmailAccountsManager::updateEmailAccountsConfiguration(m_emailAccounts);

        if (m_emailAccountsListModel->rowCount() > 0)
        {
            QModelIndex index = m_emailAccountsListModel->index(m_emailAccountsListModel->rowCount() - 1, 0, QModelIndex());
            int row = index.row();
            EmailAccount account = m_emailAccounts.at(row);

            m_ui->yourNameLineEditWidget->setText(account.contactName());
            m_ui->emailAddressLineEditWidget->setText(account.emailAddress());
            m_ui->userNameLineEditWidget->setText(account.userName());
            m_ui->passwordLineEditWidget->setText(account.password());
            m_ui->imapServerUrlLineEditWidget->setText(account.imapServerAddress());
            m_ui->imapServerPortSpinBox->setValue(account.imapServerPort());
            m_ui->smtpServerUrlLineEditWidget->setText(account.smtpServerUrl());
            m_ui->smtpServerPortSpinBox->setValue(account.smtpServerPort());

            m_ui->emailAccountsListView->setCurrentIndex(index);
        }
        else
        {
            m_ui->emailAccountDetailsWidget->setVisible(false);
        }
    }

    deactivateEditMode();
    m_ui->addAndRemoveAccountButtons->setVisible(true);
    m_ui->saveAndDiscardChangesButtons->setVisible(false);
}

void Otter::PreferencesEmailPageWidget::on_addAccountButton_clicked()
{
    m_creatingNewAccountMode = true;

    EmailAccount account;
    account.setEmailAddress("new account");

    m_emailAccountsListModel->addAccount(account);

    QModelIndex index = m_emailAccountsListModel->index(m_emailAccountsListModel->rowCount() - 1, 0, QModelIndex());
    m_ui->emailAccountsListView->setCurrentIndex(index);

    activateEditMode();

    m_ui->yourNameLineEditWidget->setText(QString());
    m_ui->emailAddressLineEditWidget->setText(QString());
    m_ui->userNameLineEditWidget->setText(QString());
    m_ui->passwordLineEditWidget->setText(QString());
    m_ui->imapServerUrlLineEditWidget->setText(QString());
    m_ui->imapServerPortSpinBox->clear();
    m_ui->smtpServerUrlLineEditWidget->setText(QString());
    m_ui->smtpServerPortSpinBox->clear();

    m_ui->emailAccountDetailsWidget->setVisible(true);
}

void Otter::PreferencesEmailPageWidget::on_removeAccountButton_clicked()
{

    if (m_ui->emailAccountsListView->selectionModel()->hasSelection())
    {
        int accountIndex = m_ui->emailAccountsListView->selectionModel()->selectedRows().first().row();
        QString accountAddress = m_emailAccounts.at(accountIndex).emailAddress();
        QString contactName = m_emailAccounts.at(accountIndex).contactName();
        QMessageBox messageBox;

        messageBox.setWindowTitle("Delete e-mail account");
        messageBox.setText("Are you sure to delete the account <" +  accountAddress + ">?");
        messageBox.setIcon(QMessageBox::Question);
        messageBox.addButton(new QPushButton("Cancel"), QMessageBox::ButtonRole::RejectRole);
        messageBox.addButton(new QPushButton("Delete Account"), QMessageBox::ButtonRole::AcceptRole);

        if (messageBox.exec())
        {
            m_emailAccountsListModel->removeRows(accountIndex, 1);
            EmailAccountsManager::updateEmailAccountsConfiguration(m_emailAccounts);
        }

        if (m_emailAccountsListModel->rowCount() > 0)
        {
            if (accountIndex >= 1)
            {
                QModelIndex newIndex = m_emailAccountsListModel->index(accountIndex - 1, 0, QModelIndex());
                EmailAccount account = m_emailAccounts.at(newIndex.row());

                m_ui->yourNameLineEditWidget->setText(account.contactName());
                m_ui->emailAddressLineEditWidget->setText(account.emailAddress());
                m_ui->userNameLineEditWidget->setText(account.userName());
                m_ui->passwordLineEditWidget->setText(account.password());
                m_ui->imapServerUrlLineEditWidget->setText(account.imapServerAddress());
                m_ui->imapServerPortSpinBox->setValue(account.imapServerPort());
                m_ui->smtpServerUrlLineEditWidget->setText(account.smtpServerUrl());
                m_ui->smtpServerPortSpinBox->setValue(account.smtpServerPort());

                m_ui->emailAccountsListView->setCurrentIndex(newIndex);
            }
            else
            {
                QModelIndex newIndex = m_emailAccountsListModel->index(0, 0, QModelIndex());
                EmailAccount account = m_emailAccounts.at(newIndex.row());

                m_ui->yourNameLineEditWidget->setText(account.contactName());
                m_ui->emailAddressLineEditWidget->setText(account.emailAddress());
                m_ui->userNameLineEditWidget->setText(account.userName());
                m_ui->passwordLineEditWidget->setText(account.password());
                m_ui->imapServerUrlLineEditWidget->setText(account.imapServerAddress());
                m_ui->imapServerPortSpinBox->setValue(account.imapServerPort());
                m_ui->smtpServerUrlLineEditWidget->setText(account.smtpServerUrl());
                m_ui->smtpServerPortSpinBox->setValue(account.smtpServerPort());

                m_ui->emailAccountsListView->setCurrentIndex(newIndex);
            }
        }
        else
        {
            m_ui->emailAccountDetailsWidget->setVisible(false);
        }

        if (m_emailAccountsListModel->rowCount() == 0)
        {
            m_ui->removeAccountButton->setEnabled(false);
        }
        else
        {
            m_ui->removeAccountButton->setEnabled(true);
        }
    }
}

void Otter::PreferencesEmailPageWidget::activateEditMode()
{
    if (!m_editMode)
    {
        m_editMode = true;

        m_ui->emailAccountsListView->setEnabled(false);

        m_ui->addAndRemoveAccountButtons->setVisible(false);
        m_ui->saveAndDiscardChangesButtons->setVisible(true);
    }
}

void Otter::PreferencesEmailPageWidget::deactivateEditMode()
{
    if (m_editMode)
    {
        m_editMode = false;

        m_ui->emailAccountsListView->setEnabled(true);

        m_ui->addAndRemoveAccountButtons->setVisible(true);
        m_ui->saveAndDiscardChangesButtons->setVisible(false);

        if (m_emailAccountsListModel->rowCount() == 0)
        {
            m_ui->removeAccountButton->setEnabled(false);
        }
        else
        {
            m_ui->removeAccountButton->setEnabled(true);
        }
    }
}

void Otter::PreferencesEmailPageWidget::on_yourNameLineEditWidget_textEdited(const QString &arg1)
{
    activateEditMode();
}

void Otter::PreferencesEmailPageWidget::on_emailAddressLineEditWidget_textEdited(const QString &arg1)
{
    activateEditMode();
}

void Otter::PreferencesEmailPageWidget::on_userNameLineEditWidget_textEdited(const QString &arg1)
{
    activateEditMode();
}

void Otter::PreferencesEmailPageWidget::on_passwordLineEditWidget_textEdited(const QString &arg1)
{
    activateEditMode();
}

void Otter::PreferencesEmailPageWidget::on_imapServerUrlLineEditWidget_textEdited(const QString &arg1)
{
    activateEditMode();
}

void Otter::PreferencesEmailPageWidget::on_imapServerPortSpinBox_valueChanged(int arg1)
{
    activateEditMode();
}

void Otter::PreferencesEmailPageWidget::on_smtpServerUrlLineEditWidget_textEdited(const QString &arg1)
{
    activateEditMode();
}

void Otter::PreferencesEmailPageWidget::on_smtpServerPortSpinBox_valueChanged(int arg1)
{
    activateEditMode();
}
