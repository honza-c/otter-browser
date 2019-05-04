#include "EmailAccountPreferencesDialogWindow.h"
#include "ui_EmailAccountPreferencesDialogWindow.h"

#include <QtWidgets>

namespace Otter
{

EmailAccountPreferencesDialogWindow::EmailAccountPreferencesDialogWindow(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::EmailAccountPreferencesDialogWindow)
{
    m_ui->setupUi(this);

    m_ui->imapServerPortSpinBox->setValue(143);
    m_ui->smtpServerPortSpinbox->setValue(25);

    setFixedHeight(m_ui->verticalLayout->sizeHint().height());

}

EmailAccountPreferencesDialogWindow::~EmailAccountPreferencesDialogWindow()
{
    delete m_ui;
}

EmailAccount EmailAccountPreferencesDialogWindow::getEmailAccount() const
{
    EmailAccount account;

    account.setContactName(m_ui->yourNameLineEdit->text());
    account.setEmailAddress(m_ui->emailAddressLineEdit->text());

    account.setImapServerUserName(m_ui->imapUserNameLineEdit->text());
    account.setImapServerPassword(m_ui->imapPasswordLineEdit->text());
    account.setImapServerAddress(m_ui->imapServerUrlLineEdit->text());
    account.setImapServerPort(m_ui->imapServerPortSpinBox->value());
    account.setIsImapServerConnectionEncrypted(m_ui->imapEncryptedConnectionCheckbox->isChecked());

    account.setSmtpServerUserName(m_ui->smtpUserNameLineEdit->text());
    account.setSmtpServerPassword(m_ui->smtpPasswordLineEdit->text());
    account.setSmtpServerAddress(m_ui->smtpServerUrlLineEdit->text());
    account.setSmtpServerPort(m_ui->smtpServerPortSpinbox->value());
    account.setIsSmtpServerConnectionEncrypted(m_ui->smtpEncryptedConnectionCheckBox->isChecked());

    return account;
}

void EmailAccountPreferencesDialogWindow::setEmailAccount(const EmailAccount account)
{
    m_ui->yourNameLineEdit->setText(account.contactName());
    m_ui->emailAddressLineEdit->setText(account.emailAddress());

    m_ui->imapUserNameLineEdit->setText(account.imapServerUserName());
    m_ui->imapPasswordLineEdit->setText(account.imapServerPassword());
    m_ui->imapServerUrlLineEdit->setText(account.imapServerAddress());
    m_ui->imapServerPortSpinBox->setValue(account.imapServerPort());
    m_ui->imapEncryptedConnectionCheckbox->setChecked(account.isImapServerConnectionEncrypted());

    m_ui->smtpUserNameLineEdit->setText(account.smtpServerUserName());
    m_ui->smtpPasswordLineEdit->setText(account.smtpServerPassword());
    m_ui->smtpServerUrlLineEdit->setText(account.smtpServerAddress());
    m_ui->smtpServerPortSpinbox->setValue(account.smtpServerPort());
    m_ui->smtpEncryptedConnectionCheckBox->setChecked(account.isSmtpServerConnectionEncrypted());
}

void EmailAccountPreferencesDialogWindow::on_imapEncryptedConnectionCheckbox_clicked()
{
    if (m_ui->imapEncryptedConnectionCheckbox->isChecked())
    {
        m_ui->imapServerPortSpinBox->setValue(993);
    }
    else
    {
        m_ui->imapServerPortSpinBox->setValue(143);
    }
}

void EmailAccountPreferencesDialogWindow::on_smtpEncryptedConnectionCheckBox_clicked()
{
    if (m_ui->smtpEncryptedConnectionCheckBox->isChecked())
    {
        m_ui->smtpServerPortSpinbox->setValue(465);
    }
    else
    {
        m_ui->smtpServerPortSpinbox->setValue(25);
    }
}

void EmailAccountPreferencesDialogWindow::on_buttonBox_accepted()
{
    QDialog::accept();
}

void EmailAccountPreferencesDialogWindow::on_buttonBox_rejected()
{
    QDialog::reject();
}

}

