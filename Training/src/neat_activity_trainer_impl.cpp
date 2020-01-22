#include "include/neat_activity_trainer_impl.h"

#include <utility>

static flux::NeatEmptyTask empty;

flux::NeatActivityTrainer::NeatActivityTrainerImpl::NeatActivityTrainerImpl(size_t populationSize,
        size_t numSpecies,
        size_t parallelPoolSize,
        size_t complexityThreshold,
        size_t maxSimplifyGeneration,
        flux::NeatEvolutionParameters evolutionParameters,
        std::shared_ptr<IEvaluationOutputUnit> trainingFitnessUnitProto,
        std::shared_ptr<IBlackBox> trainingProto,
        std::shared_ptr<IContextRegistry> trainingPool,
        const std::shared_ptr<IActivityUnit> &target)
        : _populationSize(populationSize),
        _parallelPoolSize(parallelPoolSize),
        _trainingFitnessUnitProto(std::move(trainingFitnessUnitProto)),
        _trainingProto(std::move(trainingProto)),
        _trainingPool(std::move(trainingPool)),
        _targetId(target->GetId()),
        _training(empty, target->GetInputIds().size(), target->GetOutputIds().size(), populationSize,
                0 /** dest fitness */ ,
                numSpecies,
                1 /** initial weight */,
                evolutionParameters.InitialWeight,
                evolutionParameters.WeightMutationProb,
                evolutionParameters.WeightMutationSize,
                evolutionParameters.BiasMutationProb,
                evolutionParameters.BiasMutationSize,
                evolutionParameters.NodeAdditionProb,
                evolutionParameters.ConnAdditionProb,
                evolutionParameters.ConnDeletionProb,
                0.2 /** disableProb  */,
                evolutionParameters.ElitismProportion,
                0 /** finalFitness */,
                complexityThreshold,
                maxSimplifyGeneration,
                evolutionParameters.IsAcyclic)

{
    // Initialize all genomes to evaluate
    _training.ForceInit();
}

bool flux::NeatActivityTrainer::NeatActivityTrainerImpl::IsEpochCompleted() const
{
    return _currentEvaluations.empty() && _evaluationQueue.empty();
}

void flux::NeatActivityTrainer::NeatActivityTrainerImpl::SaveCurrentChampionActivity(std::ostream &stream) const
{
    boost::archive::binary_oarchive ar(stream);
    ar << _currentChampion;
}

void flux::NeatActivityTrainer::NeatActivityTrainerImpl::SaveEvolutionState(std::ostream &stream) const
{
    boost::archive::binary_oarchive ar(stream);
    ar << _training;
}

void flux::NeatActivityTrainer::NeatActivityTrainerImpl::LoadEvolutionState(std::istream &stream)
{
    boost::archive::binary_iarchive ar(stream);
    ar >> _training;

    //TODO: wipe current evaluations & epoch
}

void flux::NeatActivityTrainer::NeatActivityTrainerImpl::Step()
{
    if (IsEpochCompleted())
    {
        StartEpoch();
    }

    for (int i = _currentEvaluations.size() - 1; i >= 0; i--)
    {
        _currentEvaluations[i].Entity->Step();

        if (_currentEvaluations[i].FitnessEvaluatorUnit->IsEvaluationCompleted())
        {
            auto &genome = _training.Genomes()[_currentEvaluations[i].Index];
            genome.Fitness() = _currentEvaluations[i].FitnessEvaluatorUnit->GetAggregatedFitness();

            _trainingPool->ReleaseContext(*_currentEvaluations[i].Entity->GetContext());
            _currentEvaluations.erase(_currentEvaluations.begin() + i);
        }
    }

    while (_currentEvaluations.size() < _parallelPoolSize && !_evaluationQueue.empty())
    {
        EmplaceForEvaluation(_evaluationQueue.front());
        _evaluationQueue.erase(_evaluationQueue.begin());
    }

    if (IsEpochCompleted())
    {
        EndEpoch();
    }
}

void flux::NeatActivityTrainer::NeatActivityTrainerImpl::StartEpoch()
{
    for (auto i = 0; i < _training.Genomes().size(); ++i)
    {
        EvaluationEntry entry;
        entry.Index = i;
        _evaluationQueue.emplace_back(entry);
    }
}

void flux::NeatActivityTrainer::NeatActivityTrainerImpl::EndEpoch()
{
    _training.FinalizePreEvaluatedStep(_currentChampion);
}

void flux::NeatActivityTrainer::NeatActivityTrainerImpl::EmplaceForEvaluation(EvaluationEntry entry)
{
    std::shared_ptr<IContext> slot = _trainingPool->RetrieveContext();
    entry.FitnessEvaluatorUnit = std::static_pointer_cast<IEvaluationOutputUnit>(_trainingFitnessUnitProto->Clone(slot));
    entry.Entity = std::static_pointer_cast<IBlackBox>(_trainingProto->Clone(slot));
    entry.Entity->AddOutput(entry.FitnessEvaluatorUnit);

    std::stringstream stream;
    boost::archive::binary_oarchive oar(stream);
    oar << _training.Genomes()[entry.Index];

    entry.Entity->UpdateChildScheme(_targetId, stream);

    _currentEvaluations.emplace_back(entry);
}

float_t flux::NeatActivityTrainer::NeatActivityTrainerImpl::GetChampionFitness() const
{
    return _currentChampion.Fitness();
}
