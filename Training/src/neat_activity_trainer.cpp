#include <flux/training/neat_activity_trainer.h>

#include <memory>
#include "include/neat_activity_trainer_impl.h"
#include "include/hyper_neat_activity_trainer_impl.h"

flux::NeatActivityTrainer::NeatActivityTrainer(size_t populationSize,
        size_t numSpecies,
        size_t parallelPoolSize,
        size_t complexityThreshold,
        size_t maxSimplifyGeneration,
        size_t cppnDimensions,
        NeatEvolutionParameters evolutionParameters,
        std::vector<size_t> substrateLayers,
        const std::shared_ptr<IActivityUnit> &target,
        const std::shared_ptr<IEvaluationOutputUnit>& trainingFitnessUnitProto,
        const std::shared_ptr<IBlackBox>& trainingProto,
        const std::shared_ptr<IContextRegistry>& trainingPool)
        : _impl(std::make_unique<NeatActivityTrainerImpl>(populationSize, numSpecies, parallelPoolSize,
                  complexityThreshold, maxSimplifyGeneration, evolutionParameters,
                  trainingFitnessUnitProto, trainingProto, trainingPool, target)),
          _himpl(std::make_unique<HyperNeatActivityTrainerImpl>(populationSize, numSpecies, parallelPoolSize,
                complexityThreshold, maxSimplifyGeneration, cppnDimensions, evolutionParameters, substrateLayers,
                trainingFitnessUnitProto, trainingProto, trainingPool, target)),
          _isHyper(substrateLayers.size() > 0) {}

bool flux::NeatActivityTrainer::IsEpochCompleted() const
{
    return _isHyper ? _himpl->IsEpochCompleted() : _impl->IsEpochCompleted();
}

size_t flux::NeatActivityTrainer::GetCurrentEpoch() const
{
    return _isHyper ? _himpl->GetCurrentEpoch() : _impl->GetCurrentEpoch();
}

bool flux::NeatActivityTrainer::IsComplexifying() const
{
	return _isHyper ? _himpl->IsComplexifying() : _impl->IsComplexifying();
}

void flux::NeatActivityTrainer::Step(flux::float_fl delta)
{
    if (_isHyper)
    {
        _himpl->Step(delta);
    }
    else
    {
        _impl->Step(delta);
    }
}

void flux::NeatActivityTrainer::SaveCurrentChampionActivity(std::ostream &stream) const
{
    if (_isHyper)
    {
        _himpl->SaveCurrentChampionActivity(stream);
    }
    else
    {
        _impl->SaveCurrentChampionActivity(stream);
    }
}

void flux::NeatActivityTrainer::SaveEvolutionState(std::ostream &stream) const
{
    if (_isHyper)
    {
        _himpl->SaveEvolutionState(stream);
    }
    else
    {
        _impl->SaveEvolutionState(stream);
    }
}

void flux::NeatActivityTrainer::LoadEvolutionState(std::istream &stream)
{
    if (_isHyper)
    {
        _himpl->LoadEvolutionState(stream);
    }
    else
    {
        _impl->LoadEvolutionState(stream);
    }
}

flux::float_fl flux::NeatActivityTrainer::GetChampionFitness() const
{
    return _isHyper ? _himpl->GetChampionFitness() : _impl->GetChampionFitness();
}

flux::NeatEntityDescriptor flux::NeatActivityTrainer::GetChampionEntity() const
{
    return _isHyper ? _himpl->GetChampionEntity() : _impl->GetChampionEntity();
}

std::vector<flux::NeatEntityDescriptor> flux::NeatActivityTrainer::GetCurrentEntities() const
{
    return _isHyper ? _himpl->GetCurrentEntities() : _impl->GetCurrentEntities();
}

flux::float_fl flux::NeatActivityTrainer::GetBestFitness() const
{
    return _isHyper ? _himpl->GetBestFitness() : _impl->GetBestFitness();
}

flux::float_fl flux::NeatActivityTrainer::GetMeanFitness() const
{
    return _isHyper ? _himpl->GetMeanFitness() : _impl->GetMeanFitness();
}

flux::float_fl flux::NeatActivityTrainer::GetMeanComplexity() const
{
    return _isHyper ? _himpl->GetMeanComplexity() : _impl->GetMeanComplexity();
}

flux::float_fl flux::NeatActivityTrainer::GetEvaluationPerSec() const
{
    return _isHyper ? _himpl->GetEvaluationPerSec() : _impl->GetEvaluationPerSec();
}

flux::float_fl flux::NeatActivityTrainer::GetTotalEvaluations() const
{
    return _isHyper ? _himpl->GetTotalEvaluations() : _impl->GetTotalEvaluations();
}

flux::float_fl flux::NeatActivityTrainer::GetTotalEvaluationTime() const
{
    return _isHyper ? _himpl->GetTotalEvaluationTime() : _impl->GetTotalEvaluationTime();
}

flux::float_fl flux::NeatActivityTrainer::GetMeanEvaluationDuration() const
{
    return _isHyper ? _himpl->GetMeanEvaluationDuration() : _impl->GetMeanEvaluationDuration();
}

const flux::NeatEvolutionParameters &flux::NeatActivityTrainer::GetEvolutionParameters() const
{
    return _isHyper ? _himpl->GetEvolutionParameters() : _impl->GetEvolutionParameters();
}

void flux::NeatActivityTrainer::DumpState(std::ostream &stream) const
{
    if (_isHyper)
    {
        _himpl->DumpState(stream);
    }
    else
    {
        _impl->DumpState(stream);
    }
}

flux::NeatActivityTrainer::~NeatActivityTrainer() = default;