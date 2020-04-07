//
// Copyright (c) 2019-2020 Temporal Games Inc. All rights reserved.
//

#ifndef FLUXNEAT_AUGMENTED_SENSOR_UNIT_H
#define FLUXNEAT_AUGMENTED_SENSOR_UNIT_H

#include <flux/prereq.h>
#include <flux/neural_node.h>
#include <flux/context_unit.h>

#include <set>

namespace flux {

    /**
     * Input Augmentation interface acts like an additional sensor in the blackbox
     */
    class FLUX_API IAugmentedSensorUnit : public IContextUnit
    {
    protected:
        IAugmentedSensorUnit(std::string id, std::shared_ptr<IContext> context) : IContextUnit(std::move(id), std::move(context)) {}
    public:
        virtual std::set<NeuralNodeId> GetInputIds() const = 0;
        virtual std::set<NeuralNodeId> GetAugmentedInputIds() const = 0;

        virtual std::vector<NeuralNode> ApplyAugmentation(const std::vector<NeuralNode> &rawInputs) const = 0;
    };

}

#endif //FLUXNEAT_AUGMENTED_SENSOR_UNIT_H
