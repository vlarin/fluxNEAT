//
// Copyright (c) 2019-2020 Temporal Games Inc. All rights reserved.
//

#ifndef FLUXNEAT_CORTEX_COLUMN_H
#define FLUXNEAT_CORTEX_COLUMN_H

#include <flux/prereq.h>
#include <flux/neural_input.h>
#include <flux/feedback/mediator_value.h>
#include <flux/cortex/cortex_transition.h>

namespace flux
{
    /**
     * Cortex state holder. Created when a new strong feedback received. Keeps mediator averages and input trusted intervals
     * TODO: serialization, intermediate columns for transition bounds
     * TODO: column cascade formation subsystem
     */
    class FLUX_API CortexColumn
    {
    public:
        explicit CortexColumn(
                size_t id,
                const std::map<NeuralInputId, NeuralInput> &immediateContext,
                std::map<MediatorId, MediatorValue> mediators);

        inline size_t GetId() const { return _id; }
        inline float_fl GetExcitementLevel() const { return _excitementLevel; }

        /**
         * Try to merge new captured feedback state into this column. This method will try to expand trusted intervals
         * of the cell to keep both ends in the same mediation response window. Otherwise if mediators doesn't fit but
         * input is inside a hypersphere, it will shrink to update correctness of the model
         * @param context
         * @param mediators
         * @return true, if merge went successful, false otherwise
         */
        bool TryMerge(
                const std::map<NeuralInputId, NeuralInput> &context,
                const std::map<MediatorId, MediatorValue> &mediators);

        void Step(const std::map<NeuralInputId, NeuralInput> &context);

        bool operator <(const CortexColumn &other) const;
        bool operator >(const CortexColumn &other) const;
    private:
        size_t _id;

        // Column is described as a hypersphere in the n-dim space input space and
        // described with a pivot (initial point), current centroid and radius.
        // Represents approximated group of neurons that constraint part of convex feature space
        //TODO: consider using low level vertex math from armadillo
        std::map<NeuralInputId, NeuralInput> _pivot;
        std::map<NeuralInputId, NeuralInput> _centroid;
        std::map<NeuralInputId, NeuralInput> _limitUpper;
        std::map<NeuralInputId, NeuralInput> _limitLower;
        float_fl _radius;

        std::map<MediatorId, MediatorValue> _mediatorsAverages;

        float_fl _excitementLevel;

        // Computes activation value of the column, it ranges from 1 inside a hypersphere and
        // then linearly drops to 0.1 at X% of outside radius continuing with more prolonged linear drop to zero at 10 units
        float_fl CalculateActivationValue(const std::map<NeuralInputId, NeuralInput> &context);
    };
}

#endif //FLUXNEAT_CORTEX_COLUMN_H
