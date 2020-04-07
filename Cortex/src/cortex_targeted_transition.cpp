#include <flux/cortex/cortex_targeted_transition.h>

flux::CortexTargetedTransition::CortexTargetedTransition(std::string id, std::string activityId,
                                                         std::map<NeuralNodeId, NeuralNode> initialSequence,
                                                         std::map<NeuralNodeId, NeuralNode> desiredSequence)
                                                         : _id(std::move(id)), _activityId(std::move(activityId)),
                                                         _initialSequence(std::move(initialSequence)),
                                                         _expectedSequence(std::move(desiredSequence)) {}

flux::CortexTargetedTransition::CortexTargetedTransition() = default;
