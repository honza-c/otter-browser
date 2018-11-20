#ifndef VMIMEINBOXSERVICE_H
#define VMIMEINBOXSERVICE_H

#include <QIcon>
#include <QVariant>
#include <vmime/vmime.hpp>
#include "vmimebasemessagingservice.h"
#include "vmimeinboxfolder.h"
#include "messagemetadata.h"
#include "databasemanager.h"
#include "messagecontent.h"
#include "contact.h"
#include <QSqlError>

class VmimeInboxService : public VmimeBaseMessagingService
{
public:
    explicit VmimeInboxService(QObject *parent = nullptr)
        : VmimeBaseMessagingService(parent) {}

    QList<InboxFolder> fetchInboxFolders();
    void initializeStore();
    QList<MessageMetadata> fetchMessagesMetadata(QMap<QString, int> folderPathsWithMessagesCountsInDb);
    MessageContent fetchMessageContent(QString folderPath, int positionInFolder);
    void moveMessage(const QString sourceFolderPath, const int messageId, const QString destinationFolderPath);
protected:
    virtual vmime::utility::url getStoreUrl() const = 0;

    bool m_storeInitialized;
    vmime::shared_ptr<vmime::net::store> m_store;
    bool m_foldersCreated();
    QList<VmimeInboxFolder> m_folders;
};

#endif // VMIMEINBOXSERVICE_H
