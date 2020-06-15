//
// Copyright (c) 2020 Temporal Games Inc. All rights reserved.
//

#ifndef FLUXNEAT_OUTPUT_UNIT_H
#define FLUXNEAT_OUTPUT_UNIT_H

#include <flux/prereq.h>
#include <flux/context_unit.h>
#include <flux/neural_node.h>

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
        virtual std::vector<NeuralNodeId> GetOutputIds() const = 0;

        virtual void Apply(const std::map<flux::NeuralNodeId, flux::NeuralNode> &outputs) const = 0;
    };
}
#endif //FLUXNEAT_OUTPUT_UNIT_H
