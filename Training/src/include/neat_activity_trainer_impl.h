//
// Copyright (c) 2019-2020 Temporal Games Inc. All rights reserved.
//

#ifndef FLUXNEAT_NEAT_ACTIVITY_TRAINER_IMPL_H
#define FLUXNEAT_NEAT_ACTIVITY_TRAINER_IMPL_H

#include <flux/training/neat_activity_trainer.h>

#include <mlpack/core.hpp>
#include <mlpack/methods/neat/neat.hpp>


namespace flux {

    class NeatEmptyTask
    {
    public:
        //std::vector<mlpack::neat::ConnectionGene> StartingGenome() { }

        double Evaluate(mlpack::neat::Genome<> &genome) { return 0; }
        template<typename Archive>
        void serialize(Archive& ar, const unsigned int /* version */) {}
    };

    class NeatActivityTrainer::NeatActivityTrainerImpl
    {
    public:
        NeatActivityTrainerImpl(size_t populationSize,
        size_t numSpecies,
        size_t parallelPoolSize,
        size_t complexityThreshold,
        size_t  maxSimplifyGeneration,
        NeatEvolutionParameters evolutionParameters,
        std::shared_ptr<IEvaluationOutputUnit> trainingFitnessUnitProto,
        std::shared_ptr<IBlackBox> trainingProto,
        std::shared_ptr<IContextRegistry> trainingPool,
        const std::shared_ptr<IActivityUnit> &target);

        bool IsEpochCompleted() const;
        float_t GetChampionFitness() const;

        void Step();

        void SaveCurrentChampionActivity(std::ostream &stream) const;
        void SaveEvolutionState(std::ostream &stream) const;
        void LoadEvolutionState(std::istream &stream);

        ~NeatActivityTrainerImpl() = default;
    private:
        struct EvaluationEntry
        {
            size_t Index;
            std::shared_ptr<IBlackBox> Entity;
            std::shared_ptr<IEvaluationOutputUnit> FitnessEvaluatorUnit;
        };

        size_t _parallelPoolSize;
        size_t _populationSize;

        std::string _targetId;

        mlpack::neat::Genome<> _currentChampion;
        std::vector<EvaluationEntry> _currentEvaluations;
        std::vector<EvaluationEntry> _evaluationQueue;

        mlpack::neat::NEAT<NeatEmptyTask> _training;
        std::shared_ptr<IBlackBox> _trainingProto;
        std::shared_ptr<IContextRegistry> _trainingPool;
        std::shared_ptr<IEvaluationOutputUnit> _trainingFitnessUnitProto;

        void StartEpoch();
        void EndEpoch();

        void EmplaceForEvaluation(EvaluationEntry entry);
    };
}

#endif //FLUXNEAT_NEAT_ACTIVITY_TRAINER_IMPL_H
