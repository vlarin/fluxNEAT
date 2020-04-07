//
// Copyright (c) 2019-2020 Temporal Games Inc. All rights reserved.
//

#ifndef FLUXNEAT_CORTEX_TARGETED_TRANSITION_H
#define FLUXNEAT_CORTEX_TARGETED_TRANSITION_H

#include <flux/prereq.h>
#include <flux/neural_node.h>
#include <flux/feedback/mediator_value.h>

namespace flux
{
    /**
     * Instanced rule of desired input transition. Based on discovered invariants present in activities.
     * For detailed info of a basic transition subsystem see @see CortexTransition
     */
    class FLUX_API CortexTargetedTransition
    {
    public:
        explicit CortexTargetedTransition(std::string id,
                                  std::string activityId,
                                  std::map<NeuralNodeId, NeuralNode> initialSequence,
                                  std::map<NeuralNodeId, NeuralNode> desiredSequence);
        explicit CortexTargetedTransition();

        inline std::string GetId() const { return _id; }
        inline std::string GetActivityId() const { return _activityId; }
        inline const std::map<NeuralNodeId, NeuralNode> &GetInitialContext() const { return _initialSequence; }
        inline const std::map<NeuralNodeId, NeuralNode> &GetDesiredContext() const { return _expectedSequence; }

    private:
        std::string _id;
        std::string _activityId;
        std::map<NeuralNodeId, NeuralNode> _initialSequence;
        std::map<NeuralNodeId, NeuralNode> _expectedSequence;
    };
}

#endif //FLUXNEAT_CORTEX_TARGETED_TRANSITION_H
