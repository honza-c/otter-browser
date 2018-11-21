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

#ifndef CONTACT_H
#define CONTACT_H

#include <QObject>
#include <QDebug>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>

namespace Otter
{

class Contact : public QObject
{
    Q_OBJECT
public:
    explicit Contact(QObject *parent = nullptr)
        : QObject(parent) {}

    Contact(const Contact &other);
    Contact &operator=(const Contact &other);

    QString name() const;
    void setName(const QString name);

    QString emailAddress() const;
    void setEmailAddress(const QString emailAddress);

    static QString toString(const Contact contact);
    static QString toString(const QList<Contact> contacts);

    static Contact contactFromJson(QString jsonString);
    static QString toJson(Contact contact);
    static QList<Contact> contactsFromJson(QString jsonString);
    static QString toJson(QList<Contact> contacts);
signals:

public slots:

private:
    void readFromJson(const QJsonObject &json);
    void writeToJson(QJsonObject &json);

    QString m_name;
    QString m_emailAddress;
};

QDebug operator<<(QDebug debug, const Contact &contact);
bool operator==(const Contact first, const Contact second);

}

#endif // CONTACT_H
