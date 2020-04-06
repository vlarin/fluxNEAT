//
// Copyright (c) 2019-2020 Temporal Games Inc. All rights reserved.
//

#ifndef FLUXNEAT_CORTEX_LAYER_H
#define FLUXNEAT_CORTEX_LAYER_H

#include <flux/prereq.h>
#include <flux/neural_input.h>
#include <flux/feedback/mediator_value.h>
#include <flux/cortex/cortex_column.h>

namespace flux
{
    /**
     * Represents container of all remembered states (columns). TODO: Persistence support
     */
    class FLUX_API CortexLayer
    {
    public:
        inline size_t GetSize() const { return _columns.size(); }

        const CortexColumn &GrowOrMerge(
                const std::map<NeuralInputId, NeuralInput> &context,
                const std::map<MediatorId, MediatorValue> &mediators);

        const std::vector<CortexColumn> &GetExcitedColumns() const;
        void Step(const std::map<NeuralInputId, NeuralInput> &context,
                  const std::map<MediatorId, MediatorValue> &mediators);
    private:
        std::vector<CortexColumn> _columns;
    };
}

#endif //FLUXNEAT_CORTEX_LAYER_H
