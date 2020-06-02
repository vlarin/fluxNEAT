//
// Copyright (c) 2019-2020 Temporal Games Inc. All rights reserved.
//

#ifndef FLUXNEAT_HNEAT_ACTIVITY_TRAINER_IMPL_H
#define FLUXNEAT_HNEAT_ACTIVITY_TRAINER_IMPL_H

#include <flux/training/neat_activity_trainer.h>
#include <flux/neat/neat_entity_descriptor.h>

#include <mlpack/core.hpp>
#include <mlpack/methods/hyperneat/hyperneat.hpp>


namespace flux {

    class HyperNeatEmptyTask
    {
    public:
        //std::vector<mlpack::hyperneat::ConnectionGene> StartingGenome() { }

        double Evaluate(mlpack::hyperneat::Genome<> &genome) { return 0; }
        template<typename Archive>
        void serialize(Archive& ar, const unsigned int /* version */) {}
    };

    class NeatActivityTrainer::HyperNeatActivityTrainerImpl
    {
    public:
        HyperNeatActivityTrainerImpl(size_t populationSize,
        size_t numSpecies,
        size_t parallelPoolSize,
        size_t complexityThreshold,
        size_t  maxSimplifyGeneration,
        size_t cppnDimensions,
        NeatEvolutionParameters evolutionParameters,
        std::vector<size_t> substrateLayers,
        std::shared_ptr<IEvaluationOutputUnit> trainingFitnessUnitProto,
        std::shared_ptr<IBlackBox> trainingProto,
        std::shared_ptr<IContextRegistry> trainingPool,
        const std::shared_ptr<IActivityUnit> &target);

        bool IsEpochCompleted() const;
        size_t GetCurrentEpoch() const;

		bool IsComplexifying() const;
        float_fl GetBestFitness() const;
        float_fl GetChampionFitness() const;
        float_fl GetMeanFitness() const;
        float_fl GetMeanComplexity() const;
        float_fl GetMeanEvaluationDuration() const;

        float_fl GetEvaluationPerSec() const;
        float_fl GetTotalEvaluations() const;
        float_fl GetTotalEvaluationTime() const;

        const NeatEvolutionParameters &GetEvolutionParameters() const;

        void Step(float_fl delta);

        void DumpState(std::ostream &stream);

        NeatEntityDescriptor GetChampionEntity() const;
        std::vector<NeatEntityDescriptor> GetCurrentEntities() const;

        void SaveCurrentChampionActivity(std::ostream &stream) const;
        void SaveEvolutionState(std::ostream &stream) const;
        void LoadEvolutionState(std::istream &stream);

        ~HyperNeatActivityTrainerImpl() = default;
    private:
        struct EvaluationEntry
        {
            size_t Index;
            std::shared_ptr<IBlackBox> Entity;
            std::shared_ptr<IEvaluationOutputUnit> FitnessEvaluatorUnit;
        };

		size_t _epochNumber = 0;

        size_t _parallelPoolSize;
        size_t _populationSize;

        std::string _targetId;
        NeatEvolutionParameters _evolutionParameters;
        std::vector<size_t> _substrateLayers;

        float_fl _meanFitness = 0;
        float_fl _meanComplexity = 0;
        float_fl _totalEvaluations = 0;
        float_fl _totalEvaluationTime = 0.01;
        float_fl _epochEvaluationTime = 0.01;
        float_fl _meanEvaluationDuration = 0;

        std::vector<NeatEntityDescriptor> _currentEntitiesDescriptors;
        NeatEntityDescriptor _championEntityDescriptor;

        mlpack::hyperneat::Genome<> _bestChampion;
        mlpack::hyperneat::Genome<> _currentChampion;
        std::vector<EvaluationEntry> _currentEvaluations;
        std::vector<EvaluationEntry> _evaluationQueue;

        mlpack::hyperneat::HyperNEAT<HyperNeatEmptyTask> _training;
        std::shared_ptr<IBlackBox> _trainingProto;
        std::shared_ptr<IContextRegistry> _trainingPool;
        std::shared_ptr<IEvaluationOutputUnit> _trainingFitnessUnitProto;

        void StartEpoch();
        void EndEpoch();

        void EmplaceForEvaluation(EvaluationEntry entry);

        NeatEntityDescriptor CreateDescriptorOf(int32_t id, int32_t specieId, mlpack::hyperneat::Genome<> genome) const;
    };
}

#endif //FLUXNEAT_NEAT_ACTIVITY_TRAINER_IMPL_H
