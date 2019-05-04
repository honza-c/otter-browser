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

#ifndef PREFERENCESEMAILPAGEWIDGET_H
#define PREFERENCESEMAILPAGEWIDGET_H

#include <QWidget>
#include "../../core/EmailAccountsManager.h"
#include "../../modules/mail/emailaccount.h"
#include "../../modules/mail/emailaccountslistmodel.h"

namespace Otter
{

namespace Ui {
    class PreferencesEmailPageWidget;
}

class PreferencesEmailPageWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PreferencesEmailPageWidget(QWidget *parent = nullptr);
    ~PreferencesEmailPageWidget();

public slots:
    void save();

protected:
    void changeEvent(QEvent *event) override;

protected slots:
    void emailAccountsListViewSelectionChanged(const QItemSelection &, const QItemSelection &);

private:
    Ui::PreferencesEmailPageWidget *m_ui;
    QList<EmailAccount> m_emailAccounts;
    EmailAccountsListModel *m_emailAccountsListModel;

signals:
    void settingsModified();

private slots:

    void on_addAccountButton_clicked();
    void on_editAccountButton_clicked();
    void on_removeAccountButton_clicked();
};

}
#endif // PREFERENCESEMAILPAGEWIDGET_H
