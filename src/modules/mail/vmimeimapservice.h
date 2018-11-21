#ifndef VMIMEIMAPSERVICE_H
#define VMIMEIMAPSERVICE_H

#include "vmimeinboxservice.h"

namespace Otter
{

class VmimeImapService : public VmimeInboxService
{
public:
    VmimeImapService(QObject *parent = nullptr)
        : VmimeInboxService(parent) {}

private:
    vmime::utility::url getStoreUrl() const;
};

}

#endif // VMIMEIMAPSERVICE_H
