//
// Copyright (c) 2019-2020 Temporal Games Inc. All rights reserved.
//

#ifndef FLUXNEAT_CORTEX_FEEDBACK_H
#define FLUXNEAT_CORTEX_FEEDBACK_H

#include <flux/prereq.h>
#include <flux/feedback/feedback_unit.h>
#include <flux/feedback/mediator_value.h>

namespace flux
{
    /**
     * Cortex interface that allows retrieval of summarized feedback of enabled feedback units, based on the full input context
     */
    class FLUX_API ICortexFeedback
    {
    public:
        virtual std::vector<MediatorValue> FetchFeedback(const std::map<NeuralInputId, NeuralInput> &context) const = 0;
    };
}

#endif //FLUXNEAT_CORTEX_FEEDBACK_H
