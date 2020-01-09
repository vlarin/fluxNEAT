//
// Copyright (c) 2019-2020 Temporal Games Inc. All rights reserved.
//

#ifndef FLUXNEAT_CONTEXT_UNIT_H
#define FLUXNEAT_CONTEXT_UNIT_H

#include <flux/prereq.h>

#include <set>
#include <utility>

namespace flux {

    /**
     * Represents target to which block box's units are applied
     */
    class FLUX_API IContext
    {
    public:
        virtual std::string GetName() const = 0;
    };

    /**
     * Base class for all block box units. Contains target context.
     * Base unit is made non-generic intentionally to avoid using templates for all core units,
     * thus making library header-only.
     */
    class FLUX_API IContextUnit
    {
    private:
        std::shared_ptr<IContext> _context;
        std::string _id;
    protected:
        IContextUnit(std::string id, std::shared_ptr<IContext> context)
        : _id(std::move(id)), _context(std::move(context)) {}
        void Reset(std::string id, std::shared_ptr<IContext> context) { _id = std::move(id); _context = std::move(context); }
    public:
        std::string GetId() const { return _id; }
        std::shared_ptr<IContext> GetContext() const { return _context; }

        IContextUnit Clone(std::shared_ptr<IContext> context) const;
        virtual void UpdateScheme(std::istream &istream) {}

        virtual ~IContextUnit() = default;
    };
}

#endif //FLUXNEAT_CONTEXT_UNIT_H
