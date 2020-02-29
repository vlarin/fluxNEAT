#include <flux/training/neat_activity_trainer.h>

#include <memory>
#include "include/neat_activity_trainer_impl.h"

flux::NeatActivityTrainer::NeatActivityTrainer(size_t populationSize,
        size_t numSpecies,
        size_t parallelPoolSize,
        size_t complexityThreshold,
        size_t maxSimplifyGeneration,
        NeatEvolutionParameters evolutionParameters,
        const std::shared_ptr<IActivityUnit> &target,
        const std::shared_ptr<IEvaluationOutputUnit>& trainingFitnessUnitProto,
        const std::shared_ptr<IBlackBox>& trainingProto,
        const std::shared_ptr<IContextRegistry>& trainingPool)
        : _impl(std::make_unique<NeatActivityTrainerImpl>(populationSize, numSpecies, parallelPoolSize,
                complexityThreshold, maxSimplifyGeneration, evolutionParameters,
                trainingFitnessUnitProto, trainingProto, trainingPool, target)) {}

bool flux::NeatActivityTrainer::IsEpochCompleted() const
{
    return _impl->IsEpochCompleted();
}

size_t flux::NeatActivityTrainer::GetCurrentEpoch() const
{
    return _impl->GetCurrentEpoch();
}

bool flux::NeatActivityTrainer::IsComplexifying() const
{
	return _impl->IsComplexifying();
}

void flux::NeatActivityTrainer::Step(flux::float_fl delta)
{
    _impl->Step(delta);
}

void flux::NeatActivityTrainer::SaveCurrentChampionActivity(std::ostream &stream) const
{
    _impl->SaveCurrentChampionActivity(stream);
}

void flux::NeatActivityTrainer::SaveEvolutionState(std::ostream &stream) const
{
    _impl->SaveEvolutionState(stream);
}

void flux::NeatActivityTrainer::LoadEvolutionState(std::istream &stream)
{
    _impl->LoadEvolutionState(stream);
}

flux::float_fl flux::NeatActivityTrainer::GetChampionFitness() const
{
    return _impl->GetChampionFitness();
}

flux::NeatEntityDescriptor flux::NeatActivityTrainer::GetChampionEntity() const
{
    return _impl->GetChampionEntity();
}

std::vector<flux::NeatEntityDescriptor> flux::NeatActivityTrainer::GetCurrentEntities() const
{
    return _impl->GetCurrentEntities();
}

flux::float_fl flux::NeatActivityTrainer::GetBestFitness() const
{
    return _impl->GetBestFitness();
}

flux::float_fl flux::NeatActivityTrainer::GetMeanFitness() const
{
    return _impl->GetMeanFitness();
}

flux::float_fl flux::NeatActivityTrainer::GetMeanComplexity() const
{
    return _impl->GetMeanComplexity();
}

flux::float_fl flux::NeatActivityTrainer::GetEvaluationPerSec() const
{
    return _impl->GetEvaluationPerSec();
}

flux::float_fl flux::NeatActivityTrainer::GetTotalEvaluations() const
{
    return _impl->GetTotalEvaluations();
}

flux::float_fl flux::NeatActivityTrainer::GetTotalEvaluationTime() const
{
    return _impl->GetTotalEvaluationTime();
}

flux::float_fl flux::NeatActivityTrainer::GetMeanEvaluationDuration() const
{
    return _impl->GetMeanEvaluationDuration();
}

const flux::NeatEvolutionParameters &flux::NeatActivityTrainer::GetEvolutionParameters() const
{
    return _impl->GetEvolutionParameters();
}

void flux::NeatActivityTrainer::DumpState(std::ostream &stream) const
{
    _impl->DumpState(stream);
}

flux::NeatActivityTrainer::~NeatActivityTrainer() = default;