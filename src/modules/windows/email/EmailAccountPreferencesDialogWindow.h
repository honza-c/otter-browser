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

#ifndef EMAILACCOUNTPREFERENCESDIALOGWINDOW_H
#define EMAILACCOUNTPREFERENCESDIALOGWINDOW_H

#include <QDialog>
#include "../../mail/emailaccount.h"

namespace Otter
{

namespace Ui {
    class EmailAccountPreferencesDialogWindow;
}

class EmailAccountPreferencesDialogWindow : public QDialog
{
    Q_OBJECT

public:
    explicit EmailAccountPreferencesDialogWindow(QWidget *parent = nullptr);
    ~EmailAccountPreferencesDialogWindow();

    EmailAccount getEmailAccount() const;
    void setEmailAccount(const EmailAccount account);


private slots:
    void on_buttonBox_accepted();
    void on_buttonBox_rejected();
    void on_imapEncryptedConnectionCheckbox_clicked();
    void on_smtpEncryptedConnectionCheckBox_clicked();

private:
    Ui::EmailAccountPreferencesDialogWindow *m_ui;
};

}
#endif // EMAILACCOUNTPREFERENCESDIALOGWINDOW_H
