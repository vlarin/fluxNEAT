#include <flux/cortex/cortex_transition.h>

#include <armadillo>

flux::CortexTransition::CortexTransition(std::string id, std::string activityId,
                                         std::map<NeuralInputId, NeuralInput> initialSequence,
                                         std::map<NeuralInputId, float_fl> initialVariation,
                                         std::map<NeuralInputId, NeuralInput> desiredSequence,
                                         std::map<NeuralInputId, float_fl> desiredVariation,
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
flux::CortexTransition::CreateInstance(const std::map<NeuralInputId, NeuralInput> &initialContext,
                                       const std::map<NeuralInputId, NeuralInput> &desiredContext) const
{
    return CortexTargetedTransition(_id, _activityId, initialContext, desiredContext);
}


flux::CortexTargetedTransition
flux::CortexTransition::CreateWanderingInstance(const std::map<NeuralInputId, NeuralInput> &initialContext) const
{
    auto mutatedDesiredSequence = _desiredSequence;
    for (auto &desired : mutatedDesiredSequence)
    {
        const auto variation = _desiredVariation.find(desired.first);
        if (variation != _desiredVariation.end())
        {
            const float_fl random = arma::mat(1, 1, arma::fill::randu)(0,0);
            desired.second.Apply(variation->second * random - (variation->second / 2.0));
        }
    }
    return CreateInstance(initialContext, mutatedDesiredSequence);
}

bool flux::CortexTransition::IsViable(const std::map<NeuralInputId, NeuralInput> &currentContext,
                                      const std::map<NeuralInputId, NeuralInput> &desiredContext,
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

    //check initial context allowance
    for (const auto &input : currentContext)
    {
        auto rangedInput = _initialSequence.find(input.first);
        auto range = _initialVariation.find(input.first);
        if (rangedInput != _initialSequence.end() && fabs(input.second.GetValue() - rangedInput->second.GetValue()) > range->second)
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

bool flux::CortexTransition::IsWanderingViable(const std::map<NeuralInputId, NeuralInput> &currentContext,
                                               const std::map<MediatorId, MediatorValue> &mediators) const
{
    return IsViable(currentContext, std::map<NeuralInputId, NeuralInput>(), mediators);
}