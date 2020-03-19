#include <flux/cortex/cortex_column.h>

#include <utility>
#include <flux/feedback/mediator_value.h>

static const flux::float_fl MEDIATION_THRESHOLD_SQR = 0.1;

static flux::float_fl CalculateMediationSqrDifference(const std::vector<flux::MediatorValue>& from, const std::vector<flux::MediatorValue>& to)
{
    flux::float_fl difference = 0;
    for (int i = 0; i < from.size(); ++i)
    {
        difference += abs(from[i].GetValue() - to[i].GetValue());
    }

    return difference;
}

flux::CortexColumn::CortexColumn(size_t id,
                                 const std::map<NeuralInputId, NeuralInput> &immediateContext,
                                 std::map<MediatorId, MediatorValue> mediators)
                                 : _id (id), _mediatorsAverages(std::move(mediators)),
                                 _sensorsHi(immediateContext),
                                 _sensorsLow(immediateContext) {}

bool flux::CortexColumn::TryMerge(const flux::ICortexFeedback &cortexFeedback,
                                  const std::map<NeuralInputId, NeuralInput> &context,
                                  const std::map<MediatorId, MediatorValue> &mediators)
{
    std::vector<MediatorValue> averages;
    for (const auto &average : _mediatorsAverages)
    {
        averages.emplace_back(average.second);
    }

    std::map<NeuralInputId, NeuralInput> newLow = _sensorsLow;
    for (const auto &input : context)
    {
        const auto oldLow = newLow.find(input.first);
        if (oldLow != newLow.end() && input.second.GetValue() < oldLow->second.GetValue())
        {
            newLow[input.first].Reset();
            newLow[input.first].Apply(input.second.GetValue());
        }
        if (oldLow == newLow.end())
        {
            newLow.insert(input);
        }
    }

    auto mediationLow = cortexFeedback.FetchFeedback(newLow);
    if (CalculateMediationSqrDifference(mediationLow, averages) > MEDIATION_THRESHOLD_SQR)
    {
        return false;
    }

    std::map<NeuralInputId, NeuralInput> newHigh = _sensorsHi;
    for (const auto &input : context)
    {
        const auto oldLow = newHigh.find(input.first);
        if (oldLow != newHigh.end() && input.second.GetValue() < oldLow->second.GetValue())
        {
            newHigh[input.first].Reset();
            newHigh[input.first].Apply(input.second.GetValue());
        }
        if (oldLow == newHigh.end())
        {
            newHigh.insert(input);
        }
    }

    auto mediationHi = cortexFeedback.FetchFeedback(newHigh);
    if (CalculateMediationSqrDifference(mediationHi, averages) > MEDIATION_THRESHOLD_SQR)
    {
        return false;
    }

    _sensorsLow = newLow;
    _sensorsHi = newHigh;
    //TODO: update averages
    return true;
}
