//
// Copyright (c) 2019-2020 Temporal Games Inc. All rights reserved.
//

#ifndef FLUXNEAT_AUGMENTED_ACTIVITY_SENSOR_UNIT_H
#define FLUXNEAT_AUGMENTED_ACTIVITY_SENSOR_UNIT_H

#include <flux/prereq.h>
#include <flux/neural_node.h>
#include <flux/context_unit.h>
#include <flux/activity_unit.h>
#include <flux/augmented_sensor_unit.h>

#include <set>

namespace flux {

    /**
     * Input Augmentation activity unit wrapper acts like an additional passive sensor in the blackbox
     */
    class FLUX_API AugmentedActivitySensorUnit : public IAugmentedSensorUnit
    {
    protected:
        explicit AugmentedActivitySensorUnit(std::shared_ptr<IActivityUnit> activity, std::shared_ptr<IContext> context)
        : IAugmentedSensorUnit(activity->GetId(), std::move(context)), _activity(activity) {}
    public:
        virtual std::set<NeuralNodeId> GetInputIds() const { return _activity->GetInputIds(); }
        virtual std::set<NeuralNodeId> GetAugmentedInputIds() const { return _activity->GetOutputIds(); }

        virtual std::vector<NeuralNode> ApplyAugmentation(const std::vector<NeuralNode> &rawInputs) const
        {
            assert(_activity);
            return _activity->Activate(rawInputs);
        }

    private:
        std::shared_ptr<IActivityUnit> _activity;
    };

}

#endif //FLUXNEAT_AUGMENTED_ACTIVITY_SENSOR_UNIT_H
