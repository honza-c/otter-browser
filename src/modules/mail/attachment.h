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

#ifndef ATTACHMENT_H
#define ATTACHMENT_H

#include <QObject>
#include <QDataStream>
#include <QDebug>

namespace Otter
{

class Attachment : public QObject
{
    Q_OBJECT
public:
    explicit Attachment(QObject *parent = nullptr)
        : QObject(parent) {}
    Attachment(const Attachment &other);
    Attachment &operator=(const Attachment &other);
    bool operator==(const Attachment &other);
    bool operator!=(const Attachment &other);

    QString name() const;
    QString mimeType() const;
    QByteArray data() const;

    void setName(const QString name);
    void setMimeType(const QString mimeType);
    void setData(const QByteArray data);
signals:

public slots:

protected:
    QString m_name;
    QString m_mimeType;
    QByteArray m_data;
};

QDebug operator <<(QDebug debug, const Attachment &attachment);
QDataStream& operator<<(QDataStream &dataStream, const Attachment &attachment);
QDataStream& operator>>(QDataStream &dataStream, Attachment &attachment);

}

#endif // ATTACHMENT_H
