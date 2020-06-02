#include "include/hyper_neat_activity_trainer_impl.h"

#include <utility>
#include <algorithm>

static flux::HyperNeatEmptyTask empty;

flux::NeatActivityTrainer::HyperNeatActivityTrainerImpl::HyperNeatActivityTrainerImpl(size_t populationSize,
        size_t numSpecies,
        size_t parallelPoolSize,
        size_t complexityThreshold,
        size_t maxSimplifyGeneration,
        size_t cppnDimensions,
        flux::NeatEvolutionParameters evolutionParameters,
        std::vector<size_t> substrateLayers,
        std::shared_ptr<IEvaluationOutputUnit> trainingFitnessUnitProto,
        std::shared_ptr<IBlackBox> trainingProto,
        std::shared_ptr<IContextRegistry> trainingPool,
        const std::shared_ptr<IActivityUnit> &target)
        : _populationSize(populationSize),
        _parallelPoolSize(parallelPoolSize),
        _evolutionParameters(evolutionParameters),
        _substrateLayers(substrateLayers),
        _trainingFitnessUnitProto(std::move(trainingFitnessUnitProto)),
        _trainingProto(std::move(trainingProto)),
        _trainingPool(std::move(trainingPool)),
        _targetId(target->GetId()),
        _training(empty, substrateLayers, (cppnDimensions + 1) * 2, 1, populationSize,
                0 /** max generations (unused) */ ,
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
                true)

{
    // Initialize all genomes to evaluate
    _training.ForceInit();
}

bool flux::NeatActivityTrainer::HyperNeatActivityTrainerImpl::IsEpochCompleted() const
{
    return _currentEvaluations.empty() && _evaluationQueue.empty();
}

size_t flux::NeatActivityTrainer::HyperNeatActivityTrainerImpl::GetCurrentEpoch() const
{
    return _epochNumber;
}

bool flux::NeatActivityTrainer::HyperNeatActivityTrainerImpl::IsComplexifying() const
{
	return _training.IsComplexifying();
}

const flux::NeatEvolutionParameters &flux::NeatActivityTrainer::HyperNeatActivityTrainerImpl::GetEvolutionParameters() const {
    return _evolutionParameters;
}

void flux::NeatActivityTrainer::HyperNeatActivityTrainerImpl::SaveCurrentChampionActivity(std::ostream &stream) const
{
    boost::archive::text_oarchive ar(stream);
    ar << _currentChampion;
}

void flux::NeatActivityTrainer::HyperNeatActivityTrainerImpl::SaveEvolutionState(std::ostream &stream) const
{
    boost::archive::binary_oarchive ar(stream);

    ar << _training;
}

void flux::NeatActivityTrainer::HyperNeatActivityTrainerImpl::LoadEvolutionState(std::istream &stream)
{
    boost::archive::binary_iarchive ar(stream);

    ar >> _training;

    //TODO: wipe current evaluations & epoch
}

