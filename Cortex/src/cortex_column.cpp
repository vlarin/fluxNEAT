#include <flux/cortex/cortex_column.h>

#include <utility>
#include <flux/feedback/mediator_value.h>

static const flux::float_fl MEDIATION_THRESHOLD_SQR = 0.1;
static const flux::float_fl STANDARD_RADIUS = 0.1;
static const flux::float_fl STANDARD_FALLOFF = 0.1;
static const flux::float_fl STANDARD_DECAY = 0.7;
static const flux::float_fl MAX_EXCITEMENT = 2.0;

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
                                 _excitementLevel(0.0),
                                 _pivot(immediateContext),
                                 _centroid(immediateContext),
                                 _radius(STANDARD_RADIUS) {}

bool flux::CortexColumn::TryMerge(const std::map<NeuralInputId, NeuralInput> &context,
                                  const std::map<MediatorId, MediatorValue> &mediators)
{
    std::vector<MediatorValue> averages;
    averages.reserve(_mediatorsAverages.size());
    for (const auto &average : _mediatorsAverages)
    {
        averages.emplace_back(average.second);
    }

    std::vector<MediatorValue> currentMediators;
    currentMediators.reserve(mediators.size());
    for (const auto &mediator : mediators)
    {
        currentMediators.emplace_back(mediator.second);
    }

    bool isMediationFitting = CalculateMediationSqrDifference(currentMediators, averages) <= MEDIATION_THRESHOLD_SQR;

    float_fl activationValue = CalculateActivationValue(context);
    if (activationValue >= 1 && isMediationFitting)
    {
        return true;
    }
    if (activationValue < 1 && !isMediationFitting)
    {
        return false;
    }
    if (activationValue < 1 && isMediationFitting)
    {
        //activation is not fit while mediators are ok try to make greedy extension
        std::map<NeuralInputId, NeuralInput> newCentroid;
        float_fl newRadius = 0;
        for (const auto &inputKey : context)
        {
            newCentroid.insert(std::make_pair(inputKey.first,
                    NeuralInput(inputKey.first, (_centroid[inputKey.first].GetValue() + inputKey.second.GetValue()) / 2.0)));
            float_fl delta = abs(_centroid[inputKey.first].GetValue() - inputKey.second.GetValue());
            newRadius += delta * delta;
        }

        newRadius = sqrt(newRadius) / 2.0 + _radius;

        //check upper limit (bound)
        for (const auto &upperLimit : _limitUpper)
        {
            if (newCentroid[upperLimit.first].GetValue() + newRadius >= upperLimit.second.GetValue())
            {
                //growth is already constrained in that direction, unable to merge
                return false;
            }
        }

        //check lower limit (bound)
        for (const auto &lowerLimit : _limitLower)
        {
            if (newCentroid[lowerLimit.first].GetValue() - newRadius <= lowerLimit.second.GetValue())
            {
                //growth is already constrained in that direction, unable to merge
                return false;
            }
        }

        //merge is possible, so proceed
        _centroid = newCentroid;
        _radius = newRadius;
        return true;
    }
    if (activationValue >= 1 && !isMediationFitting)
    {
        //column is activated but mediator levels is not fit, we need to shrink
        //TODO: Draw hypersphere around not fitting point and set center at closest to centroid point to maximize saved hypersphere volume
        //TODO (advanced): Split immediately into multiple hyperspheres to preserve ~all gained volume

        //current solution: fallback to pivot and update limits
        float_fl pivotToCenterDistance = 0;
        for (const auto &inputKey : _pivot)
        {
            float_fl delta = abs(_centroid[inputKey.first].GetValue() - inputKey.second.GetValue());
            pivotToCenterDistance += delta * delta;
        }
        pivotToCenterDistance = sqrt(pivotToCenterDistance);

        _centroid = _pivot;
        float_fl newRadius = 0;
        for (const auto &inputKey : context)
        {
            float_fl delta = abs(_centroid[inputKey.first].GetValue() - inputKey.second.GetValue());
            newRadius += delta * delta;
        }

        newRadius = sqrt(newRadius) - 0.000001;
        _radius = fmin(newRadius, _radius - pivotToCenterDistance);

        //update limits
        for (const auto &inputKey : context)
        {
            if (_centroid[inputKey.first].GetValue() < inputKey.second.GetValue())
            {
                bool exists = _limitUpper.find(inputKey.first) != _limitUpper.end();
                if (exists && _limitUpper[inputKey.first].GetValue() > inputKey.second.GetValue())
                {
                    _limitUpper[inputKey.first] = inputKey.second;
                }
                else
                {
                    _limitUpper.insert(inputKey);
                }
            }

            if (_centroid[inputKey.first].GetValue() > inputKey.second.GetValue())
            {
                bool exists = _limitLower.find(inputKey.first) != _limitLower.end();
                if (exists && _limitLower[inputKey.first].GetValue() > inputKey.second.GetValue())
                {
                    _limitLower[inputKey.first] = inputKey.second;
                }
                else
                {
                    _limitLower.insert(inputKey);
                }
            }
        }

    }
    return false;
}

void flux::CortexColumn::Step(const std::map<NeuralInputId, NeuralInput> &context,
                              const std::map<MediatorId, MediatorValue> &mediators)
{
    TryMerge(context, mediators);
    _excitementLevel += CalculateActivationValue(context);
    _excitementLevel *= STANDARD_DECAY;

    if (_excitementLevel > MAX_EXCITEMENT)
    {
        _excitementLevel = MAX_EXCITEMENT;
    }
}

bool flux::CortexColumn::operator <(const flux::CortexColumn &other) const
{
    return _excitementLevel < other._excitementLevel;
}

bool flux::CortexColumn::operator >(const flux::CortexColumn &other) const
{
    return _excitementLevel > other._excitementLevel;
}

flux::float_fl flux::CortexColumn::CalculateActivationValue(const std::map<NeuralInputId, NeuralInput> &context)
{
    float_fl activationValue = 0;
    float_fl radiusSquared = _radius * _radius;
    for (const auto &inputKey : context)
    {
        float_fl delta = abs(_centroid[inputKey.first].GetValue() - inputKey.second.GetValue());
        activationValue += delta * delta / radiusSquared;
    }

    activationValue = sqrt(activationValue);
    if (activationValue < 1)
    {
        return 1;
    }
    if (activationValue > 1 && activationValue <= 1.1)
    {
        return 1 - ((activationValue - 1) * (1 - STANDARD_FALLOFF) * 10);
    }
    return 1 - (activationValue - 1.1) / 5.0;
}
