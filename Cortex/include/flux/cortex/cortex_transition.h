//
// Copyright (c) 2019-2020 Temporal Games Inc. All rights reserved.
//

#ifndef FLUXNEAT_CORTEX_TRANSITION_H
#define FLUXNEAT_CORTEX_TRANSITION_H

#include <flux/prereq.h>
#include <flux/neural_input.h>
#include <flux/feedback/mediator_value.h>
#include <flux/cortex/cortex_targeted_transition.h>

namespace flux
{
    /**
     * Defined rule of possible input transition. Based on discovered invariants present in activities. Initial
     * pool of cortex transitions are needed to be prebuilt, TODO: others are constructed in real time with probabilistic verification
     */
    class FLUX_API CortexTransition
    {
    public:
        explicit CortexTransition(std::string id,
                std::string activityId,
                std::map<NeuralInputId, NeuralInput> initialSequence,
                std::map<NeuralInputId, float_fl> initialVariation,
                std::map<NeuralInputId, NeuralInput> desiredSequence,
                std::map<NeuralInputId, float_fl> desiredVariation,
                std::map<MediatorId, MediatorValue> minimalMediatorValues);

        /**
         * Checks whether specified transition can be used for the desired input (context) state transition
         * @param desiredContext desired input context
         * @param mediators current mediators state
         * @return true if transition is possible, false otherwise
         */
        bool IsViable(const std::map<NeuralInputId, NeuralInput> &currentContext,
                const std::map<NeuralInputId, NeuralInput> &desiredContext,
                const std::map<MediatorId, MediatorValue> &mediators) const;

        /**
         * Basic cortex transitions keep initial context variation without
         * exact goal but this method allows to create targeted transition that are keep in a decision making core of the cortex
         * @param initialContext fixed initial context (variations will be empty)
         * @param desiredContext
         * @return
         */
        CortexTargetedTransition CreateInstance(const std::map<NeuralInputId, NeuralInput> &initialContext,
                const std::map<NeuralInputId, NeuralInput> &desiredContext) const;

        inline std::string GetId() const { return _id; }
        inline std::string GetActivityId() const { return _activityId; }

    private:
        std::string _id;
        std::string _activityId;
        std::map<NeuralInputId, NeuralInput> _initialSequence;
        std::map<NeuralInputId, float_fl> _initialVariation;
        std::map<NeuralInputId, NeuralInput> _desiredSequence;
        std::map<NeuralInputId, float_fl> _desiredVariation;
        std::map<MediatorId, MediatorValue> _minimalMediatorValues;
    };
}

#endif //FLUXNEAT_CORTEX_TRANSITION_H
