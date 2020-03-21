//
// Copyright (c) 2019-2020 Temporal Games Inc. All rights reserved.
//

#ifndef FLUXNEAT_CORTEX_COLUMN_H
#define FLUXNEAT_CORTEX_COLUMN_H

#include <flux/prereq.h>
#include <flux/neural_input.h>
#include <flux/feedback/mediator_value.h>
#include <flux/cortex/cortex_feedback.h>
#include <flux/cortex/cortex_transition.h>

namespace flux
{
    /**
     * Cortex state holder. Created when a new strong feedback received. Keeps mediator averages and input trusted intervals
     * TODO: serialization, intermediate columns for transition bounds
     */
    class FLUX_API CortexColumn
    {
    public:
        explicit CortexColumn(
                size_t id,
                const std::map<NeuralInputId, NeuralInput> &immediateContext,
                std::map<MediatorId, MediatorValue> mediators);

        /**
         * Try to merge new captured feedback state into this column. This method will try to expand trusted intervals
         * of the cell to keep both ends in the same mediation response window.
         * @param cortexFeedback
         * @param context
         * @param mediators
         * @return true, if merge went successful, false otherwise
         */
        bool TryMerge(
                const ICortexFeedback &cortexFeedback,
                const std::map<NeuralInputId, NeuralInput> &context,
                const std::map<MediatorId, MediatorValue> &mediators);
    private:
        size_t _id;
        std::map<NeuralInputId, NeuralInput> _sensorsLow;
        std::map<NeuralInputId, NeuralInput> _sensorsHi;
        std::map<MediatorId, MediatorValue> _mediatorsAverages;
    };
}

#endif //FLUXNEAT_CORTEX_COLUMN_H
