//
// Copyright (c) 2019-2020 Temporal Games Inc. All rights reserved.
//

#ifndef FLUXNEAT_CONTEXT_REGISTRY_H
#define FLUXNEAT_CONTEXT_REGISTRY_H

#include <flux/context_unit.h>

namespace flux {

    class FLUX_API IContextRegistry
    {
    public:
        virtual std::shared_ptr<IContext> RetrieveContext(bool isInPreviewMode) = 0;
        virtual void ReleaseContext(const IContext &context) = 0;
        virtual void ForceReleaseAll() = 0;
    };
}

#endif //FLUXNEAT_CONTEXT_REGISTRY_H
