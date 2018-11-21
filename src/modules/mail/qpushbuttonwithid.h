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

#ifndef QPUSHBUTTONWITHID_H
#define QPUSHBUTTONWITHID_H

#include <QObject>
#include <QPushButton>

namespace Otter
{

class QPushButtonWithId : public QPushButton
{
public:
    QPushButtonWithId() : QPushButton() {}
    QPushButtonWithId(QString name) : QPushButton(name) {}

    int id();
    void setId(int id);

private:
    int m_id;
};

}

#endif // QPUSHBUTTONWITHID_H
