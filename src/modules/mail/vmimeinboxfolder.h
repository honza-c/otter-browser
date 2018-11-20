#ifndef VMIMEINBOXFOLDER_H
#define VMIMEINBOXFOLDER_H

#include <QObject>
#include <QDebug>
#include <vmime/vmime.hpp>
#include "vmimeinboxfolderparser.h"
#include "vmimemessagecontentparser.h"
#include "vmimemessagemetadataparser.h"
#include "messagemetadata.h"
#include "attachment.h"
#include "databasemanager.h"
#include "inboxfolder.h"
#include "contact.h"

class VmimeInboxFolder : public QObject
{
    Q_OBJECT
public:
    explicit VmimeInboxFolder(vmime::shared_ptr<vmime::net::folder> remoteFolder, QString emailAddress, QObject *parent = nullptr);
    VmimeInboxFolder(const VmimeInboxFolder &other);
    VmimeInboxFolder &operator=(const VmimeInboxFolder &other);

    InboxFolder data() const;
    void setData(const InboxFolder data);

    QList<MessageMetadata> getMessagesMetadataFromPosition(int position) const;
signals:

public slots:

private:
    void openFolder();
    vmime::shared_ptr<vmime::net::folder> m_remoteFolder;
    InboxFolder m_inboxFolder;

    bool m_successfullyOpened;
};

#endif // VMIMEINBOXFOLDER_H
