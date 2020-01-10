//
// Copyright (c) 2019-2020 Temporal Games Inc. All rights reserved.
//

#ifndef FLUXNEAT_UNIT_CONTEXT_H
#define FLUXNEAT_UNIT_CONTEXT_H

#include <flux/context_unit.h>

namespace flux {

    /**
     * Represents "empty" context that contains only debug name.
     * Note that only base units support unit context while other will fail with runtime error
     */
    class UnitContext : public IContext
    {
    public:
        explicit UnitContext(std::string name) : _name(std::move(name)) {}

        std::string GetName() const override { return _name; }
    private:
        std::string _name;
    };
}
#endif //FLUXNEAT_UNIT_CONTEXT_H
