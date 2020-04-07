#include <flux/cortex/cortex_transition.h>

#include <armadillo>

flux::CortexTransition::CortexTransition(std::string id, std::string activityId,
                                         std::map<NeuralNodeId, NeuralNode> initialSequence,
                                         std::map<NeuralNodeId, float_fl> initialVariation,
                                         std::map<NeuralNodeId, NeuralNode> desiredSequence,
                                         std::map<NeuralNodeId, float_fl> desiredVariation,
                                         std::map<MediatorId, MediatorValue> minimalMediatorValues)
                                         : _id(std::move(id)), _activityId(std::move(activityId)),
                                           _initialSequence(std::move(initialSequence)),
                                           _initialVariation(std::move(initialVariation)),
                                           _desiredSequence(std::move(desiredSequence)),
                                           _desiredVariation(std::move(desiredVariation)),
                                           _minimalMediatorValues(std::move(minimalMediatorValues))
{
}

flux::CortexTargetedTransition
flux::CortexTransition::CreateInstance(const std::map<NeuralNodeId, NeuralNode> &initialContext,
                                       const std::map<NeuralNodeId, NeuralNode> &desiredContext) const
{
    return CortexTargetedTransition(_id, _activityId, initialContext, desiredContext);
}


flux::CortexTargetedTransition
flux::CortexTransition::CreateWanderingInstance(const std::map<NeuralNodeId, NeuralNode> &initialContext) const
{
    auto mutatedDesiredSequence = _desiredSequence;
    for (auto &desired : mutatedDesiredSequence)
    {
        const auto variation = _desiredVariation.find(desired.first);
        if (variation != _desiredVariation.end())
        {
            float_fl random = arma::mat(1, 1, arma::fill::randu)(0,0);
            desired.second.Apply((2 * variation->second) * random - variation->second);
        }
    }
    return CreateInstance(initialContext, mutatedDesiredSequence);
}

bool flux::CortexTransition::IsViable(const std::map<NeuralNodeId, NeuralNode> &currentContext,
                                      const std::map<NeuralNodeId, NeuralNode> &desiredContext,
                                      const std::map<MediatorId, MediatorValue> &mediators) const
{
    //check minimal mediators allowance
    for (const auto &mediator : mediators)
    {
        auto minMediator = _minimalMediatorValues.find(mediator.first);
        if (minMediator != _minimalMediatorValues.end() && minMediator->second.GetValue() > mediator.second.GetValue())
        {
            return false;
        }
    }

    //check initial and cross context allowance
    for (const auto &input : currentContext)
    {
        auto rangedInput = _initialSequence.find(input.first);
        auto range = _initialVariation.find(input.first);
        auto desired = desiredContext.find(input.first);
        if (rangedInput != _initialSequence.end() && fabs(input.second.GetValue() - rangedInput->second.GetValue()) > range->second)
        {
            return false;
        }

        //cross context disallowance
        if (rangedInput == _initialSequence.end() && desired != desiredContext.end() &&
            fabs(input.second.GetValue() - desired->second.GetValue()) > 0.001)
        {
            return false;
        }
    }

    //check desired context allowance
    for (const auto &input : desiredContext)
    {
        auto rangedInput = _desiredSequence.find(input.first);
        auto range = _desiredVariation.find(input.first);
        if (rangedInput != _desiredSequence.end() && fabs(input.second.GetValue() - rangedInput->second.GetValue()) > range->second)
        {
            return false;
        }
    }

    return true;
}

bool flux::CortexTransition::IsWanderingViable(const std::map<NeuralNodeId, NeuralNode> &currentContext,
                                               const std::map<MediatorId, MediatorValue> &mediators) const
{
    return IsViable(currentContext, std::map<NeuralNodeId, NeuralNode>(), mediators);
}