//
// Copyright (c) 2019-2020 Temporal Games Inc. All rights reserved.
//

#ifndef FLUXNEAT_FEEDBACK_UNIT_H
#define FLUXNEAT_FEEDBACK_UNIT_H

#include <flux/prereq.h>
#include <flux/neural_input.h>
#include <flux/feedback/mediator_value.h>

#include <set>

namespace flux
{

    /**
     * Feedback unit interface that provides multi-dimensional feedback to the cortex
     */
    class FLUX_API IFeedbackUnit
    {
    public:
        virtual std::set<NeuralInputId> GetInputIds() const = 0;

        virtual std::vector<MediatorValue> Activate(const std::vector<NeuralInput> &inputs) const = 0;
    };
}
#endif //FLUXNEAT_FEEDBACK_UNIT_H
