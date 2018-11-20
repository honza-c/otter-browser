#ifndef EMAILACCOUNTSMANAGER_H
#define EMAILACCOUNTSMANAGER_H

#include <QtCore/QObject>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include "../modules/mail/useraccount.h"
#include "../modules/mail/databasemanager.h"

namespace Otter
{

class EmailAccountsManager final : public QObject
{
    Q_OBJECT

public:
    static void createInstance();
    static EmailAccountsManager* getInstance();

protected:
    explicit EmailAccountsManager(QObject *parent);

    static bool loadEmailAccounts(const QString &path);
    static bool saveEmailAccounts(const QString &path);

    static QJsonObject writeToJson(const UserAccount account);
    static UserAccount readFromJson(const QJsonObject json);

private:
    static EmailAccountsManager *m_instance;
    static DatabaseManager *m_databaseManager;
    static QList<UserAccount> m_emailAccounts;
};

}

#endif // EMAILACCOUNTSMANAGER_H
