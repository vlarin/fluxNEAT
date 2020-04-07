#include <flux/cortex/cortex_decision_layer.h>

#include <utility>
#include <iostream>

flux::CortexDecisionLayer::CortexDecisionLayer() : _currentTransitionLifetime(0), _isWandering(false)
{
}

void flux::CortexDecisionLayer::AddBuiltinTransition(const flux::CortexTransition &transition)
{
    _transitions.push_back(transition);
}

const flux::CortexTargetedTransition &flux::CortexDecisionLayer::GetCurrentTransition() const
{
    return _currentTransition;
}

void flux::CortexDecisionLayer::Step(const std::map<NeuralNodeId, NeuralNode> &context,
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

bool flux::CortexDecisionLayer::TryMakeDecision(const std::map<NeuralNodeId, NeuralNode> &context,
                                                const std::map<NeuralNodeId, NeuralNode> &desiredContext,
                                                const std::map<MediatorId, MediatorValue> &mediators)
{
    //TODO: add inertia, transitions multi-dim cashing
    for (const auto &transition : _transitions)
    {
        if (transition.IsViable(context, desiredContext, mediators))
        {
            _currentTransition = transition.CreateInstance(context, desiredContext);
            _currentTransitionLifetime = 0;
            _isWandering = false;
            std::cout << "Target transition used." << std::endl;
            return true;
        }
    }

    return false;
}

void flux::CortexDecisionLayer::StartWandering(const std::map<NeuralNodeId, NeuralNode> &context,
                                               const std::map<MediatorId, MediatorValue> &mediators)
{
    //TODO: add inertia, transitions multi-dim cashing, handle no available transitions case
    for (const auto &transition : _transitions)
    {
        if (transition.IsWanderingViable(context, mediators))
        {
            _currentTransition = transition.CreateWanderingInstance(context);
            _currentTransitionLifetime = 0;
            _isWandering = true;
            std::cout << "Wandering to " << _currentTransition.GetDesiredContext().find(NeuralNodeId("c"))->second.GetValue() << std::endl;
        }
    }
}
