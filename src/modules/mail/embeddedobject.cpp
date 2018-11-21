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

#include "embeddedobject.h"

namespace Otter
{

EmbeddedObject::EmbeddedObject(const EmbeddedObject &other)
{
    m_name = other.m_name;
    m_mimeType = other.m_mimeType;
    m_contentId = other.m_contentId;
    m_data = other.m_data;
}

EmbeddedObject& EmbeddedObject::operator=(const EmbeddedObject &other)
{
    m_name = other.m_name;
    m_mimeType = other.m_mimeType;
    m_contentId = other.m_contentId;
    m_data = other.m_data;

    return *this;
}

bool EmbeddedObject::operator==(const EmbeddedObject &other)
{
    return m_name == other.m_name &&
           m_mimeType == other.m_mimeType &&
           m_contentId == other.m_contentId &&
           m_data == other.m_data;
}

bool EmbeddedObject::operator!=(const EmbeddedObject &other)
{
    return m_name != other.m_name ||
            m_mimeType != other.m_mimeType ||
            m_contentId != other.m_contentId ||
            m_data != other.m_data;
}

QString EmbeddedObject::contentId() const
{
    return m_contentId;
}

void EmbeddedObject::setContentId(const QString contentId)
{
    m_contentId = contentId;
}

QDebug operator <<(QDebug debug, const EmbeddedObject &embeddedObject)
{
    QDebugStateSaver saver(debug);

    debug.noquote() << "Name: " << embeddedObject.name()
                    << "\nMimeType: " << embeddedObject.mimeType()
                    << "\nContentId: " << embeddedObject.contentId()
                    << "\nData: " << embeddedObject.data();

    return debug;
}

QDataStream& operator<<(QDataStream &dataStream, const EmbeddedObject &embeddedObject)
{
    dataStream << embeddedObject.name()
               << embeddedObject.mimeType()
               << embeddedObject.contentId()
               << embeddedObject.data();

    return dataStream;
}

QDataStream& operator>>(QDataStream &dataStream, EmbeddedObject &embeddedObject)
{
    QString name;
    QString mimeType;
    QString contentId;
    QByteArray data;

    dataStream >> name >> mimeType >> contentId >> data;

    embeddedObject.setName(name);
    embeddedObject.setMimeType(mimeType);
    embeddedObject.setContentId(contentId);
    embeddedObject.setData(data);

    return dataStream;
}

}
