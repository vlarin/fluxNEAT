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
     * Represents target to which black box's units are applied
     */
    class FLUX_API IContext
    {
    public:
        virtual std::string GetName() const = 0;
    };

    /**
     * Base class for all black box units. Contains target context.
     * Base unit is made non-generic intentionally to avoid using templates for all core units,
     * thus making library header-only.
     */
    class FLUX_API IContextUnit
    {
    private:
		std::string _id;
        std::shared_ptr<IContext> _context;

    protected:
        IContextUnit(std::string id, std::shared_ptr<IContext> context)
        : _id(std::move(id)), _context(std::move(context)) {}

        template <typename TContextUnit>
        std::shared_ptr<TContextUnit> CloneToContext(std::shared_ptr<IContext> context) const
        {
            TContextUnit clone = *static_cast<const TContextUnit *>(this);
            clone._context = std::move(context);
            std::unique_ptr<TContextUnit> clone_ref = std::unique_ptr<TContextUnit>(new TContextUnit(clone));

            return clone_ref;
        }
        void Reset(std::string id, std::shared_ptr<IContext> context) { _id = std::move(id); _context = std::move(context); }
    public:
        std::string GetId() const { return _id; }
        std::shared_ptr<IContext> GetContext() const { return _context; }

        virtual std::shared_ptr<IContextUnit> Clone(std::shared_ptr<IContext> context) const = 0;
        virtual void UpdateScheme(std::istream &istream) {}
        virtual ~IContextUnit() = default;
    };
}

#endif //FLUXNEAT_CONTEXT_UNIT_H
