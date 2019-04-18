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

#include "multiselectcompleter.h"

namespace Otter
{

QString MultiSelectCompleter::pathFromIndex(const QModelIndex &index) const
{
    QString path = QCompleter::pathFromIndex(index);
    QString text = static_cast<QLineEdit*>(widget())->text();

    int position = text.lastIndexOf(",");

    if (position >= 0)
    {
        path = text.left(position) + ", " + path;
    }

    return path;
}

QStringList MultiSelectCompleter::splitPath(const QString &path) const
{
    int position = path.lastIndexOf(",") + 1;

    while (position < path.length() && path.at(position) == ' ')
    {
        position++;
    }

    return QStringList(path.mid(position));
}

}
