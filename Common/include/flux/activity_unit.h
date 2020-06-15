//
// Copyright (c) 2019-2020 Temporal Games Inc. All rights reserved.
//

#ifndef FLUXNEAT_ACTIVITY_UNIT_H
#define FLUXNEAT_ACTIVITY_UNIT_H

#include <set>

#include <flux/prereq.h>
#include <flux/neural_node.h>
#include <flux/context_unit.h>

namespace flux {

    /**
     * Activity unit represents standard input -> output conversion unit
     */
    class FLUX_API IActivityUnit : public IContextUnit
    {
    protected:
        IActivityUnit(std::string id, std::shared_ptr<IContext> context) : IContextUnit(std::move(id), std::move(context)) {}
    public:
        virtual std::vector<NeuralNodeId> GetInputIds() const = 0;
        virtual std::vector<NeuralNodeId> GetOutputIds() const = 0;

        virtual std::vector<NeuralNode> Activate(const std::vector<NeuralNode> &inputs) const = 0;
    };
}

#endif //FLUXNEAT_ACTIVITY_UNIT_H
