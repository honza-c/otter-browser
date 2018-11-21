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

#include "contact.h"

namespace Otter
{

Contact::Contact(const Contact &other)
{
    m_name = other.m_name;
    m_emailAddress = other.m_emailAddress;
}

Contact& Contact::operator=(const Contact &other)
{
    m_name = other.m_name;
    m_emailAddress = other.m_emailAddress;

    return *this;
}

QString Contact::name() const
{
    return m_name;
}

void Contact::setName(const QString name)
{
    m_name = name;
}

QString Contact::emailAddress() const
{
    return m_emailAddress;
}

void Contact::setEmailAddress(const QString emailAddress)
{
    m_emailAddress = emailAddress;
}

QString Contact::toString(const Contact contact)
{
    QString result;

    if (contact.name() != QString())
    {
        result.append(contact.name());
        result.append(" ");
    }

    result.append("<");
    result.append(contact.emailAddress());
    result.append(">");

    return result;
}

QString Contact::toString(const QList<Contact> contacts)
{
    QString result;

    for (int i = 0; i < contacts.size(); i++)
    {
        result.append(toString(contacts.at(i)));

        if (i < contacts.size() - 1)
        {
            result.append(", ");
        }
    }

    return result;
}

Contact Contact::contactFromJson(QString jsonString)
{
    QJsonDocument document = QJsonDocument::fromJson(jsonString.toUtf8());
    QJsonObject json = document.object();

    Contact contact;
    contact.readFromJson(json);

    return contact;
}

QString Contact::toJson(Contact contact)
{
    QJsonDocument document;
    QJsonObject jsonObject;

    contact.writeToJson(jsonObject);
    document.setObject(jsonObject);

    return document.toJson().toStdString().c_str();
}

QList<Contact> Contact::contactsFromJson(QString jsonString)
{
    QList<Contact> contacts;

    QJsonDocument document = QJsonDocument::fromJson(jsonString.toUtf8());
    QJsonArray jsonArray = document.array();

    for (int i = 0; i < jsonArray.size(); i++)
    {
        QJsonObject jsonObject = jsonArray[i].toObject();

        Contact contact;
        contact.readFromJson(jsonObject);

        contacts << contact;
    }

    return contacts;
}

QString Contact::toJson(QList<Contact> contacts)
{
    QJsonDocument jsonDocument;
    QJsonArray jsonArray;

    for (Contact contact : contacts)
    {
        QJsonObject jsonObject;
        contact.writeToJson(jsonObject);
        jsonArray.append(jsonObject);
    }

    jsonDocument.setArray(jsonArray);

    return jsonDocument.toJson().toStdString().c_str();
}

void Contact::readFromJson(const QJsonObject &json)
{
    m_emailAddress = json["emailAddress"].toString();
    m_name = json["name"].toString();
}

void Contact::writeToJson(QJsonObject &json)
{
    json["emailAddress"] = m_emailAddress;
    json["name"] = m_name;
}

QDebug operator<<(QDebug debug, const Contact &contact)
{
    QDebugStateSaver saver(debug);

    debug.noquote() << "Name: "
                    << contact.name()
                    << ", E-mail address: "
                    << contact.emailAddress();

    return debug;
}

bool operator==(const Contact first, const Contact second)
{
    return first.name() == second.name() && first.emailAddress() == second.emailAddress();
}

}
