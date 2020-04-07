//
// Copyright (c) 2019-2020 Temporal Games Inc. All rights reserved.
//

#ifndef FLUXNEAT_CORTEX_DECISION_LAYER_H
#define FLUXNEAT_CORTEX_DECISION_LAYER_H

#include <flux/prereq.h>
#include <flux/cortex/cortex_transition.h>
#include <flux/cortex/cortex_targeted_transition.h>

namespace flux
{
    /**
     * Level of the high-level columns organization. Keeps available invariants - transitions and selects best to use
     */
    class FLUX_API CortexDecisionLayer
    {
    public:
        CortexDecisionLayer();

        void AddBuiltinTransition(const CortexTransition &transition);

        void Step(const std::map<NeuralNodeId, NeuralNode> &context,
                  const std::map<MediatorId, MediatorValue> &mediators);

        bool TryMakeDecision(const std::map<NeuralNodeId, NeuralNode> &context,
                             const std::map<NeuralNodeId, NeuralNode> &desiredContext,
                             const std::map<MediatorId, MediatorValue> &mediators);

        void StartWandering(const std::map<NeuralNodeId, NeuralNode> &context,
                             const std::map<MediatorId, MediatorValue> &mediators);

        const CortexTargetedTransition &GetCurrentTransition() const;

        void SetActivityBound(std::string activityId);
        void ResetActivityBound();
    private:
        std::vector<CortexTransition> _transitions;

        bool _isWandering;
        CortexTargetedTransition _currentTransition;
        size_t _currentTransitionLifetime;

        std::string _activityBound;
    };
}

#endif //FLUXNEAT_CORTEX_DECISION_LAYER_H
