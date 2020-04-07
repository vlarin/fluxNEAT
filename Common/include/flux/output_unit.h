//
// Copyright (c) 2020 Temporal Games Inc. All rights reserved.
//

#ifndef FLUXNEAT_OUTPUT_UNIT_H
#define FLUXNEAT_OUTPUT_UNIT_H

#include <flux/prereq.h>
#include <flux/context_unit.h>

#include <set>

namespace flux {

    /**
     * Output Unit interface applies neural outputs to the external state
     */
    class FLUX_API IOutputUnit : public IContextUnit
    {
    protected:
        IOutputUnit(std::string id, std::shared_ptr<IContext> context) : IContextUnit(std::move(id), std::move(context)) {}
    public:
        virtual void Apply(const std::vector<flux::NeuralNode> &outputs) const = 0;
    };
}
#endif //FLUXNEAT_OUTPUT_UNIT_H
