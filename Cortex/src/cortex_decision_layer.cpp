#include <flux/cortex/cortex_decision_layer.h>

#include <utility>

void flux::CortexDecisionLayer::AddBuiltinTransition(const flux::CortexTransition &transition)
{
    _transitions.push_back(transition);
}

const flux::CortexTargetedTransition &flux::CortexDecisionLayer::GetCurrentTransition() const
{
    return _currentTransition;
}

void flux::CortexDecisionLayer::Step(const std::map<NeuralInputId, NeuralInput> &context,
                                     const std::map<MediatorId, MediatorValue> &mediators)
{
    _currentTransitionLifetime++;

}

void flux::CortexDecisionLayer::SetActivityBound(std::string activityId)
{
    _activityBound = std::move(activityId);
}

void flux::CortexDecisionLayer::ResetActivityBound()
{
    _activityBound = "";
}
