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

void flux::NeatActivityTrainer::Step()
{
    _impl->Step();
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

float_t flux::NeatActivityTrainer::GetChampionFitness() const
{
    return _impl->GetChampionFitness();
}

flux::NeatActivityTrainer::~NeatActivityTrainer() = default;