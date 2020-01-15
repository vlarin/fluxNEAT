//
// Copyright (c) 2019-2020 Temporal Games Inc. All rights reserved.
//

#ifndef FLUXNEAT_NEAT_EVOLUTION_PARAMS_H
#define FLUXNEAT_NEAT_EVOLUTION_PARAMS_H

#include <flux/prereq.h>

namespace flux
{
    /**
     * Container of the NEAT evolution parameters for the trainer
     */
    struct FLUX_API NeatEvolutionParameters
    {
    public:
        double Bias;
        double InitialWeight;
        double WeightMutationProb;
        double WeightMutationSize;
        double BiasMutationProb;
        double BiasMutationSize;
        double NodeAdditionProb;
        double ConnAdditionProb;
        double ConnDeletionProb;
        double ElitismProportion;
        bool IsAcyclic;

        explicit NeatEvolutionParameters(double weightMutationProb = 0.8,
                double connAdditionProb = 0.505,
                double connDeletionProb = 0.5,
                double biasMutationProb = 0.7,
                double nodeAdditionProb = 0.2,
                double bias = 1,
                double initialWeight = 0,
                double weightMutationSize = 0.5,
                double biasMutationSize = 0.5,
                double elitismProportion = 0.1,
                bool isAcyclic = true);
    };

    /**
     * C Flat-style API for creating Evolution Parameters
     * @param bias The bias of the genome.
     * @param initialWeight The initial weight of the connections.
     * @param weightMutationProb The probability of a weight mutating.
     * @param weightMutationSize The degree to which the weight will mutate.
     * @param biasMutationProb The probability of a bias mutating.
     * @param biasMutationSize The degree to which the bias will mutate.
     * @param nodeAdditionProb The probability of a new node being added.
     * @param connAdditionProb The probability of a new connection being added.
     * @param connDeletionProb The probability of a connection being deleted.
     * @param isAcyclic Denotes whether or not the generated network is acyclic.
     * @return pointer to the configured evolution params
     */
#ifdef __cplusplus
    extern "C" {
#endif
    NeatEvolutionParameters *CreateEvolutionParameters(
             double bias,
             double initialWeight,
             double weightMutationProb,
             double weightMutationSize,
             double biasMutationProb,
             double biasMutationSize,
             double nodeAdditionProb,
             double connAdditionProb,
             double connDeletionProb,
             double elitismProportion,
             bool isAcyclic);
#ifdef __cplusplus
    };
#endif

    /**
     * C Flat-style API for deleting/releasing no longer needed evolution params
     */
#ifdef __cplusplus
    extern "C" {
#endif
    void DeleteEvolutionParameters(NeatEvolutionParameters *parameters);
#ifdef __cplusplus
    };
#endif
}

#endif //FLUXNEAT_NEAT_EVOLUTION_PARAMS_H
