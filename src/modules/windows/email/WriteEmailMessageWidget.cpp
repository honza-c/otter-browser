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

#include "WriteEmailMessageWidget.h"
#include "../../../core/ThemesManager.h"
#include "../../../ui/Action.h"
#include "../../../ui/MainWindow.h"
#include "../../../ui/Window.h"

#include "ui_WriteEmailMessageWidget.h"

#include <QtWidgets/QDesktopWidget>
#include <QtWidgets/QToolTip>

namespace Otter
{

WriteEmailMessageWidget::WriteEmailMessageWidget(const QVariantMap &parameters, Window *window, QWidget *parent) : ContentsWidget (parameters, nullptr, parent),
    m_window(window),
    ui(new Ui::WriteEmailMessageWidget)
{
    ui->setupUi(this);

    const MainWindow *mainWindow(MainWindow::findMainWindow(parentWidget()));

    if (mainWindow)
    {
        m_window = mainWindow->getActiveWindow();
    }

}

WriteEmailMessageWidget::~WriteEmailMessageWidget()
{
    delete ui;
}

QString WriteEmailMessageWidget::getTitle() const
{
    return tr("Write email");
}

QLatin1String WriteEmailMessageWidget::getType() const
{
    return QLatin1String("writeEmail");
}

QUrl WriteEmailMessageWidget::getUrl() const
{
    return QUrl(QLatin1String("about:writeEmail"));
}

QIcon WriteEmailMessageWidget::getIcon() const
{
    return ThemesManager::createIcon(QLatin1String("mail-send"), true);
}

}