void flux::NeatActivityTrainer::HyperNeatActivityTrainerImpl::Step(flux::float_fl delta)
{
    if (IsEpochCompleted())
    {
        StartEpoch();
    }

    _epochEvaluationTime += delta;
    _totalEvaluationTime += delta;
    for (int i = _currentEvaluations.size() - 1; i >= 0; i--)
    {
        _currentEvaluations[i].Entity->Step();

        if (_currentEvaluations[i].FitnessEvaluatorUnit->IsEvaluationCompleted())
        {
            auto &genome = _training.Genomes()[_currentEvaluations[i].Index];
            genome.Fitness() = _currentEvaluations[i].FitnessEvaluatorUnit->GetAggregatedFitness();

            if (genome.Fitness() > _bestChampion.Fitness())
            {
                _bestChampion = genome;
            }

            _totalEvaluations++;
            _currentEvaluations[i].Entity->ResetActivityBound();
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

void flux::NeatActivityTrainer::HyperNeatActivityTrainerImpl::StartEpoch()
{
    for (auto i = 0; i < _training.Genomes().size(); ++i)
    {
        EvaluationEntry entry;
        entry.Index = i;
        _evaluationQueue.emplace_back(entry);
    }
}

void flux::NeatActivityTrainer::HyperNeatActivityTrainerImpl::EndEpoch()
{
    _meanFitness = 0;
    for (const auto &genome : _training.Genomes())
    {
        _meanFitness += genome.Fitness();
    }

    _meanFitness /= _training.Genomes().size();
    _meanEvaluationDuration = _epochEvaluationTime / _training.Genomes().size();

    _epochEvaluationTime = 0.01;
    _training.FinalizePreEvaluatedStep(_epochNumber, _currentChampion);
	_meanComplexity = _training.MeanComplexity();
	_epochNumber++;

    _championEntityDescriptor.Fitness = -1;
    _currentEntitiesDescriptors.clear();
    std::vector<std::vector<mlpack::hyperneat::Genome<>>> species = _training.SpeciesClusters();
    int32_t id = 0;
    int32_t specieId = 0;

    for (const auto &specieList : species)
    {
        for (const auto &genome : specieList)
        {
            _currentEntitiesDescriptors.push_back(CreateDescriptorOf(id, specieId, genome));
            id++;

            if (genome.Fitness() > _championEntityDescriptor.Fitness)
            {
                _championEntityDescriptor = _currentEntitiesDescriptors[_currentEntitiesDescriptors.size() - 1];
            }
        }

        specieId++;
    }
}

void flux::NeatActivityTrainer::HyperNeatActivityTrainerImpl::EmplaceForEvaluation(EvaluationEntry entry)
{
    std::shared_ptr<IContext> slot = _trainingPool->RetrieveContext(false);
    entry.FitnessEvaluatorUnit = std::static_pointer_cast<IEvaluationOutputUnit>(_trainingFitnessUnitProto->Clone(slot));
    entry.Entity = std::static_pointer_cast<IBlackBox>(_trainingProto->Clone(slot));
    entry.Entity->AddOutput(entry.FitnessEvaluatorUnit);

    std::stringstream stream;
    boost::archive::text_oarchive oar(stream);
    oar << _training.Genomes()[entry.Index];

    entry.Entity->UpdateChildScheme(_targetId, stream);
    entry.Entity->SetActivityBound(_targetId);

    _currentEvaluations.emplace_back(entry);
}


flux::float_fl flux::NeatActivityTrainer::HyperNeatActivityTrainerImpl::GetBestFitness() const
{
    return _bestChampion.Fitness();
}

flux::float_fl flux::NeatActivityTrainer::HyperNeatActivityTrainerImpl::GetChampionFitness() const
{
    return _currentChampion.Fitness();
}

flux::NeatEntityDescriptor flux::NeatActivityTrainer::HyperNeatActivityTrainerImpl::GetChampionEntity() const
{
    return _championEntityDescriptor;
}

std::vector<flux::NeatEntityDescriptor> flux::NeatActivityTrainer::HyperNeatActivityTrainerImpl::GetCurrentEntities() const
{
    return _currentEntitiesDescriptors;
}

flux::float_fl flux::NeatActivityTrainer::HyperNeatActivityTrainerImpl::GetMeanFitness() const
{
    return _meanFitness;
}

flux::float_fl flux::NeatActivityTrainer::HyperNeatActivityTrainerImpl::GetMeanComplexity() const
{
    return _meanComplexity;
}

flux::float_fl flux::NeatActivityTrainer::HyperNeatActivityTrainerImpl::GetMeanEvaluationDuration() const
{
    return _meanEvaluationDuration;
}

flux::float_fl flux::NeatActivityTrainer::HyperNeatActivityTrainerImpl::GetEvaluationPerSec() const
{
    return _totalEvaluations / _totalEvaluationTime;
}

flux::float_fl flux::NeatActivityTrainer::HyperNeatActivityTrainerImpl::GetTotalEvaluations() const
{
    return _totalEvaluations;
}

flux::float_fl flux::NeatActivityTrainer::HyperNeatActivityTrainerImpl::GetTotalEvaluationTime() const
{
    return _totalEvaluationTime;
}

flux::NeatEntityDescriptor flux::NeatActivityTrainer::HyperNeatActivityTrainerImpl::CreateDescriptorOf(int32_t id, int32_t specieId, mlpack::hyperneat::Genome<> genome) const
{
    auto *neurons = new NeatEntityDescriptor::Neuron[genome.NodeCount()];
    auto *connections = new NeatEntityDescriptor::NeuronConnection[genome.Complexity()];

    float_fl maxDepth = _evolutionParameters.IsAcyclic
            ? static_cast<float_fl>(*std::max_element(genome.nodeDepths.begin(), genome.nodeDepths.end()))
            : 1.0;

    neurons[0] = NeatEntityDescriptor::Neuron(0, NeatEntityDescriptor::BIAS, 0);
    for (int i = 1; i < genome.NodeCount(); i++)
    {
        auto type = i <= genome.InputNodeCount() ? NeatEntityDescriptor::INPUT :
                (i <= (genome.InputNodeCount() + genome.OutputNodeCount())
                ? NeatEntityDescriptor::OUTPUT : NeatEntityDescriptor::HIDDEN);

        neurons[i] = NeatEntityDescriptor::Neuron(i, type, _evolutionParameters.IsAcyclic
        ? genome.nodeDepths[i] / maxDepth
        : (type == NeatEntityDescriptor::OUTPUT ? 1 : (type == NeatEntityDescriptor::HIDDEN ? 0.5 : 0.0)));
    }

	int k = 0;
    for (auto i = 0; i < genome.connectionGeneList.size() && k < genome.Complexity(); i++)
    {
    	if (!genome.connectionGeneList[i].Enabled())
    	{
			continue;
    	}
    	
        connections[k] = NeatEntityDescriptor::NeuronConnection(
                genome.connectionGeneList[i].Source(),
                genome.connectionGeneList[i].Target(),
                genome.connectionGeneList[i].Weight(),
                genome.connectionGeneList[i].SourceActivation());
		k++;
    }

    if (!_evolutionParameters.IsAcyclic)
    {
        for (int j = 1; j < 5; j++)
        {
            for (int i = 1; i < genome.NodeCount(); i++)
            {
                if (neurons[i].Type != NeatEntityDescriptor::HIDDEN)
                {
                    continue;
                }

                float_fl weight = 0;
                int connCount = 0;
                for (int l = 1; l < genome.Complexity(); l++)
                {
                    if (connections[l].OriginId == i)
                    {
                        weight += neurons[connections[l].DestinationId].NormalizedPosition;
                        connCount++;
                    }
                    if (connections[l].DestinationId == i)
                    {
                        weight += neurons[connections[l].OriginId].NormalizedPosition;
                        connCount++;
                    }
                }

                neurons[i].NormalizedPosition = weight / connCount;
            }
        }
    }

    return { id, specieId, genome.Complexity(), genome.Fitness(), genome.NodeCount(), genome.Complexity(), neurons, connections };
}

void flux::NeatActivityTrainer::HyperNeatActivityTrainerImpl::DumpState(std::ostream &stream)
{
    boost::archive::text_oarchive ar(stream);
    auto genomes = _training.Genomes();
    for (const auto& genome : genomes)
    {
        ar << genome;
    }
}
