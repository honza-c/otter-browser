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

#ifndef EMBEDDEDOBJECT_H
#define EMBEDDEDOBJECT_H

#include <QObject>
#include "attachment.h"

namespace Otter
{

class EmbeddedObject : public Attachment
{
    Q_OBJECT
public:
    EmbeddedObject(QObject *parent = nullptr)
        : Attachment(parent) {}

    EmbeddedObject(const EmbeddedObject &other);
    EmbeddedObject &operator=(const EmbeddedObject &other);
    bool operator==(const EmbeddedObject &other);
    bool operator!=(const EmbeddedObject &other);

    QString contentId() const;
    void setContentId(const QString contentId);

signals:

public slots:

private:
    QString m_contentId;
};

QDebug operator <<(QDebug debug, const EmbeddedObject &embeddedObject);
QDataStream& operator<<(QDataStream &dataStream, const EmbeddedObject &embeddedObject);
QDataStream& operator>>(QDataStream &dataStream, EmbeddedObject &embeddedObject);

}

#endif // EMBEDDEDOBJECT_H
