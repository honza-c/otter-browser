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

#include "attachment.h"

namespace Otter
{

Attachment::Attachment(const Attachment &other)
{
    m_name = other.m_name;
    m_mimeType = other.m_mimeType;
    m_data = other.m_data;
}

Attachment& Attachment::operator=(const Attachment &other)
{
    m_name = other.m_name;
    m_mimeType = other.m_mimeType;
    m_data = other.m_data;

    return *this;
}

bool Attachment::operator==(const Attachment &other)
{
    return (m_name == other.m_name) && (m_data == other.m_data) && (m_mimeType == other.m_mimeType);
}

bool Attachment::operator!=(const Attachment &other)
{
    return (m_name != other.m_name) || (m_data != other.m_data) || (m_mimeType != other.m_mimeType);
}

QString Attachment::name() const
{
    return m_name;
}

QString Attachment::mimeType() const
{
    return m_mimeType;
}

QByteArray Attachment::data() const
{
    return m_data;
}

void Attachment::setName(const QString name)
{
    m_name = name;
}

void Attachment::setMimeType(const QString mimeType)
{
    m_mimeType = mimeType;
}

void Attachment::setData(const QByteArray data)
{
    m_data = data;
}

QDebug operator <<(QDebug debug, const Attachment &attachment)
{
    QDebugStateSaver saver(debug);
    debug.noquote() << "Name: " << attachment.name() << " MimeType: " << attachment.mimeType() << " Data: " << attachment.data();

    return debug;
}

QDataStream& operator<<(QDataStream &dataStream, const Attachment &attachment)
{
    dataStream << attachment.name() << attachment.mimeType() << attachment.data();

    return dataStream;
}

QDataStream& operator>>(QDataStream &dataStream, Attachment &attachment)
{
    QString name;
    QString mimeType;
    QByteArray data;

    dataStream >> name >> mimeType >> data;

    attachment.setName(name);
    attachment.setMimeType(mimeType);
    attachment.setData(data);

    return dataStream;
}

}
