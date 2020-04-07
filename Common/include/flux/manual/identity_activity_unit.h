//
// Copyright (c) 2019-2020 Temporal Games Inc. All rights reserved.
//

#ifndef FLUXNEAT_IDENTITY_ACTIVITY_UNIT_H
#define FLUXNEAT_IDENTITY_ACTIVITY_UNIT_H

#include <flux/prereq.h>
#include <flux/activity_unit.h>

namespace flux
{
    /**
     * Reference identity activity unit for debugging or control flow extension capabilities.
     * Replicates target input to the output.
     */
    class FLUX_API IdentityActivityUnit : public IActivityUnit
    {
    public:
        IdentityActivityUnit(NeuralNodeId target, NeuralNodeId outputId, std::string id, std::shared_ptr<IContext> context);

        std::set<NeuralNodeId> GetInputIds() const override ;
        std::set<NeuralNodeId> GetOutputIds() const override ;

        std::vector<NeuralNode> Activate(const std::vector<NeuralNode> &inputs) const override ;

        std::shared_ptr<IContextUnit> Clone(std::shared_ptr<IContext> context) const override;
    private:
        NeuralNodeId _target;
        NeuralNodeId _outputId;
    };
}

#endif //FLUXNEAT_IDENTITY_ACTIVITY_UNIT_H
