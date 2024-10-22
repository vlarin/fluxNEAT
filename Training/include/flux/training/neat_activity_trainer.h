//
// Copyright (c) 2019-2020 Temporal Games Inc. All rights reserved.
//

#ifndef FLUXNEAT_NEAT_ACTIVITY_TRAINER_H
#define FLUXNEAT_NEAT_ACTIVITY_TRAINER_H

#include <flux/common.h>

#include <flux/training/evaluation_output_unit.h>
#include <flux/training/neat_evolution_params.h>

#include <iostream>

namespace flux {

    /**
     * Defines fluxNEAT Activity simulation trainer
     */
    class FLUX_API NeatActivityTrainer
    {
    public:
        NeatActivityTrainer(size_t populationSize,
                size_t numSpecies,
                size_t parallelPoolSize,
                size_t complexityThreshold,
                size_t  maxSimplifyGeneration,
                NeatEvolutionParameters evolutionParameters,
                const std::shared_ptr<IActivityUnit> &target,
                const std::shared_ptr<IEvaluationOutputUnit>& trainingFitnessUnitProto,
                const std::shared_ptr<IBlackBox>& trainingProto,
                const std::shared_ptr<IContextRegistry>& trainingPool);

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

        NeatEntityDescriptor GetChampionEntity() const;
        std::vector<NeatEntityDescriptor> GetCurrentEntities() const;

        void DumpState(std::ostream &stream) const;
        void Step(float_fl delta);

        void SaveCurrentChampionActivity(std::ostream &stream) const;
        void SaveEvolutionState(std::ostream &stream) const;
        void LoadEvolutionState(std::istream &stream);

        ~NeatActivityTrainer();
    private:
        class NeatActivityTrainerImpl;
        std::unique_ptr<NeatActivityTrainerImpl> _impl;
    };
}

#endif //FLUXNEAT_NEAT_ACTIVITY_TRAINER_H
