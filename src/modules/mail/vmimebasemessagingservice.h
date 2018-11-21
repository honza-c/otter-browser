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

#ifndef VMIMEBASEMESSAGINGSERVICE_H
#define VMIMEBASEMESSAGINGSERVICE_H

#include <QObject>
#include <QSslConfiguration>
#include <QSslCertificate>
#include <vmime/vmime.hpp>

namespace Otter
{

class VmimeBaseMessagingService : public QObject
{
    Q_OBJECT
public:
    explicit VmimeBaseMessagingService(QObject *parent = nullptr);
    VmimeBaseMessagingService(const VmimeBaseMessagingService &other);

    QString serverUrl() const;
    void setServerUrl(const QString serverUrl);

    QString emailAddress() const;
    void setEmailAddress(const QString emailAddress);

    QString userName() const;
    void setUserName(const QString userName);

    QString password() const;
    void setPassword(const QString password);

    int port() const;
    void setPort(const int port);

    void initializeSession();

signals:

public slots:

    protected:
    void initializeCertificateVerifier();
    std::vector<vmime::shared_ptr<vmime::security::cert::X509Certificate>> getSystemCertificates();

    std::string m_serverUrl;
    std::string m_emailAddress;
    std::string m_userName;
    std::string m_password;
    int m_port;

    vmime::shared_ptr<vmime::net::session> m_session;
    vmime::shared_ptr<vmime::security::cert::defaultCertificateVerifier> m_certificateVerifier;
};

}

#endif // VMIMEBASEMESSAGINGSERVICE_H
