//
// Copyright (c) 2019-2020 Temporal Games Inc. All rights reserved.
//

#ifndef FLUXNEAT_BLACK_BOX_H
#define FLUXNEAT_BLACK_BOX_H

#include <flux/prereq.h>

#include <flux/context_unit.h>

#include <flux/raw_sensor_unit.h>
#include <flux/augmented_sensor_unit.h>
#include <flux/activity_unit.h>
#include <flux/output_unit.h>

namespace flux {

    /**
     * Represents full packed set of units that provide inputs, transforms them and provide outputs to the external
     */
    class FLUX_API IBlackBox : public IContextUnit
    {
    protected:
        IBlackBox(std::string id, std::shared_ptr<IContext> context) : IContextUnit(std::move(id), std::move(context)) {}
    public:
        virtual void AddRawInput(std::shared_ptr<IRawSensorUnit> input) = 0;
        virtual void AddAugmentedInput(std::shared_ptr<IAugmentedSensorUnit> input) = 0;
        virtual void AddActivity(std::shared_ptr<IActivityUnit> activity) = 0;
        virtual void AddOutput(std::shared_ptr<IOutputUnit> output) = 0;

        virtual const NeuralNode &GetInputOf(std::string id) const = 0;
        virtual const NeuralNode &GetOutputOf(std::string id) const = 0;

        virtual void Step() = 0;
        virtual void UpdateChildScheme(std::string childId, std::istream &istream) = 0;

        /**
         * Manual mode control that allows to bind black box execution to the single activity
         * @param activityId
         */
        virtual void SetActivityBound(std::string activityId) {}

        /**
         * Reset manual activity bound
         */
        virtual void ResetActivityBound() {}
    };
}
#endif //FLUXNEAT_BLACK_BOX_H
